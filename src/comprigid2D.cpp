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
#include "comprigid2D.h"

#include "compnode.h"

#include <vtkLineSource.h>
#include <vtkTubeFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkDelaunay2D.h>
#include <vtkTriangle.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkLookupTable.h>

CompRigid2D::CompRigid2D()
{
}

CompRigid2D::CompRigid2D( std::string name_in
			, int mat_in
			, CompNode* nodeA_in
			, CompNode* nodeB_in
			, CompNode* nodeC_in
                          )
  : name(name_in)
    , mat(mat_in)
    , nodeA(nodeA_in)
    , nodeB(nodeB_in)
    , nodeC(nodeC_in)
    , numPoints(0)
    , currentIndex(0)
    , anActor(0)
{
  for(int i=0; i<2; ++i){
    line.push_back(vtkLineSource::New());
    line.back()->SetResolution(10);

    lineTubes.push_back( vtkTubeFilter::New() );
    lineTubes.back()->SetInputConnection(line.back()->GetOutputPort());
    lineTubes.back()->SetRadius(0.15);
    lineTubes.back()->SetNumberOfSides(8);

    lineMapper.push_back( vtkPolyDataMapper::New() );
    lineMapper.back()->SetInputConnection( lineTubes.back()->GetOutputPort() );

    lineActor.push_back( vtkActor::New() );
//     lineActor.back()->SetMapper( lineMapper.back() );
// l    lineActor.back()->GetProperty()->SetColor(0.4235,0.6667,0.000);
  }
  lineActor[0]->GetProperty()->SetColor(0.9,0.,0.);
  lineActor[1]->GetProperty()->SetColor(0.,0.9,0.);

}


CompRigid2D::~CompRigid2D()
{
//   vPoints->Delete();
// //   boundary->Delete();
//   profile->Delete();
//   del2D->Delete();
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


void CompRigid2D::addToRender(vtkRenderer * renderer_in)
{
  std::vector<vtkActor*>::iterator it_actor;
  for( it_actor = lineActor.begin();
       it_actor!= lineActor.end();
       ++it_actor)
    renderer_in->AddActor( *(it_actor) );

  if(anActor) renderer_in->AddActor( anActor );

}

void CompRigid2D::removeFromRender(vtkRenderer * renderer_in)
{
  std::vector<vtkActor*>::iterator it_actor;
  for( it_actor = lineActor.begin();
       it_actor!= lineActor.end();
       ++it_actor)
    renderer_in->RemoveActor( *(it_actor) );

    if(anActor) renderer_in->RemoveActor( anActor );
}

void CompRigid2D::updatePoints(int step)
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

  if( numPoints > 0){
    for(int i=0; i<numPoints; ++i){
      vPoints->SetPoint( i,
			  points[step][i]->getx(),
			  points[step][i]->gety(),
			  points[step][i]->getz()
		      );
    }
    vPoints->Modified();
    boundary->Modified();
  }


}

void CompRigid2D::readBoundary(ifstream& input, int trianglesNumber )
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



void CompRigid2D::readDomain(ifstream& input, int timeSize, int nodesNumber, int dim, double* color )
{
  //////////////////////////////////////////////
  // Populating the storage vector of positions
  double res1, res2, zero(0);
  int i,j;
  if(timeSize==2) --timeSize; // if it is static

  for( int i=points.size(); i<timeSize; ++i ){
    points.push_back( *(new std::vector< CompNode* >) ); // Container for step

    for(int j=0; j<nodesNumber; ++j){
      points[i].push_back( new CompNode );
      input >> res1 >> res2;
      points[i][j]->setx( res1 ); // x
      points[i][j]->sety( res2 ); // y
      points[i][j]->setz( zero ); // z
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
  }
  if(profile){
    profile->SetPoints( vPoints ); // if existed, there is a boundary defined
    profile->SetPolys( boundary );
    boundaryMapper = vtkPolyDataMapper::New();
#if VTK_MAJOR_VERSION > 5
    boundaryMapper->SetInputData( profile );
#else
    boundaryMapper->SetInput( profile );
#endif
    anActor = vtkActor::New();
    anActor->SetMapper(boundaryMapper);
  }
  else{
    profile = vtkPolyData::New();
    profile->SetPoints( vPoints );
  //   profile->SetPolys( boundary );
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
  aDataSetMapper->SetInputData( del2D->GetOutput() );
// IF this doesn't work, try:
//  aDataSetMapper->SetInputConnection( del2D->GetOutputPort() );
#else
  aDataSetMapper->SetInput( del2D->GetOutput() );
#endif

    anActor = vtkActor::New();
    anActor->SetMapper(aDataSetMapper);
  //   anActor->AddPosition(2, 0, 0);InsertNextValue
  }
//   anActor->GetProperty()->SetDiffuseColor(1.0, 0.3, 0.3);
  anActor->GetProperty()->SetDiffuseColor(color[0], color[1], color[2]);
}


void CompRigid2D::readResults( std::ifstream & input, int timeSize )
{
}

// Not compatible with STRESS reading yet...
void CompRigid2D::readTemps( std::ifstream & input, int timeSize )
{
}

void CompRigid2D::drawScalarField(int index, int step)
{
}

void CompRigid2D::updateScalarField(int step)
{
}


void CompRigid2D::setLookUpTable( vtkLookupTable* table_in )
{
}


void CompRigid2D::findMinMaxScalars()
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
