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
#ifndef COMPMESHFREE_H
#define COMPMESHFREE_H

#include <vector>
#include <map>
#include <fstream>

class vtkPoints;
class vtkCellArray;
class vtkPolyData;
class vtkFloatArray;
class vtkDelaunay2D;
class vtkLine;
class vtkDataSetMapper;
class vtkActor;
class vtkRenderer;
class vtkLookupTable;

class CompNode;

/**
	@author Daniel Iglesias <diglesias@mecanica.upm.es>
*/
class CompMeshfree{
public:
    CompMeshfree();

    ~CompMeshfree();

    void initialize( std::map<int, CompNode>&, int, int, std::vector<int> );
  
    void readBoundary(std::ifstream& , int  );

    void addToRender( vtkRenderer* );

    void removeFromRender( vtkRenderer* );

    void updatePoints();

    void readEnergy( std::ifstream&, int );
    
    void readResults( std::ifstream&, int );

    void readTemps( std::ifstream&, int );

    void drawScalarField( int, int );

    void updateScalarField( int );

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
    vtkPolyData* profile;
    std::vector< vtkLine* > lines;
    std::vector< std::vector< vtkFloatArray* > > scalarFields;
    std::vector< std::vector< double > > energyFields;
    vtkDelaunay2D* del2D;

    vtkDataSetMapper* aDataSetMapper;
    vtkActor* anActor;

};

#endif
