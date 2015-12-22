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
#ifndef COMPRADMAP_H
#define COMPRADMAP_H

#include <vector>
#include <map>
#include <fstream>

class vtkPoints;
class vtkCellArray;
class vtkFloatArray;
class vtkStructuredGrid;
class vtkContourFilter;
class vtkPolyDataMapper;
class vtkDataSetMapper;
class vtkPiecewiseFunction;
class vtkDepthSortPolyData;
class vtkCamera;
//class vtkVolumeProperty;
//class vtkSmartVolumeMapper;
//class vtkVolume;
class vtkActor;
class vtkRenderer;
class vtkLookupTable;

class CompNode;

/**
	@author Daniel Iglesias <diglesias@mecanica.upm.es>
*/
class CompRadMap{
public:
    CompRadMap();

    ~CompRadMap();

    void addToRender( vtkRenderer* );

    void removeFromRender( vtkRenderer* );

    void readResults( std::ifstream& );

    void setLookUpTable( vtkLookupTable* table_in );

    double getMinScalar(){ return minScalar; }

    double getMaxScalar(){ return maxScalar; }

  private:
    void findMinMaxScalars();

  private:
    std::map<int,CompNode*> nodes;
    int currentIndex;
    double minScalar;
    double maxScalar;

    vtkPoints* vPoints;
    vtkCellArray* boundary;
    vtkFloatArray* scalarField;
    vtkStructuredGrid* structuredGrid;
    vtkContourFilter* contour;
    vtkPiecewiseFunction* opacityTransferFunction;
    vtkDepthSortPolyData* depthSort;
    vtkCamera* camera;
//    vtkVolumeProperty* volumeProperty;
//    vtkSmartVolumeMapper* volumeMapper;
//    vtkVolume* volume;

    vtkPolyDataMapper* contourMapper;
    vtkDataSetMapper* mapper;
    vtkActor* contourActor;
    vtkActor* anActor;

};

#endif
