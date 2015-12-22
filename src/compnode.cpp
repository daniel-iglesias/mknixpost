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
#include <vtkSphereSource.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

#include "compnode.h"

CompNode::CompNode()
{
}


CompNode::CompNode(int num_in, double x_in, double y_in, double z_in)
  : num(num_in)
    , x(x_in)
    , y(y_in)
    , z(z_in)
{
//   sphere = vtkSphereSource::New();
//   sphere->SetThetaResolution(30);
//   sphere->SetPhiResolution(30);
//   sphere->SetRadius(0.1);
// 
//   sphereMapper = vtkPolyDataMapper::New();
//   sphereMapper->SetInputConnection( sphere->GetOutputPort() );
// 
//   sphereActor = vtkActor::New();
//   sphereActor->SetMapper( sphereMapper );
//   sphereActor->GetProperty()->SetColor(0.4235,0.6667,0.000);
// 
// //   sphere->SetCenter(x, y, z);
//   sphereActor->AddPosition(x, y, z);

}


CompNode::~CompNode()
{
}

void CompNode::addToRender(vtkRenderer * renderer_in)
{
//       renderer_in->AddActor( sphereActor );
}

void CompNode::removeFromRender(vtkRenderer * renderer_in)
{
//       renderer_in->RemoveActor( sphereActor );
}

void CompNode::move(double & x_in, double & y_in, double & z_in)
{
  x = x_in;
  y = y_in;
  z = z_in;
}



