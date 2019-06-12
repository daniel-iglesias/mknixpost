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
#include <vtkPolyDataMapper.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkDelaunay3D.h>
#include <vtkTriangle.h>
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
                              , double* color )
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
  //  profile->SetPolys( boundary );

  del3D = vtkDelaunay3D::New();
//   del3D->SetAlpha(5);
//   del3D->SetInput( profile );
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
  anActor->GetProperty()->SetColor(color[0], color[1], color[2]);
//   anActor->GetProperty()->SetDiffuseColor(1.0, 0.3, 0.3);

}

void CompMeshfree3D::readBoundary(ifstream& input, int trianglesNumber )
{
  int n1, n2, n3, j;

  boundary = vtkCellArray::New();
    for(j=0; j<trianglesNumber; ++j){
      input >> n1 >> n2 >> n3;
      triangles.push_back( vtkTriangle::New() );
      triangles.back()->GetPointIds()->SetId ( 0, n1 );
      triangles.back()->GetPointIds()->SetId ( 1, n2 );
      triangles.back()->GetPointIds()->SetId ( 2, n3 );
      //add the triangles to the list of triangles
      boundary->InsertNextCell( triangles[j] );
    }
  profile->SetPolys( boundary );
    boundaryMapper = vtkPolyDataMapper::New();
#if VTK_MAJOR_VERSION > 5
    boundaryMapper->SetInputData( profile );
#else
    boundaryMapper->SetInput( profile );
#endif
    anActor->SetMapper(boundaryMapper);
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

// Does not allow to join different analysis if they need multiple readings
// But allows to have the stress and temperatures in different blocks
// STRESS must be read AFTER
void CompMeshfree3D::readTemps( std::ifstream & input, int timeSize )
{
  double res1, res2(0.);
  int node_max, node_count(0);
  std::ofstream outFile("MAX_TEMPS.dat");

  if (timeSize == 2) --timeSize;

  for( int i=0; i<timeSize; ++i ){
    if(scalarFields.size() < i+1){
      scalarFields.push_back( std::vector< vtkFloatArray* >() ); // Container
      scalarFields[i].push_back( vtkFloatArray::New() ); // always zero
    }
    scalarFields[i].push_back( vtkFloatArray::New() ); // Temp

    if (timeSize == 1){//for static case
      if(scalarFields.size() < 2){
	scalarFields.push_back( std::vector< vtkFloatArray* >() ); // Container
	scalarFields[1].push_back( vtkFloatArray::New() ); // always zero
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
// 	  cout << node_count << " " << res1 << ", " <<endl;

	  scalarFields[0][0]->InsertNextValue( 0. ); // no contour
	  scalarFields[0][1]->InsertNextValue( res1 ); // Temp

	  if (timeSize == 1){//for static case
	    scalarFields[1][0]->InsertNextValue( 0. ); // no contour
	    scalarFields[1][1]->InsertNextValue( res1 ); // Temp
	  }
	}
	outFile << "MAXIMUM TEMPERATURE AT STEP " << i << ", NODE " << node_max << " = " << res2 << endl;
	res2=0;
  }
//   cout << "scalarFields[0][0].components =" << scalarFields[0][0]->GetNumberOfTuples() << endl;
//   cout << "scalarFields[0][1].components =" << scalarFields[0][1]->GetNumberOfTuples() << endl;
//   cout << "scalarFields[1][0].components =" << scalarFields[1][0]->GetNumberOfTuples() << endl;
//   cout << "scalarFields[1][1].components =" << scalarFields[1][1]->GetNumberOfTuples() << endl;
  drawScalarField(0,0);
}

void CompMeshfree3D::drawScalarField(int index, int step)
{
  currentIndex = index;
  updateScalarField(step);
}

void CompMeshfree3D::updateScalarField(int step)
{
  if( scalarFields.size() > 0){
    profile->GetPointData()->SetScalars( scalarFields[step][currentIndex] );
    findMinMaxScalars();
    if(boundaryMapper)
      boundaryMapper->SetScalarRange(minScalar, maxScalar);
    else
      aDataSetMapper->SetScalarRange(minScalar, maxScalar);
  }

}

void CompMeshfree3D::setLookUpTable( vtkLookupTable* table_in )
{
  if(boundaryMapper)
    boundaryMapper->SetLookupTable( table_in);
  else
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
//   cout << "scalarFields.size() "<< scalarFields.size() << endl;
//   cout << "scalarFields[0][0] "<< scalarFields[0][0] << endl;
  for( int i=1; i<scalarFields.size(); ++i ){
//     cout << "i, currentIndex = " << i << currentIndex << endl;
//     cout << "scalarFields[i][currentIndex] "; scalarFields[i][currentIndex]->Print(std::cout);
    scalarFields[i][currentIndex]->GetRange(min_max);
    if(minScalar > min_max[0]) minScalar = min_max[0];
    if(maxScalar < min_max[1]) maxScalar = min_max[1];
//     cout << "minScalar " << minScalar << ", min_max[0] " << min_max[0] << endl;
//     cout << "maxScalar " << maxScalar << ", min_max[1] " << min_max[1] << endl;
  }
}
