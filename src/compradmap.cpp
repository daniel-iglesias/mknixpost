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
#include "compradmap.h"

#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkStructuredGrid.h>
#include <vtkContourFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkDataSetMapper.h>
#include <vtkPiecewiseFunction.h>
#include <vtkDepthSortPolyData.h>
#include <vtkCamera.h>
//#include <vtkVolumeProperty.h>
//#include <vtkSmartVolumeMapper.h>
//#include <vtkVolume.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkLookupTable.h>
#include <vtkSmartPointer.h>

CompRadMap::CompRadMap()
: vPoints(0)
, scalarField(0)
, structuredGrid(0)
//, volume(0)
, camera(0)
, contourMapper(0)
, mapper(0)
, contourActor(0)
, anActor(0)
{
}


CompRadMap::~CompRadMap()
{
  if (vPoints!=0) vPoints->Delete();
//  boundary->Delete();
  if (structuredGrid!=0) structuredGrid->Delete();
  if (mapper!=0) mapper->Delete();
  if (anActor!=0) anActor->Delete();

  if (scalarField!=0){
    scalarField->Delete();
    scalarField = 0;
  }
}


void CompRadMap::addToRender(vtkRenderer * renderer_in)
{
  if(anActor) renderer_in->AddActor( anActor );
  if(contourActor) renderer_in->AddActor( contourActor );
//  if(volume) renderer_in->AddViewProp(volume);
  if(camera) renderer_in->SetActiveCamera(camera);
}

void CompRadMap::removeFromRender(vtkRenderer * renderer_in)
{
  if(anActor) renderer_in->RemoveActor( anActor );
  if(contourActor) renderer_in->RemoveActor( contourActor );
//  if(volume) renderer_in->RemoveViewProp(volume);
  if(camera) renderer_in->ResetCamera();
}


void CompRadMap::readResults( std::ifstream & input )
{
  int num_p, num_x, num_y, num_z, p_counter;
  int maxContour(2*10);
  double x, y, z, value;
  // Read size of grid
  input >> num_x >> num_y >> num_z;
  cout << "Grid size\t" << num_x << "\t" << num_y << "\t" << num_z << endl;
  num_p = num_x*num_y*num_z; // Regular grid
  cout << "Number of points in grid = " << num_p << endl; 
  
  vPoints = vtkPoints::New();
//  vPoints->SetNumberOfPoints( num_p );
  scalarField = vtkFloatArray::New();
     
  for( p_counter=0; p_counter < num_p; ++p_counter )
  {
    input >> x >> y >> z >> value;
    vPoints->InsertNextPoint( x, y, z);
    scalarField->InsertNextValue( value ); // rad value
  }
  
  structuredGrid = vtkStructuredGrid::New();
  structuredGrid->SetDimensions(num_x, num_y, num_z);
  structuredGrid->SetPoints(vPoints);
  structuredGrid->GetPointData()->SetScalars( scalarField );
  
  findMinMaxScalars();
  cout << "RANGE: " << minScalar << ", " << maxScalar << endl;
  
  // Create transfer mapping scalar value to opacity.
  double low_values = minScalar + (maxScalar-minScalar)*0.1;
  double mid_values = minScalar + (maxScalar-minScalar)*0.6;
  vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction =
  vtkSmartPointer<vtkPiecewiseFunction>::New();
  opacityTransferFunction->AddPoint(low_values,  0.0);
  opacityTransferFunction->AddPoint(mid_values, 0.2);
  opacityTransferFunction->AddPoint(maxScalar, 0.7);
    
  // The property describes how the data will look.
//  vtkSmartPointer<vtkVolumeProperty> volumeProperty =
//  vtkSmartPointer<vtkVolumeProperty>::New();
////  volumeProperty->SetColor(colorTransferFunction);
//  volumeProperty->SetScalarOpacity(opacityTransferFunction);
//  volumeProperty->ShadeOff();
//  volumeProperty->SetInterpolationTypeToLinear();
  
  contour = vtkContourFilter::New();
#if VTK_MAJOR_VERSION > 5
  contour->SetInputData( structuredGrid );
#else
  contour->SetInput( structuredGrid );
#endif
  contour->SetNumberOfContours(maxContour);
  for (int i=0; i<maxContour/2; ++i) {
    contour->SetValue(i, minScalar + i*(maxScalar-minScalar)*0.6/maxContour);
    contour->SetValue(2*i, maxScalar*.6 + i*(maxScalar-maxScalar*0.6)/maxContour);
  }
  contour->SetValue(maxContour-1, maxScalar);
//  contour->GenerateValues(maxContour/2, maxScalar*.6, maxScalar);
//  contour->GenerateValues(maxContour/2, minScalar, maxScalar*.6);
//  contour->SetValue(0, minScalar);
  contour->Update();
  
//  vtkSmartPointer<vtkCamera> camera =
//  vtkSmartPointer<vtkCamera>::New();
//
//  vtkSmartPointer<vtkDepthSortPolyData> depthSort =
//  vtkSmartPointer<vtkDepthSortPolyData>::New();  
//  depthSort->SetInputConnection(contour->GetOutputPort());
//  depthSort->SetDirectionToBackToFront();
//  depthSort->SetVector(0, 0, 1);
//  depthSort->SetCamera(camera);
//  depthSort->SortScalarsOn();
//  depthSort->Update();
  
  contourMapper = vtkPolyDataMapper::New();
//  contourMapper->SetInputConnection( depthSort->GetOutputPort() );
  contourMapper->SetInputConnection( contour->GetOutputPort() );
  contourMapper->SetScalarRange(minScalar, maxScalar);
  
  mapper = vtkDataSetMapper::New();
#if VTK_MAJOR_VERSION > 5
  mapper->SetInputData( structuredGrid );
#else
  mapper->SetInput( structuredGrid );
#endif
  mapper->SetScalarRange(minScalar, maxScalar);
  
//  vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper =
//  vtkSmartPointer<vtkSmartVolumeMapper>::New();
//  volumeMapper->SetInput(structuredGrid);
//  
//  // The volume holds the mapper and the property and can be used to
//  // position/orient the volume.
//  vtkSmartPointer<vtkVolume> volume =
//  vtkSmartPointer<vtkVolume>::New();
//  volume->SetMapper(volumeMapper);
//  volume->SetProperty(volumeProperty);
  
  contourActor = vtkActor::New();
  contourActor->SetMapper(contourMapper);
  contourActor->GetProperty()->SetOpacity(0.4); // translucent !!!
  
  anActor = vtkActor::New();
  anActor->SetMapper(mapper);
  anActor->GetProperty()->SetOpacity(0.25); // translucent !!!
  //   anActor->AddPosition(2, 0, 0);
  anActor->GetProperty()->SetDiffuseColor(1.0, 0.3, 0.3);
}

void CompRadMap::setLookUpTable( vtkLookupTable* table_in )
{ 
  if(mapper) mapper->SetLookupTable( table_in); 
  if(contourMapper) contourMapper->SetLookupTable( table_in); 
}

void CompRadMap::findMinMaxScalars()
{
  double min_max[2];
  if( scalarField!=0 ){ // set first values
    scalarField->GetRange(min_max);
    minScalar = min_max[0];
    maxScalar = min_max[1];
  }
}
