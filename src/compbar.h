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
#ifndef COMPBAR_H
#define COMPBAR_H

#include <string>

class CompNode;
class vtkRenderer;
class vtkLineSource;
class vtkTubeFilter;
class vtkPolyDataMapper;
class vtkActor;

/**
	@author Daniel Iglesias <daniel@extremo>
*/
class CompBar{
public:
    CompBar();

    CompBar(std::string, int, CompNode*, CompNode*);

    ~CompBar();

    void addToRender( vtkRenderer* );

    void removeFromRender( vtkRenderer* );

    void updatePoints();

  private:
    std::string name;
    int mat;
    CompNode* nodeA;
    CompNode* nodeB;
    vtkLineSource *line;
    vtkTubeFilter *lineTubes;
    vtkPolyDataMapper *lineMapper;
    vtkActor *lineActor;
};

#endif
