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
#include "comprigid3D.h"

#include "compnode.h"

#include <vtkLineSource.h>
#include <vtkTubeFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
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

CompRigid3D::CompRigid3D()
{
}

CompRigid3D::CompRigid3D( std::string name_in
			, int mat_in
			, CompNode* nodeA_in
			, CompNode* nodeB_in
			, CompNode* nodeC_in
                        , CompNode* nodeD_in
                          )
  : name(name_in)
    , mat(mat_in)
    , nodeA(nodeA_in)
    , nodeB(nodeB_in)
    , nodeC(nodeC_in)
    , nodeD(nodeD_in)
    , numPoints(0)
    , currentIndex(0)
    , boundary(0)
    , profile(0)
    , boundaryMapper(0)
    , aDataSetMapper(0)
    , anActor(0)
{
  for(int i=0; i<3; ++i){ 
    line.push_back(vtkLineSource::New());
    line.back()->SetResolution(10);

    lineTubes.push_back( vtkTubeFilter::New() );
    lineTubes.back()->SetInputConnection(line.back()->GetOutputPort());
    lineTubes.back()->SetRadius(0.05E-2);
    lineTubes.back()->SetNumberOfSides(8);

    lineMapper.push_back( vtkPolyDataMapper::New() );
    lineMapper.back()->SetInputConnection( lineTubes.back()->GetOutputPort() );

    lineActor.push_back( vtkActor::New() );
    lineActor.back()->SetMapper( lineMapper.back() );
  }
  lineActor[0]->GetProperty()->SetColor(0.9,0.,0.);
  lineActor[1]->GetProperty()->SetColor(0.,0.9,0.);
  lineActor[2]->GetProperty()->SetColor(0.,0.,0.9);
}


CompRigid3D::~CompRigid3D()
{
//   vPoints->Delete();
// //   boundary->Delete();
//   profile->Delete();
//   del3D->Delete();
//   aDataSetMapper->Delete();
//   anActor->Delete();
// 
//   std::vector< std::vector< vtkFloatArray* > >::iterator itScalarFields;
//   std::vector< vtkFloatArray* >::iterator itOneField;
//   for( itScalarFields = scalarFields.begin();
//        itScalarFields!= scalarFields.end();
//        ++itScalarFields
//      )
//   {
//     for( itOneField = itScalarFields->begin();
//          itOneField!= itScalarFields->end();
//          ++itOneField
//        )
//     {
//       (*itOneField)->Delete();
//       *itOneField = 0;
//     }
//   }
}


void CompRigid3D::addToRender(vtkRenderer * renderer_in)
{
  std::vector<vtkActor*>::iterator it_actor;
  for( it_actor = lineActor.begin();
       it_actor!= lineActor.end();
       ++it_actor)
    renderer_in->AddActor( *(it_actor) );
  
  if(anActor) renderer_in->AddActor( anActor );

}

void CompRigid3D::removeFromRender(vtkRenderer * renderer_in)
{
  std::vector<vtkActor*>::iterator it_actor;
  for( it_actor = lineActor.begin();
       it_actor!= lineActor.end();
       ++it_actor)
    renderer_in->RemoveActor( *(it_actor) );
   
    if(anActor) renderer_in->RemoveActor( anActor );
}

void CompRigid3D::updatePoints(int step)
{
  line[0]->SetPoint1( nodeA->getx(),
                   nodeA->gety(),
                   nodeA->getz()
                  );
  line[0]->SetPoint2( nodeB->getx(),
                   nodeB->gety(),
                   nodeB->getz()
                  );
  line[1]->SetPoint1( nodeA->getx(),
                   nodeA->gety(),
                   nodeA->getz()
                  );
  line[1]->SetPoint2( nodeC->getx(),
                   nodeC->gety(),
                   nodeC->getz()
                  );
  line[2]->SetPoint1( nodeA->getx(),
                   nodeA->gety(),
                   nodeA->getz()
                  );
  line[2]->SetPoint2( nodeD->getx(),
                   nodeD->gety(),
                   nodeD->getz()
                  );

  if( numPoints > 0){
//    cout << "Point 0, step " << step
//	 << ": " << points[step][0]->getx()
//	 << ", " << points[step][0]->gety()
//	 << ", " << points[step][0]->getz() << endl;

    for(int i=0; i<numPoints; ++i){
      vPoints->SetPoint( i,
			  points[step][i]->getx(),
			  points[step][i]->gety(),
			  points[step][i]->getz()
		      );
    }
    vPoints->Modified();
    boundary->Modified();
//     profile->Update();
//     del3D->Update();
  }


}

void CompRigid3D::readBoundary(std::ifstream& input, int trianglesNumber )
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
    profile = vtkPolyData::New();
}

void CompRigid3D::readDomain(std::ifstream& input, int timeSize, int nodesNumber, int dim, double* color )
{
  //////////////////////////////////////////////
  // Populating the storage vector of positions
  double res1, res2, res3;
  int i,j;
  if(timeSize==2) --timeSize; // if it is static

  for( i=points.size(); i<timeSize; ++i ){
    points.push_back( *(new std::vector< CompNode* >) ); // always zero
    
    for(j=0; j<nodesNumber; ++j){
      points[i].push_back( new CompNode ); 
      input >> res1 >> res2 >> res3;
      points[i][j]->setx( res1 ); // x
      points[i][j]->sety( res2 ); // y
      points[i][j]->setz( res3 ); // z
    }
    numPoints = nodesNumber;
  }
  vPoints = vtkPoints::New();
  vPoints->SetNumberOfPoints( nodesNumber );

  for(int i=0; i<nodesNumber; ++i){
    // initialize vPoints with initial position
    vPoints->InsertPoint( i,
                         points[0][i]->getx(),
                         points[0][i]->gety(),
                         points[0][i]->getz()
                       );
//     if( points[0][i]->getx() < 1. )
//       cout << "BAD NODE: " << points[0][i]->getx()
//            << ", " << points[0][i]->gety()
//            << ", " << points[0][i]->gety() << endl;
  }
  if(profile){
    profile->SetPoints( vPoints ); // if existed, there is a boundary defined
    profile->SetPolys( boundary );
    boundaryMapper = vtkPolyDataMapper::New();
#if VTK_MAJOR_VERSION > 5
    boundaryMapper->SetInputData(profile);
#else
    boundaryMapper->SetInput(profile);
#endif
    anActor = vtkActor::New();
    anActor->SetMapper(boundaryMapper);
  }
  else{
    profile = vtkPolyData::New();
    profile->SetPoints( vPoints );
    del3D = vtkDelaunay3D::New();
    del3D->SetAlpha(.25);
#if VTK_MAJOR_VERSION > 5
    del3D->SetInputData( profile );
#else
    del3D->SetInput( profile );
#endif
  //   del3D->SetSource( profile );
    del3D->Update();

    aDataSetMapper = vtkDataSetMapper::New();
#if VTK_MAJOR_VERSION > 5
    aDataSetMapper->SetInputData( del3D->GetOutput() );
#else
    aDataSetMapper->SetInput( del3D->GetOutput() );
#endif
    anActor = vtkActor::New();
    anActor->SetMapper(aDataSetMapper);
  }
  
//   anActor->GetProperty()->SetOpacity(0.75); // translucent !!!
//   anActor->AddPosition(2, 0, 0);InsertNextValue
  anActor->GetProperty()->SetDiffuseColor(color[0], color[1], color[2]);

}


void CompRigid3D::readResults( std::ifstream & input, int timeSize )
{
}

// Not compatible with STRESS reading yet...
void CompRigid3D::readTemps( std::ifstream & input, int timeSize )
{
}

void CompRigid3D::drawScalarField(int index, int step)
{
}

void CompRigid3D::updateScalarField(int step)
{
}


void CompRigid3D::setLookUpTable( vtkLookupTable* table_in )
{ 
}


void CompRigid3D::findMinMaxScalars()
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
