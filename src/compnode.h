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
#ifndef COMPNODE_H
#define COMPNODE_H

class vtkRenderer;
class vtkSphereSource;
class vtkPolyDataMapper;
class vtkActor;

/**
	@author Daniel Iglesias <daniel@extremo>
*/
class CompNode{
  public:
    CompNode();

    CompNode(int, double, double, double);

    ~CompNode();

    int & getnum(){return num;}
    double& getx(){return x;}
    double& gety(){return y;}
    double& getz(){return z;}

    void setx(double& x_in){x = x_in;}
    void sety(double& y_in){y = y_in;}
    void setz(double& z_in){z = z_in;}

    void addToRender( vtkRenderer* );
    void removeFromRender( vtkRenderer* );
    void move( double&, double&, double& );

  private:
    int num;
    double x,y,z;
    vtkSphereSource *sphere;
    vtkPolyDataMapper *sphereMapper;
    vtkActor *sphereActor;
};

#endif
