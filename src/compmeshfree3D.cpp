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
#include "compmeshfree3D.h"

#include "compnode.h"

#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkDelaunay3D.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkLookupTable.h>

CompMeshfree3D::CompMeshfree3D()
: currentIndex(0),
  minScalar(0),
  maxScalar(0),
  vPoints(0),
  boundary(0),
  profile(0),
  del3D(0),
  aDataSetMapper(0),
  anActor(0)
{
}


CompMeshfree3D::~CompMeshfree3D()
{
  if(vPoints) vPoints->Delete();
//  if(boundary) boundary->Delete();
  if(profile) profile->Delete();
  if(del3D) del3D->Delete();
  if (aDataSetMapper) aDataSetMapper->Delete();
  if (anActor) anActor->Delete();

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


void CompMeshfree3D::initialize( std::map<int, CompNode>& nodes_in,
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
  
  //  boundary = vtkCellArray::New();
  //  if(boundaryNodes.size() != 0){
  //    boundary->InsertNextCell(boundaryNodes.size());
  //    for (int i=0; i<boundaryNodes.size(); ++i){
  //      boundary->InsertCellPoint( boundaryNodes[i]-firstNode_in );
  //    }
  //  }
  
  
  profile = vtkPolyData::New();
  profile->SetPoints( vPoints );
  //  profile->SetPolys( boundary );
  
  del3D = vtkDelaunay3D::New();
  del3D->SetAlpha(0.5);
#if VTK_MAJOR_VERSION > 5
  del3D->SetInputData( profile );
#else
  del3D->SetInput( profile );
#endif
  //  del3D->SetSource( profile );
  
  aDataSetMapper = vtkDataSetMapper::New();
#if VTK_MAJOR_VERSION > 5
  aDataSetMapper->SetInputData( del3D->GetOutput() );
#else
  aDataSetMapper->SetInput( del3D->GetOutput() );
#endif
  
  anActor = vtkActor::New();
  anActor->SetMapper(aDataSetMapper);
  //   anActor->GetProperty()->SetOpacity(0.5); // translucent !!!
  //   anActor->AddPosition(2, 0, 0);
  anActor->GetProperty()->SetDiffuseColor(1.0, 0.3, 0.3);
  
}

void CompMeshfree3D::addToRender(vtkRenderer * renderer_in)
{
  renderer_in->AddActor( anActor );

}

void CompMeshfree3D::removeFromRender(vtkRenderer * renderer_in)
{
  renderer_in->RemoveActor( anActor );
}

void CompMeshfree3D::updatePoints()
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

void CompMeshfree3D::readResults( std::ifstream & input, int timeSize )
{
  int lastField(0), oldFields( scalarFields.size() );
  if(oldFields != 0) lastField = oldFields-1;
  double res1, res2, res3, res4, res5, res6;
  if (timeSize == 2) --timeSize;

  for( int i=0; i<timeSize; ++i ){
    if(scalarFields.size() < i+1){
      scalarFields.push_back( *(new std::vector< vtkFloatArray* >) ); // Container
      scalarFields[i].push_back( vtkFloatArray::New() ); // always zero
    }
    scalarFields[i].push_back( vtkFloatArray::New() ); // sigma_x
    scalarFields[i].push_back( vtkFloatArray::New() ); // sigma_y
    scalarFields[i].push_back( vtkFloatArray::New() ); // sigma_z
    scalarFields[i].push_back( vtkFloatArray::New() ); // sigma_xy
    scalarFields[i].push_back( vtkFloatArray::New() ); // sigma_yz
    scalarFields[i].push_back( vtkFloatArray::New() ); // sigma_xz
    scalarFields[i].push_back( vtkFloatArray::New() ); // sigma_VM

    if (timeSize == 1){//for static case
      if(scalarFields.size() < i + 2){
        scalarFields.push_back( *(new std::vector< vtkFloatArray* >) ); // Container
        scalarFields[1].push_back( vtkFloatArray::New() ); // always zero
      }
    
      scalarFields[1].push_back( vtkFloatArray::New() ); // sigma_x
      scalarFields[1].push_back( vtkFloatArray::New() ); // sigma_y
      scalarFields[1].push_back( vtkFloatArray::New() ); // sigma_z
      scalarFields[1].push_back( vtkFloatArray::New() ); // sigma_xy
      scalarFields[1].push_back( vtkFloatArray::New() ); // sigma_yz
      scalarFields[1].push_back( vtkFloatArray::New() ); // sigma_xz
      scalarFields[1].push_back( vtkFloatArray::New() ); // sigma_VM
    }

    std::map<int,CompNode*>::iterator itNodes;
    for( itNodes = nodes.begin();
        itNodes!= nodes.end();
        ++itNodes
      )
    {
      input >> res1 >> res2 >> res3 >> res4 >> res5 >> res6;
     
      if(oldFields==0)
        scalarFields[i][0]->InsertNextValue( 0. ); // no contour

      scalarFields[i][lastField+1]->InsertNextValue( res1 ); // sigma_x
      scalarFields[i][lastField+2]->InsertNextValue( res2 ); // sigma_y
      scalarFields[i][lastField+3]->InsertNextValue( res3 ); // sigma_z
      scalarFields[i][lastField+4]->InsertNextValue( res4 ); // sigma_xy
      scalarFields[i][lastField+5]->InsertNextValue( res5 ); // sigma_yz
      scalarFields[i][lastField+6]->InsertNextValue( res6 ); // sigma_xz
      scalarFields[i][lastField+7]->InsertNextValue( sqrt(0.5*( pow(res1-res2,2) 
                                                              + pow(res2-res3,2)
                                                              + pow(res3-res1,2)
                                                              + 6*( pow(res4,2) 
                                                                  + pow(res5,2)
                                                                  + pow(res6,2) ) )
                                                         ) ); // sigma_VM

      if (timeSize == 1){//for static case
        if(oldFields==0)
          scalarFields[1][0]->InsertNextValue( 0. ); // no contour
          
        scalarFields[1][lastField+1]->InsertNextValue( res1 ); // sigma_x
        scalarFields[1][lastField+2]->InsertNextValue( res2 ); // sigma_y
        scalarFields[1][lastField+3]->InsertNextValue( res3 ); // sigma_z
        scalarFields[1][lastField+4]->InsertNextValue( res4 ); // sigma_xy
        scalarFields[1][lastField+5]->InsertNextValue( res5 ); // sigma_yz
        scalarFields[1][lastField+6]->InsertNextValue( res6 ); // sigma_xz
        scalarFields[1][lastField+7]->InsertNextValue( sqrt(0.5*( pow(res1-res2,2) 
                                                              + pow(res2-res3,2)
                                                              + pow(res3-res1,2)
                                                              + 6*( pow(res4,2) 
                                                                  + pow(res5,2)
                                                                  + pow(res6,2) ) )
                                                         ) ); // sigma_VM
      }
    }
  }

  drawScalarField(0,0);
}

void CompMeshfree3D::drawScalarField(int index, int step)
{
  currentIndex = index;
  profile->GetPointData()->SetScalars( scalarFields[step][currentIndex] );
  findMinMaxScalars();
  aDataSetMapper->SetScalarRange(minScalar, maxScalar);
  
}

void CompMeshfree3D::updateScalarField(int step)
{
  if( scalarFields.size() > 0){
    profile->GetPointData()->SetScalars( scalarFields[step][currentIndex] );
    findMinMaxScalars();
    aDataSetMapper->SetScalarRange(minScalar, maxScalar);
  }
  
}

void CompMeshfree3D::setLookUpTable( vtkLookupTable* table_in )
{ 
  aDataSetMapper->SetLookupTable( table_in); 
}

void CompMeshfree3D::findMinMaxScalars()
{
  double min_max[2];
  if( scalarFields.size()>0 ){ // set first values
    scalarFields[0][currentIndex]->GetRange(min_max);
    minScalar = min_max[0];
    maxScalar = min_max[1];
  }
  cout << "scalarFields.size() "<< scalarFields.size() << endl;  
  cout << "scalarFields[0][0] "<< scalarFields[0][0] << endl;
  for( int i=1; i<scalarFields.size(); ++i ){
    cout << "i, currentIndex = " << i << currentIndex << endl;
    cout << "scalarFields[i][currentIndex] "<< scalarFields[i][currentIndex] << endl;
    scalarFields[i][currentIndex]->GetRange(min_max);
    if(minScalar > min_max[0]) minScalar = min_max[0];
    if(maxScalar < min_max[1]) maxScalar = min_max[1];
  }
}
