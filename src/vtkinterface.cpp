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
#include <vtkRenderer.h>
#include <vtkTextProperty.h>
#include <vtkCaptionActor2D.h>
#include <vtkMapper.h>
#include <vtkRenderWindow.h>

#include <vtkSphereSource.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkLookupTable.h>
#include <vtkScalarBarActor.h>

#include <vtkWindowToImageFilter.h>
#include <vtkJPEGWriter.h>

#include <QString>
#include <sstream>

#include "vtkinterface.h"
#include "compnode.h"
#include "compbar.h"
#include "compmasspoint.h"
#include "comprigid2D.h"
#include "comprigid3D.h"
#include "compmeshfree.h"
#include "compmeshfree3D.h"
#include "compradmap.h"


double VTKInterface::palette[10][3] = 
{ {1.0,0.3,0.3},
  {0.3,1.0,0.3},
  {0.3,0.3,1.0},
  {0.5,0.3,0.3},
  {0.3,0.5,0.3},
  {0.3,0.3,0.5},
  {0.2,0.3,0.3},
  {0.3,0.2,0.3},
  {0.3,0.3,0.2},
  {0.5,0.5,1.0}
};

VTKInterface::VTKInterface()
{
}

VTKInterface::VTKInterface(vtkRenderer* theRenderer_in)
  : theRenderer(theRenderer_in)
{
  sphere = vtkSphereSource::New();
  sphere->SetThetaResolution(20);
  sphere->SetPhiResolution(15);
//   sphere->SetRadius(0.00002); // slits
//   sphere->SetRadius(0.5); // pendulum
  sphere->SetRadius(0.5E-3); // pendulum

  sphereMapper = vtkPolyDataMapper::New();
  sphereMapper->SetInputConnection(sphere->GetOutputPort());

  table = vtkLookupTable::New();
  table->SetTableRange( 0.0, 0.0 );
  table->SetNumberOfTableValues(100);
  // Stress preferred:
  table->SetHueRange( 0.667, 0.0 );
  // Thermal preferred:
//   table->SetHueRange( 0.0, 0.1 );
//   table->SetValueRange( 0.2, 0.95 );
  table->Build();

  barActor = vtkScalarBarActor::New();
  barActor->SetLookupTable(table);
//   barActor->SetTitle("No results");
  barActor->SetOrientationToVertical();
  barActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  barActor->GetPositionCoordinate()->SetValue(0.90,0.1);
  barActor->SetWidth(0.07);
  barActor->SetHeight(0.9);
  barActor->SetNumberOfLabels(9);
  barActor->GetLabelTextProperty()->SetFontSize(16);
  barActor->GetLabelTextProperty()->ItalicOff();

  rec = false;
}


VTKInterface::~VTKInterface()
{
}

void VTKInterface::readFile(const QString & file_name )
{
  input.open(file_name.toStdString().data());
  char keyword[100];
  char a; // For reading one-by-one the file.

  while(input >> keyword) {

    if(!strcmp(keyword,"//")) {
      do {input.get(a);} while(a!='\n');
    }

//     else if(!strcmp(keyword,"TITLE")) {
//       input >> theSimulation->title;
//       output << "TITLE: " << theSimulation->title << std::endl;
//     }

    else if(!strcmp(keyword,"DIMENSION")) {
      input >> dimension;
    }

    else if(!strcmp(keyword,"SYSTEM")) {
      this->readSystem( );
    }

    else if(!strcmp(keyword,"ANALYSIS")) {
      this->readAnalysis( );
    }
  }

//   }
//  theRenderer->SetBackground( 1, 1, 1); // white
  theRenderer->ResetCamera();
  theRenderer->ResetCameraClippingRange();
//   Render the scene and start interaction.
  theRenderer->GetRenderWindow()->Render();

  this->setStep( 0 );
}


void VTKInterface::closeFile()
{
  std::map<int, CompNode>::iterator it_nodes;
  std::vector<vtkActor*>::iterator it_spheres;
  std::map<std::string, CompBar>::iterator it_bars;
  std::map<std::string, CompRigid2D>::iterator it_rigid2D;
  std::map<std::string, CompMassPoint>::iterator it_massPoints;
  std::map<std::string, CompMeshfree>::iterator it_meshfree;
  std::map<std::string, CompMeshfree3D>::iterator it_meshfree3D;
  std::map<std::string, CompRadMap>::iterator it_radMap;

  for( it_nodes = nodes.begin();
      it_nodes!= nodes.end();
      ++it_nodes )
  {
      it_nodes->second.removeFromRender(theRenderer);
  }
  for( it_spheres = spheres.begin();
      it_spheres!= spheres.end();
      ++it_spheres )
  {
      theRenderer->RemoveActor((*it_spheres));
  }
//   for( it_bars = bars.begin();
//       it_bars!= bars.end();
//       ++it_bars )
//   {
//       it_bars->second.removeFromRender(theRenderer);
//   }
  for( it_rigid2D = rigid2Ds.begin();
      it_rigid2D!= rigid2Ds.end();
      ++it_rigid2D )
  {
      it_rigid2D->second.removeFromRender(theRenderer);
  }
  for( it_massPoints =  massPoints.begin();
      it_massPoints!= massPoints.end();
      ++it_massPoints )
  {
      it_massPoints->second.removeFromRender(theRenderer);
  }
  for( it_meshfree = meshfree.begin();
       it_meshfree!= meshfree.end();
       ++it_meshfree )
  {
    it_meshfree->second.removeFromRender(theRenderer);
  }
  for( it_meshfree3D = meshfree3D.begin();
      it_meshfree3D!= meshfree3D.end();
      ++it_meshfree3D )
    {
      it_meshfree3D->second.removeFromRender(theRenderer);
    }
  for( it_radMap = environments.begin();
      it_radMap!= environments.end();
      ++it_radMap )
  {
    it_radMap->second.removeFromRender(theRenderer);
  }
  nodes.clear();
  spheres.clear();
  bars.clear();
  massPoints.clear();
  timeConf.clear();
  theRenderer->GetRenderWindow()->Render();
  input.close();
  spheres.clear();
  resultsNames.clear();
  environments.clear();
}


void VTKInterface::readSystem()
{
  char keyword[20];
  while(input >> keyword) {
    if(!strcmp(keyword,"ENDSYSTEM")) return;

    else if(!strcmp(keyword,"NODES")) {
      this->readNodes( );
    }
    else if(!strcmp(keyword,"RIGIDBODIES")) {
      this->readRigidBodies( );
    }
    else if(!strcmp(keyword,"FLEXBODIES")) {
      this->readFlexBodies( );
    }
//     else if(!strcmp(keyword,"CONSTRAINTS")) {
//       this->readConstraints( system_in->subSystems[sysTitle] );
//     }
  }
}


void VTKInterface::readNodes()
{
  char keyword[20];
  int num;
  double x,y,z;
  int counter=0;

  while(input >> keyword) {
    if(!strcmp(keyword,"ENDNODES")) return;

    else{
      num = atoi(keyword);
      input >> x >> y >> z;
      nodes[num] = *( new CompNode(num, x, y, z) );
//       nodes[num].addToRender( this->theRenderer );
      spheres.push_back( vtkActor::New() );
      spheres[counter]->SetMapper(sphereMapper);
      spheres[counter]->GetProperty()->SetColor(0.4235,0.6667,0.000);
      spheres[counter]->GetProperty()->SetAmbient(0.3);
      spheres[counter]->GetProperty()->SetDiffuse( (double)0.5 );
      spheres[counter]->GetProperty()->SetSpecular(0.0);
      spheres[counter]->GetProperty()->SetSpecularPower(5.0);
      spheres[counter]->AddPosition(x, y, z);
      theRenderer->AddActor(spheres[counter]);
      ++counter;
    }
  }
}


void VTKInterface::readRigidBodies()
{
  char keyword[20];
  std::string name;
  int mat, nodeA, nodeB, nodeC, nodeD;

  while(input >> keyword) {
    if(!strcmp(keyword,"ENDRIGIDBODIES")) return;

    else if(!strcmp(keyword,"BAR")){
      input >> name >> mat >> nodeA >> nodeB;
      bars[name] = CompBar(name, mat, &nodes[nodeA], &nodes[nodeB]);
      bars[name].updatePoints( );
      bars[name].addToRender( this->theRenderer );
      componentsNames.push_back("BAR.");
      componentsNames.back().append(name.c_str());
    }
    else if(!strcmp(keyword,"MASSPOINT")){
      input >> name >> mat >> nodeA >> nodeB; //nodeB is nothing...
      massPoints[name] = CompMassPoint(name, mat, &nodes[nodeA]);
      massPoints[name].updatePoints( );
      massPoints[name].addToRender( this->theRenderer );
      componentsNames.push_back("MASSPOINT.");
      componentsNames.back().append(name.c_str());
    }
    else if(!strcmp(keyword,"GENERIC2D")){
      input >> name >> mat >> nodeA >> nodeB >> nodeC;
      rigid2Ds[name] = CompRigid2D(name, mat, &nodes[nodeA], &nodes[nodeB], &nodes[nodeC]);
      rigid2Ds[name].updatePoints( 0 );
      rigid2Ds[name].addToRender( this->theRenderer );
      componentsNames.push_back("GENERIC2D.");
      componentsNames.back().append(name.c_str());
    }
    else if(!strcmp(keyword,"GENERIC3D")){
      input >> name >> mat >> nodeA >> nodeB >> nodeC >> nodeD;
      rigid3Ds[name] = CompRigid3D(name, mat, &nodes[nodeA], &nodes[nodeB], &nodes[nodeC], &nodes[nodeD]);
      rigid3Ds[name].updatePoints( 0 );
      rigid3Ds[name].addToRender( this->theRenderer );
      componentsNames.push_back("GENERIC3D.");
      componentsNames.back().append(name.c_str());
    }
  }
}


void VTKInterface::readFlexBodies()
{
  char keyword[20];
  std::string name;
  int firstNode, lastNode;
  std::vector<int> boundaryNodes;

  while(input >> keyword) {
    if(!strcmp(keyword,"ENDFLEXBODIES")) return;

    else if(!strcmp(keyword,"MESHFREE") || !strcmp(keyword, "MESH")){
      input >> name;

      while(input >> keyword) { //read the node numbers
        if(!strcmp(keyword,"ENDMESHFREE") || !strcmp(keyword, "ENDMESH")) break;

        else if(!strcmp(keyword,"NODES")) {
          input >> firstNode >> lastNode;
        }
        else if(!strcmp(keyword,"BOUNDARY")) {
          int boundarySize, nodeNumber;
          boundaryNodes.clear();
          input >> boundarySize;
          // Initialize boundary loop
          for(int i=0; i< boundarySize; ++i){
            input >> nodeNumber;
            boundaryNodes.push_back(nodeNumber);
          }
        }
      }
      if(dimension==2){
        meshfree[name] = CompMeshfree();
        meshfree[name].initialize( nodes, firstNode, lastNode, boundaryNodes );
        meshfree[name].updatePoints();
        meshfree[name].addToRender( this->theRenderer );
        componentsNames.push_back("MESHFREE2D.");
        componentsNames.back().append(name.c_str());

      }
      else{
        meshfree3D[name] = CompMeshfree3D();
        meshfree3D[name].initialize( nodes, firstNode, lastNode, boundaryNodes );
        meshfree3D[name].addToRender( this->theRenderer );
        componentsNames.push_back("MESHFREE3D.");
        componentsNames.back().append(name.c_str());
      }
//       meshfree[name].updatePoints( );

    }
  }
}


void VTKInterface::readAnalysis()
{
  std::string keyword;
  int colorIndex(0);
  std::string stringKeyword;
  double time, qi;

  while(input >> keyword) {
    if(!strcmp(keyword.c_str(),"ENDANALYSIS")) return;
    else if(!strcmp(keyword.c_str(),"FILE")){
      input >> keyword; // read filename
      std::ifstream input_analysis( keyword.c_str() ); // file to read analysis from
      while(input_analysis >> keyword) {
        time = atof(keyword.c_str());
        int qSize = dimension*nodes.size();
        for (int i = 0; i < qSize; ++i){
          input_analysis >> qi;
          timeConf[time].push_back(qi);
        }
        it_timeConf = timeConf.begin();
      }
    }
    else if(!strcmp(keyword.c_str(),"STRESS")){
      input >> stringKeyword;
      if( meshfree.find(stringKeyword) != meshfree.end() ) {
        meshfree[stringKeyword].readResults( input, timeConf.size() );
        resultsNames.push_back("sigma_x");
        resultsNames.push_back("sigma_y");
        resultsNames.push_back("sigma_xy");
        resultsNames.push_back("sigma_VM");
      }
      else if( meshfree3D.find(stringKeyword) != meshfree3D.end() ) {
        meshfree3D[stringKeyword].readResults( input, timeConf.size() );
        resultsNames.push_back("sigma_x");
        resultsNames.push_back("sigma_y");
        resultsNames.push_back("sigma_z");
        resultsNames.push_back("sigma_xy");
        resultsNames.push_back("sigma_yz");
        resultsNames.push_back("sigma_xz");
        resultsNames.push_back("sigma_VM");
      }
      else
        cerr << "ERROR: Body " << stringKeyword << " not found for STRESS reading" << endl;
    }
    else if(!strcmp(keyword.c_str(),"TEMPERATURE")){
      input >> stringKeyword;
      meshfree[stringKeyword].readTemps( input, timeConf.size() );
      resultsNames.push_back("temperatures");
    }
    else if(!strcmp(keyword.c_str(),"ENERGY")){
      input >> stringKeyword; // name of components
      if( meshfree.find(stringKeyword) != meshfree.end() ) {
	meshfree[stringKeyword].readEnergy( input, timeConf.size() ); // does nothing, read stuff is in graphwidget
      }      
    }
    else if(!strcmp(keyword.c_str(),"DOMAIN")){
      input >> stringKeyword; // name of rigid body
      int nodesNumber;
      if( rigid2Ds.find(stringKeyword) != rigid2Ds.end() ) {
        rigid2Ds[stringKeyword].removeFromRender( this->theRenderer );

        input >> nodesNumber;
        rigid2Ds[stringKeyword].readDomain( input, timeConf.size(), nodesNumber, dimension );
        rigid2Ds[stringKeyword].addToRender( this->theRenderer );
      }
      else if( rigid3Ds.find(stringKeyword) != rigid3Ds.end() ) {
        rigid3Ds[stringKeyword].removeFromRender( this->theRenderer );
        input >> nodesNumber;
        rigid3Ds[stringKeyword].readDomain
        ( input, timeConf.size(), nodesNumber, dimension, palette[colorIndex] );
        rigid3Ds[stringKeyword].addToRender( this->theRenderer );
        colorIndex == 9 ? colorIndex=0 : colorIndex++;
      }
    }
    else if(!strcmp(keyword.c_str(),"BOUNDARY")){
      input >> stringKeyword; // name of rigid body
      int segmentsNumber;
//       if( rigid2Ds.find(stringKeyword) != rigid2Ds.end() ) {
//         rigid2Ds[stringKeyword].removeFromRender( this->theRenderer );
// 
//         input >> nodesNumber;
//         rigid2Ds[stringKeyword].readDomain( input, timeConf.size(), nodesNumber, dimension );
//         rigid2Ds[stringKeyword].addToRender( this->theRenderer );
//       }
      if( rigid3Ds.find(stringKeyword) != rigid3Ds.end() ) {
        input >> segmentsNumber; // segments-facets number
        rigid3Ds[stringKeyword].readBoundary( input, segmentsNumber );
      }
    }
    else if(!strcmp(keyword.c_str(),"RADIATION")){
      environments[keyword] = CompRadMap();
      environments[keyword].readResults( input );
      environments[keyword].setLookUpTable(table);
//      environments[keyword].addToRender( theRenderer );
      environmentsNames.push_back(keyword.c_str());
    }
    else if(!strcmp(keyword.c_str(), "CONFIGURATION")){ // Read the positions of formulation nodes along simulation time
		while (input >> keyword) {
			if (!strcmp(keyword.c_str(), "ENDCONFIGURATION")) break;
			std::istringstream i(keyword);
			i >> time;
			//      time = atof(keyword);
			//      cout << keyword << ", ";
			int qSize = dimension * nodes.size();
			for (int i = 0; i < qSize; ++i) {
				input >> qi;
				timeConf[time].push_back(qi);
			}
			it_timeConf = timeConf.begin();
		}
	}
  }
//  std::map<double, std::vector< double > >::iterator it_timeConf_temp;
//  std::vector< double >::iterator it_vector;
//  for(it_timeConf_temp = timeConf.begin();
//      it_timeConf_temp != timeConf.end();
//      ++it_timeConf_temp)
//  {
//    cout << endl << "Time = "<< it_timeConf_temp->first << endl;
//    for(it_vector = it_timeConf_temp->second.begin();
//        it_vector!= it_timeConf_temp->second.end();
//        ++it_vector){
//      cout << (*it_vector) << ", ";
//    }
//  }
}


vtkRenderer* VTKInterface::getRenderer()
{
  return this->theRenderer;
}


void VTKInterface::toggleContourBar(bool state)
{
  if ( !state ){
    theRenderer->RemoveActor2D( barActor );
//     cout << "Deactivating scale bar..." << endl;
  }
  if ( state ){
    theRenderer->AddActor2D( barActor );
//     cout << "Activating scale bar..." << endl;
  }
  theRenderer->GetRenderWindow()->Render();
}

void VTKInterface::viewContour( int index )
{
  std::map<std::string, CompMeshfree>::iterator itMeshfree;
  std::map<std::string, CompMeshfree3D>::iterator itMeshfree3D;

  table->SetTableRange( 0., 0.);

  for( itMeshfree = meshfree.begin();
       itMeshfree!= meshfree.end();
       ++itMeshfree
     )
  {
    itMeshfree->second.drawScalarField( index, getStep() );
    itMeshfree->second.setLookUpTable( table );
    refreshTable( &itMeshfree->second );
  }
  
  for( itMeshfree3D = meshfree3D.begin();
       itMeshfree3D!= meshfree3D.end();
       ++itMeshfree3D
     )
  {
    itMeshfree3D->second.drawScalarField( index, getStep() );
    itMeshfree3D->second.setLookUpTable( table );
    refreshTable( &itMeshfree3D->second );
  }
  theRenderer->GetRenderWindow()->Render();

}

void VTKInterface::viewEnvironment( int index )
{
  int i=1;
  std::map<std::string, CompRadMap>::iterator itEnvironment;
  
  for( itEnvironment = environments.begin();
      itEnvironment!= environments.end(); 
      ++itEnvironment )
  {
    if( i==index )
      itEnvironment->second.addToRender(theRenderer);
    else
      itEnvironment->second.removeFromRender(theRenderer);
    ++i;
  }
  theRenderer->GetRenderWindow()->Render();
  
}

void VTKInterface::viewRec( bool state )
{
  rec = state;
}

void VTKInterface::stepForward( int steps )
{
  int i;

  for( i=0; i<steps; ++i )
  {
    ++it_timeConf;
    if(it_timeConf == timeConf.end()){
      it_timeConf = timeConf.begin();
    }
  }
  this->updateStep();
}

void VTKInterface::stepBack( int steps )
{
  int i;

  for( i=0; i<steps; ++i )
  {
    if(it_timeConf == timeConf.begin()){
      it_timeConf = timeConf.end();
    }
    --it_timeConf;
  }
  this->updateStep();
}

void VTKInterface::stepFirst( )
{
  it_timeConf = timeConf.begin();
  this->updateStep();
}

void VTKInterface::setStep( int step )
{
//  cout << "timeConf.size() = "<< timeConf.size() << endl;
  it_timeConf = timeConf.begin();
  for( int i=0; i<step; ++i )
    ++it_timeConf;
  this->updateStep();

  if(rec) stepRecord(step);
}

int VTKInterface::getStep( )
{
  std::map<double, std::vector< double > >::iterator itTimeTemp;
  int counter=0;
  for( itTimeTemp = timeConf.begin();
       itTimeTemp!= it_timeConf;
       ++itTimeTemp )
    ++counter;
  return counter;
}

void VTKInterface::updateStep( )
{
  std::map<int, CompNode>::iterator it_nodes;
  std::vector<vtkActor*>::iterator it_spheres;
  std::map<std::string, CompBar>::iterator it_bars;
  std::map<std::string, CompRigid2D>::iterator it_rigid2D;
  std::map<std::string, CompRigid3D>::iterator it_rigid3D;
  std::map<std::string, CompMassPoint>::iterator it_massPoints;
  std::map<std::string, CompMeshfree>::iterator it_meshfree;
  std::map<std::string, CompMeshfree3D>::iterator it_meshfree3D;
  double zero=0.;

  int i=0;

  for( it_nodes = nodes.begin();
      it_nodes!= nodes.end();
      ++it_nodes )
  {
    if(dimension==3){
      it_nodes->second.move(  it_timeConf->second[3*i],
                              it_timeConf->second[3*i+1],
                              it_timeConf->second[3*i+2]
                            );
    }
    else if(dimension==2){
      it_nodes->second.move(  it_timeConf->second[2*i],
                              it_timeConf->second[2*i+1],
                              zero
                           );
    }
    ++i;
  }
  i=0;
  for( it_spheres = spheres.begin();
      it_spheres!= spheres.end();
      ++it_spheres )
  {
    if(dimension==3){
      (*it_spheres)->SetPosition( it_timeConf->second[3*i],
                                it_timeConf->second[3*i+1],
                                it_timeConf->second[3*i+2]
                              );
    }
    else if(dimension==2){
      (*it_spheres)->SetPosition( it_timeConf->second[2*i],
                                  it_timeConf->second[2*i+1],
                                  zero
                                );
    }
    ++i;
  }

  for( it_bars = bars.begin();
      it_bars!= bars.end();
      ++it_bars )
  {
      it_bars->second.updatePoints( );
  }

  for( it_rigid2D = rigid2Ds.begin();
      it_rigid2D!= rigid2Ds.end();
      ++it_rigid2D )
  {
      it_rigid2D->second.updatePoints( getStep() );
  }

  for( it_rigid3D = rigid3Ds.begin();
      it_rigid3D!= rigid3Ds.end();
      ++it_rigid3D )
  {
      it_rigid3D->second.updatePoints( getStep() );
  }

  for( it_massPoints = massPoints.begin();
      it_massPoints!= massPoints.end();
      ++it_massPoints )
  {
      it_massPoints->second.updatePoints( );
  }

  for( it_meshfree = meshfree.begin();
       it_meshfree!= meshfree.end();
       ++it_meshfree )
  {
    it_meshfree->second.updatePoints( );
    it_meshfree->second.updateScalarField( getStep() );
  }
  
  for( it_meshfree3D = meshfree3D.begin();
      it_meshfree3D!= meshfree3D.end();
      ++it_meshfree3D )
    {
      it_meshfree3D->second.updatePoints( );
      it_meshfree3D->second.updateScalarField( getStep() );
    }
  //   theRenderer->GetRenderWindow()->Render();

    double t;
    std::string tLetter="t=";

    t = it_timeConf->first;

    std::ostringstream timeString;
    std::string timeAll;

    timeString << t;
    timeAll = tLetter + timeString.str();

    const char* timeChar;
    timeChar= timeAll.data();

    timeActor = vtkTextActor::New();
    timeTextProperty = vtkTextProperty::New();
    timeTextProperty->SetColor(1.0,1.0,1.0);
//    timeTextProperty->SetFontFamilyToTimes();
    timeTextProperty->SetFontSize(16);
    timeActor->SetInput(timeChar);
    timeActor->SetTextProperty(timeTextProperty);
    timeActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
    timeActor->GetPositionCoordinate()->SetValue(0.05,0.95);
    timeActor->SetHeight(0.5);

    theRenderer->AddActor( timeActor );

    theRenderer->GetRenderWindow()->Render();
    theRenderer->RemoveActor(timeActor);

}

void VTKInterface::refreshTable( CompMeshfree* pMeshfree )
{
  double min_max[2];
  table->GetTableRange( min_max );
  if( min_max[0] == 0. && min_max[1] == 0. ){
    table->SetTableRange( pMeshfree->getMinScalar(),
                          pMeshfree->getMaxScalar()
                        );
  }
  else{
    if( min_max[0] > pMeshfree->getMinScalar() )
      min_max[0] = pMeshfree->getMinScalar();
    if( min_max[1] < pMeshfree->getMaxScalar() )
      min_max[1] = pMeshfree->getMaxScalar();
  }
}

void VTKInterface::refreshTable( CompMeshfree3D* pMeshfree )
{
  double min_max[2];
  table->GetTableRange( min_max );
  if( min_max[0] == 0. && min_max[1] == 0. ){
    table->SetTableRange( pMeshfree->getMinScalar(),
                          pMeshfree->getMaxScalar()
                        );
  }
  else{
    if( min_max[0] > pMeshfree->getMinScalar() )
      min_max[0] = pMeshfree->getMinScalar();
    if( min_max[1] < pMeshfree->getMaxScalar() )
      min_max[1] = pMeshfree->getMaxScalar();
  }
}

void VTKInterface::stepRecord( int step )
{
  vtkWindowToImageFilter* w2if = vtkWindowToImageFilter::New();
  w2if->SetInput(theRenderer->GetRenderWindow());

  vtkJPEGWriter* psw = vtkJPEGWriter::New();
#if VTK_MAJOR_VERSION > 5
  psw->SetInputData(w2if->GetOutput());
#else
  psw->SetInput(w2if->GetOutput());
#endif

  std::ostringstream title;
  if (step <10 )
    title << "image00" << step << ".jpg" ;
  else if (step <100 )
    title << "image0" << step << ".jpg" ;
  else
    title << "image" << step << ".jpg" ;
  psw->SetFileName(title.str().c_str());
  psw->Write();

  psw->Delete();

}

