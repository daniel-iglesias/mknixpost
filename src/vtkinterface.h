/***************************************************************************
 *   Copyright (C) 2007 by Daniel Iglesias   *
 *   daniel@extremo   *
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
#ifndef VTKINTERFACE_H
#define VTKINTERFACE_H

#include <map>
#include <vector>

class QString;
class vtkRenderer;
class vtkScalarBarActor;
class vtkLookupTable;

class vtkSphereSource;
class vtkPolyDataMapper;
class vtkActor;

class vtkTextActor;
class vtkTextProperty;

class vtkLookupTable;
class vtkScalarBarActor;

class CompNode;
class CompMass;
class CompRigid2D;
class CompRigid3D;
class CompBar;
class CompMassPoint;
class CompMesh2D;
class CompMesh3D;
class CompMeshfree;
class CompMeshfree3D;
class CompRadMap;

/**
	@author Daniel Iglesias <daniel@extremo>
*/
class VTKInterface{
  public:
    VTKInterface();

    VTKInterface(vtkRenderer*);

    ~VTKInterface();

    void readFile( const QString& );
    void closeFile( );

    void toggleContourBar( bool );
    void viewContour( int );
    void viewEnvironment( int );
    void viewRec( bool );
    std::vector<QString>& getResultsNames()
    { return resultsNames; }
    std::vector<QString>& getComponentsNames()
    { return componentsNames; }
    std::vector<QString>& getEnvironmentsNames()
    { return environmentsNames; }
    void stepForward( int steps=1 );
    void stepBack( int steps=1 );
    void stepFirst( );
    void setStep( int );
    int getStep( );
    double getMinTime( )
    { if( timeConf.size() != 0 ) return timeConf.begin()->first;
      else return 0.;
    }
    double getMaxTime( )
    { if( timeConf.size() != 0 ) return (--timeConf.end() )->first;
      else return 0.;
    }
    int getNumberOfSteps()
    { return timeConf.size(); }

  private:
    void readSystem( );
    void readNodes( );
    void readRigidBodies( );
    void readFlexBodies( );
    void readAnalysis( );
    vtkRenderer* getRenderer();
    void updateStep( );
    void refreshTable( CompMesh2D* );
    void refreshTable( CompMesh3D* );
    void refreshTable( CompMeshfree* );
    void refreshTable( CompMeshfree3D* );
    void stepRecord( int );

  private:
    vtkRenderer* theRenderer;
    std::ifstream input; // file to read system from

    int dimension;
    int colorIndex;

    bool rec;
    std::map<int, CompNode> nodes;
    std::map<std::string, CompBar> bars;
    std::map<std::string, CompMassPoint> massPoints;
    std::map<std::string, CompRigid2D> rigid2Ds;
    std::map<std::string, CompRigid3D> rigid3Ds;
    std::map<std::string, CompMesh2D> mesh2D;
    std::map<std::string, CompMesh3D> mesh3D;
    std::map<std::string, CompMeshfree> meshfree;
    std::map<std::string, CompMeshfree3D> meshfree3D;
    std::map<std::string, CompRadMap> environments;
    std::map<double, std::vector< double > > timeConf;
    std::map<double, std::vector< double > >::iterator it_timeConf;

    std::vector< QString> resultsNames;
    std::vector< QString> componentsNames;
    std::vector< QString> environmentsNames;

    vtkSphereSource *sphere;
    vtkPolyDataMapper *sphereMapper;

    vtkTextActor* timeActor;
    vtkTextProperty* timeTextProperty;

    vtkLookupTable* table;
    vtkScalarBarActor* barActor;

    std::vector<vtkActor*> spheres;

    static double palette[10][3];
};

#endif
