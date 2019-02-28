/***************************************************************************
 *   Copyright (C) 2007 by Daniel Iglesias   *
 *   diglesias@mecanica.upm.es   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "compmeshfree.h"

#include "compnode.h"

#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkDelaunay2D.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkLookupTable.h>

CompMeshfree::CompMeshfree()
: currentIndex(0),
  minScalar(0),
  maxScalar(0),
  vPoints(0),
  boundary(0),
  profile(0),
  del2D(0),
  aDataSetMapper(0),
  anActor(0)
{
}

CompMeshfree::~CompMeshfree()
{
  if(vPoints) vPoints->Delete();
  if(boundary) boundary->Delete();
  if(profile) profile->Delete();
  if(del2D) del2D->Delete();
  if(aDataSetMapper) aDataSetMapper->Delete();
  if(anActor) anActor->Delete();

  std::vector< std::vector< vtkFloatArray* > >::iterator itScalarFields;
  std::vector< vtkFloatArray* >::iterator itOneField;
  for( itScalarFields = scalarFields.begin();
       itScalarFields!= scalarFields.end();
       ++itScalarFields
     )
  {
    for( itOneField = itScalarFields->begin();
         itOneField!= itScalarFields->end();
         ++itOneField
       )
    {
      (*itOneField)->Delete();
      *itOneField = 0;
    }
  }
}


void CompMeshfree::initialize( std::map<int, CompNode>& nodes_in,
                              int firstNode_in,
                              int lastNode_in,
                              std::vector<int> boundaryNodes
                              )
{
  int j=0;
  for(int i=firstNode_in; i<=lastNode_in; ++i){
    nodes[j] = &(nodes_in[i]);
    ++j;
  }
  
  vPoints = vtkPoints::New();
  vPoints->SetNumberOfPoints( nodes.size() );
  
  for(std::map<int,CompNode*>::iterator it = nodes.begin();
      it != nodes.end();
      ++it)
  {
    vPoints->InsertPoint( (*it).first,
                         (*it).second->getx(),
                         (*it).second->gety(),
                         (*it).second->getz()
                         );
  }
  
  
  profile = vtkPolyData::New();
  profile->SetPoints( vPoints );

  if(boundaryNodes.size() != 0){
    boundary = vtkCellArray::New();
    boundary->InsertNextCell(boundaryNodes.size());
    for (int i=0; i<boundaryNodes.size(); ++i){
      boundary->InsertCellPoint( boundaryNodes[i]-firstNode_in );
    }
    profile->SetPolys( boundary );
  }
  
  del2D = vtkDelaunay2D::New();
  del2D->SetAlpha(5.);
#if VTK_MAJOR_VERSION > 5
  del2D->SetInputData( profile );
  del2D->SetSourceData( profile );
#else
  del2D->SetInput( profile );
  del2D->SetSource( profile );
#endif
  del2D->Update();
  
  aDataSetMapper = vtkDataSetMapper::New();
#if VTK_MAJOR_VERSION > 5
  aDataSetMapper->SetInputConnection( del2D->GetOutputPort() );
#else
  aDataSetMapper->SetInput( del2D->GetOutput() );
#endif
  
  anActor = vtkActor::New();
  anActor->SetMapper(aDataSetMapper);
  //   anActor->GetProperty()->SetOpacity(0.5); // translucent !!!
  //   anActor->AddPosition(2, 0, 0);
  anActor->GetProperty()->SetDiffuseColor(1.0, 0.3, 0.3);
  
}


void CompMeshfree::addToRender(vtkRenderer * renderer_in)
{
  if(anActor) renderer_in->AddActor( anActor );
}

void CompMeshfree::removeFromRender(vtkRenderer * renderer_in)
{
  if(anActor) renderer_in->RemoveActor( anActor );
}

void CompMeshfree::updatePoints()
{
  for(std::map<int,CompNode*>::iterator it = nodes.begin();
      it != nodes.end();
      ++it)
  {
    vPoints->SetPoint( (*it).first,
                      (*it).second->getx(),
                      (*it).second->gety(),
                      (*it).second->getz()
                    );
  }
  vPoints->Modified();
//  boundary->Modified();

}

void CompMeshfree::readEnergy( std::ifstream & input, int timeSize )
{
//   double ePot, eKin, eEla, eTot;
//   if (timeSize == 2) --timeSize; // Static case
// 
//   for( int i=0; i<timeSize; ++i ){
//     input >> ePot >> eKin >> eEla >> eTot;
//     energyFields.push_back( std::vector<double>() );
//     energyFields[i].push_back( ePot ); // Potential
//     energyFields[i].push_back( eKin ); // Kinetic
//     energyFields[i].push_back( eEla ); // Elastic
//     energyFields[i].push_back( eTot ); // Total
//   }  
}

// Does not allow to join different analysis if they need multiple readings
// But allows to have the stress and temperatures in different blocks
void CompMeshfree::readResults( std::ifstream & input, int timeSize )
{
  int lastField(0), oldFields( scalarFields.size() );
  if(oldFields != 0) lastField = oldFields-1;
  double res1, res2, res3;
  if (timeSize == 2) --timeSize;

  for( int i=0; i<timeSize; ++i ){
    if(scalarFields.size() < i+1){
      scalarFields.push_back( *(new std::vector< vtkFloatArray* >) ); // Container
      scalarFields[i].push_back( vtkFloatArray::New() ); // always zero
    }
    scalarFields[i].push_back( vtkFloatArray::New() ); // sigma_x
    scalarFields[i].push_back( vtkFloatArray::New() ); // sigma_y
    scalarFields[i].push_back( vtkFloatArray::New() ); // sigma_xy
    scalarFields[i].push_back( vtkFloatArray::New() ); // sigma_VM

    if (timeSize == 1){//for static case
      if(scalarFields.size() < i + 2){
        scalarFields.push_back( *(new std::vector< vtkFloatArray* >) ); // Container
        scalarFields[1].push_back( vtkFloatArray::New() ); // always zero
      }
    
      scalarFields[1].push_back( vtkFloatArray::New() ); // sigma_x
      scalarFields[1].push_back( vtkFloatArray::New() ); // sigma_y
      scalarFields[1].push_back( vtkFloatArray::New() ); // sigma_xy
      scalarFields[1].push_back( vtkFloatArray::New() ); // sigma_VM
    }

    std::map<int,CompNode*>::iterator itNodes;
    for( itNodes = nodes.begin();
        itNodes!= nodes.end();
        ++itNodes
      )
    {
      input >> res1 >> res2 >> res3;

      if(oldFields==0)
        scalarFields[i][0]->InsertNextValue( 0. ); // no contour

      scalarFields[i][lastField+1]->InsertNextValue( res1 ); // sigma_x
      scalarFields[i][lastField+2]->InsertNextValue( res2 ); // sigma_y
      scalarFields[i][lastField+3]->InsertNextValue( res3 ); // sigma_xy
      scalarFields[i][lastField+4]->InsertNextValue( sqrt(  pow(res1,2)
                                                          + pow(res2,2)
                                                          - res1 * res2
                                                          + 3*pow(res3,2) ) ); // sigma_VM

      if (timeSize == 1){//for static case
        if(oldFields==0)
          scalarFields[1][0]->InsertNextValue( 0. ); // no contour
	  
        scalarFields[1][lastField+1]->InsertNextValue( res1 ); // sigma_x
        scalarFields[1][lastField+2]->InsertNextValue( res2 ); // sigma_y
        scalarFields[1][lastField+3]->InsertNextValue( res3 ); // sigma_xy
        scalarFields[1][lastField+4]->InsertNextValue( sqrt(  pow(res1,2)
                                                            + pow(res2,2)
                                                            - res1 * res2
                                                            + 3*pow(res3,2) ) ); // sigma_VM
      }
    }
  }

  drawScalarField(0,0);
}

// Does not allow to join different analysis if they need multiple readings
// But allows to have the stress and temperatures in different blocks
// STRESS must be read AFTER
void CompMeshfree::readTemps( std::ifstream & input, int timeSize )
{
  double res1, res2(0.);
    int node_max(0), node_count(0);
    std::ofstream outFile("MAX_TEMPS.dat");

    if (timeSize == 2) --timeSize;

  for( int i=0; i<timeSize; ++i ){
    if(scalarFields.size() < i+1){
      scalarFields.push_back( std::vector< vtkFloatArray* >() ); // Container
      scalarFields[i].push_back( vtkFloatArray::New() ); // always zero
    }
    scalarFields[i].push_back( vtkFloatArray::New() ); // Temp

    if (timeSize == 1){//for static case
    if(scalarFields.size() < i+2){
      scalarFields.push_back( std::vector< vtkFloatArray* >() ); // Container
      scalarFields[i].push_back( vtkFloatArray::New() ); // always zero
    }
      scalarFields[1].push_back( vtkFloatArray::New() ); // Temp
    }

    std::map<int,CompNode*>::iterator itNodes;
    for( itNodes = nodes.begin();
        itNodes!= nodes.end();
        ++itNodes
      )
    {
        ++node_count;
      input >> res1;
        if (res1 > res2){
            res2 = res1;
            node_max = node_count;
        }

      scalarFields[i][0]->InsertNextValue( 0. ); // no contour
      scalarFields[i][1]->InsertNextValue( res1 ); // Temp

      if (timeSize == 1){//for static case
        scalarFields[1][0]->InsertNextValue( 0. ); // no contour
        scalarFields[1][1]->InsertNextValue( res1 ); // Temp
      }
    }
      outFile << "MAXIMUM TEMPERATURE AT STEP " << i << ", NODE " << node_max << " = " << res2 << endl;
  }

  drawScalarField(0,0);
}

void CompMeshfree::drawScalarField(int index, int step)
{
//  double min_max[2];
  currentIndex = index;
  profile->GetPointData()->SetScalars( scalarFields[step][currentIndex] );
  findMinMaxScalars();
//  aDataSetMapper->SetScalarRange(0, 1100);
  aDataSetMapper->SetScalarRange(minScalar, maxScalar);
 
}

void CompMeshfree::updateScalarField(int step)
{
//  double min_max[2];
  if( scalarFields.size() > 0){
    profile->GetPointData()->SetScalars( scalarFields[step][currentIndex] );
    findMinMaxScalars();
//    aDataSetMapper->SetScalarRange(0, 1100);
    aDataSetMapper->SetScalarRange(minScalar, maxScalar);
  }
  
}


void CompMeshfree::setLookUpTable( vtkLookupTable* table_in )
{ 
  aDataSetMapper->SetLookupTable( table_in); 
}


void CompMeshfree::findMinMaxScalars()
{
  double min_max[2];
  if( scalarFields.size()>0 ){ // set first values
    scalarFields[0][currentIndex]->GetRange(min_max);
    minScalar = min_max[0];
    maxScalar = min_max[1];
  }
  
  for( int i=1; i<scalarFields.size(); ++i ){
    scalarFields[i][currentIndex]->GetRange(min_max);
    if(minScalar > min_max[0]) minScalar = min_max[0];
    if(maxScalar < min_max[1]) maxScalar = min_max[1];
  }
}
