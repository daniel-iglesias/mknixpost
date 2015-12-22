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
#ifndef COMPMASSPOINT_H
#define COMPMASSPOINT_H

class CompNode;
class vtkRenderer;
class vtkSphereSource;
class vtkPolyDataMapper;
class vtkActor;

/**
	@author Daniel Iglesias <daniel@extremo>
*/
class CompMassPoint{
  public:
    CompMassPoint();

    CompMassPoint(std::string, int, CompNode*);

    ~CompMassPoint();

    void addToRender( vtkRenderer* );

    void removeFromRender( vtkRenderer* );

    void updatePoints();

  private:
    std::string name;
    int mat;
    CompNode* nodeA;
    vtkSphereSource *sphere;
    vtkPolyDataMapper *sphereMapper;
    vtkActor *sphereActor;
};

#endif
