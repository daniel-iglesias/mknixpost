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

#include "graphwidget.h"
#include "graphics.h"

#if QT_VERSION >= 0x050000
#  include <QtWidgets>
#else
#  include <QtGui/QtGui>
#endif

#include <QPixmap>
#include <QLabel>

#include <sstream>

GraphWidget::GraphWidget()
 : QDockWidget()
{
}


GraphWidget::GraphWidget( const QString & title, QWidget * parent )
  : QDockWidget( title, parent )
{
  theWidget = new QMainWindow;
  theGraph = new QLabel;
  theGraphMap = new QPixmap;
  this->setWidget( theWidget );
  theWidget->setCentralWidget( theGraph );
  
  createActions();
  createMenus();
  createToolBars();

  theWidget->show();

  this->theWidget->adjustSize();
//  this->resize(800,2000);
//  this->move(0,0);

}


GraphWidget::~GraphWidget()
{
  std::ostringstream graphicsName;
  graphicsName << this->windowTitle().toStdString() << ".gnu";
  std::ostringstream systemCommand;
  systemCommand << "rm " << graphicsName.str();
  system( systemCommand.str().c_str() );
}


void GraphWidget::readFile(const QString & file_name )
{
  std::ifstream input;
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
      while(input >> keyword) {
        if(!strcmp(keyword,"ENDSYSTEM")) {
          break;
        }
        else if(!strcmp(keyword,"NODES")) {
          std::string num;
          double x,y,z;
          while(input >> keyword) {
            if(!strcmp(keyword,"ENDNODES")) break;
            else{
              num = keyword;
              input >> x >> y >> z;
              nodes[num.c_str()];
            }
          }
        }
        else if(!strcmp(keyword,"RIGIDBODIES")) {
          std::string name;
          int material, nodeA, nodeB, nodeC, nodeD;
          while(input >> keyword) {
            if(!strcmp(keyword,"ENDRIGIDBODIES")){
              break; 
            }
            else if(!strcmp(keyword,"BAR")){
              input >> name;
              input >> material >> nodeA >> nodeB;
              rigids[name.c_str()];
            }
            else if(!strcmp(keyword,"GENERIC2D")){
              input >> name;
              input >> material >> nodeA >> nodeB >> nodeC;
              rigids[name.c_str()];
            }
            else if(!strcmp(keyword,"GENERIC3D")){
              input >> name;
              input >> material >> nodeA >> nodeB >> nodeC >> nodeD;
              rigids[name.c_str()];
            }
          }
        }
        else if(!strcmp(keyword,"FLEXBODIES")) {
          std::string stdName;
          while(input >> keyword) {
            if(!strcmp(keyword,"ENDFLEXBODIES")){
              break;
            }
            else if(!strcmp(keyword,"NODES")){
              int firstNode, lastNode;
              QString qName(stdName.c_str());
              input >> firstNode >> lastNode;
              for( int i=firstNode; i<=lastNode; ++i){
                // this is a slow conversion from int to QString...
                std::ostringstream converter;
                converter << i;
                flexNodes[qName].push_back( converter.str().c_str() );
              }
            }
            else{
              if(!strcmp(keyword,"MESHFREE") || !strcmp(keyword,"FEMESH")){
                input >> stdName;
                QString qName(stdName.c_str());
                flexibles[qName];
              }
            }
          }
        }
        else if(!strcmp(keyword,"JOINTS")) {
          std::string name;
          int nodeA, nodeB;
          while(input >> keyword) {
            if(!strcmp(keyword,"ENDJOINTS")){
              break;
            }
            else{
              input >> name; // dump method in keyword
              input >> nodeA >> nodeB; // Not used yet
              joints[name.c_str()];
            }
          }
        }
      }
    }
    else if(!strcmp(keyword,"ANALYSIS")) {
      double time, disp_x, disp_y, disp_z;
      std::string stringKeyword;

      while(input >> stringKeyword) {
        if(!strcmp(stringKeyword.c_str(),"ENDANALYSIS")) break;
        else if(!strcmp(stringKeyword.c_str(),"ENERGY")){
          double ePot, eKin, eEla, eTot;
          std::string sBodyName;

          input >> sBodyName;
          std::cout << "ENERGY: " << sBodyName << std::endl;
          QString qBodyName( sBodyName.c_str() );

          it = rigids.find( qBodyName );
          if( it==rigids.end() ){
            it = flexibles.find( qBodyName );
            if( it==flexibles.end() ){
              std::cout << ":::ERROR: ENERGY body not found:::" << std::endl;
            }
          }
          for(itTime = timeLine.begin();
              itTime!= timeLine.end();
              ++itTime
              )
          {
            input >> ePot >> eKin >> eEla >> eTot;
            it->second["Potential-E"].push_back(ePot);
            it->second["Kinetic-E"].push_back(eKin);
            it->second["Elastic-E"].push_back(eEla);
            it->second["Total-E"].push_back(eTot);
          }
        }
        else if(!strcmp(stringKeyword.c_str(),"STRESS")){
          double sx, sy, sxy;
          std::string sFlexBodyName;
          std::vector<QString>::iterator itFlexNodes;

          input >> sFlexBodyName;
          QString qFlexBodyName( sFlexBodyName.c_str() );
          
          if(dimension==2){
            for(itTime = timeLine.begin();
                itTime!= timeLine.end();
                ++itTime
               )
            {
              for( itFlexNodes = flexNodes[qFlexBodyName].begin();
                  itFlexNodes!= flexNodes[qFlexBodyName].end();
                  ++itFlexNodes
                )
              {
                input >> sx >> sy >> sxy;
                nodes[*itFlexNodes]["Sx"].push_back(sx);
                nodes[*itFlexNodes]["Sy"].push_back(sy);
                nodes[*itFlexNodes]["Sxy"].push_back(sxy);
              }
            }
          }
        }
        else if(!strcmp(stringKeyword.c_str(),"TEMPERATURE")){
          std::string sRigidBodyName;          
          input >> sRigidBodyName   ;
          double temp;
          for(itTime = timeLine.begin();
              itTime!= timeLine.end();
              ++itTime
              )
          {
              for(it = nodes.begin();
                  it!= nodes.end();
                  ++it
                )
              {
                input >> temp;
                it->second["temp"].push_back(temp);
              }
          }
        }
        else if(!strcmp(stringKeyword.c_str(),"DOMAIN")){
          double dx,dy,dz;
          int size, i;
          std::string sRigidBodyName;
          std::stringstream pointName;
          
          input >> sRigidBodyName >> size;
          QString qRigidBodyName( sRigidBodyName.c_str() );
          
          it = rigids.find( qRigidBodyName );
          if( it==rigids.end() ){
            std::cout << ":::ERROR: DOMAIN body not found:::" << std::endl;
          }

          for(itTime = timeLine.begin();
              itTime!= timeLine.end();
              ++itTime
              )
          {
            for( i=0; i<size; ++i )
            {
              input >> dx >> dy;
              if (dimension==3){
                input >> dz;
              } // TODO: store data
              std::cout << dy << std::endl;
//              it->second[forceName.str().c_str()].push_back(f_in);
            }
          }
        }
        else if(!strcmp(stringKeyword.c_str(),"FORCES")){
          double f_in;
          int i, size;
          std::string sJointName;
          std::vector<QString>::iterator itJoints;
          
          input >> sJointName >> size;
          QString qJointName( sJointName.c_str() );
          
          it = joints.find( qJointName );
          if( it==joints.end() ){
            std::cout << ":::ERROR: JOINT not found:::" << std::endl;
          }
            
          for(itTime = timeLine.begin();
              itTime!= timeLine.end();
              ++itTime
              )
          {
            for( i=0; i<size; ++i )
            {
              std::stringstream forceName; // Not optimal, but clear does not work
              input >> f_in;
              forceName << "F_" << i;
              it->second[forceName.str().c_str()].push_back(f_in);
//              forceName.clear(); // does not seem to work
            }
          }
        }

        else if(!strcmp(stringKeyword.c_str(),"CONFIGURATION")){
          while(input >> stringKeyword){
            if(!strcmp(stringKeyword.c_str(),"ENDCONFIGURATION")) break;
            time = atof(stringKeyword.c_str());
            timeLine.push_back( time );
            for(it = nodes.begin();
                it!= nodes.end();
                ++it
              )
            {
              input >> disp_x >> disp_y;
              it->second["x"].push_back(disp_x);
              it->second["y"].push_back(disp_y);
              if(dimension==3){
                input >> disp_z;
                it->second["z"].push_back(disp_z);
              }
            }
          }
        }
      }
    }
  }
  updateTypeBox();

}


void GraphWidget::createActions()
{
  newAct = new QAction(QIcon(":/images/filenew.png"), tr("&New Graph"),
                        this);
  newAct->setShortcut(tr("Ctrl+N"));
  newAct->setStatusTip(tr("New graph of loaded model"));
  connect(newAct, SIGNAL(triggered()), this, SLOT(newGraph()));

  addGraphAct = new QAction(QIcon(":/images/viewgraph.png"), tr("&Add Graph..."),
                            this);
  addGraphAct->setShortcut(tr("Ctrl+G"));
  addGraphAct->setStatusTip(tr("Add graph of selected x-y entities"));
  connect(addGraphAct, SIGNAL(triggered()), this, SLOT(addGraph()));

  xTypeBox = new QComboBox( this );
  xTypeBox->setObjectName(QString::fromUtf8("x Axis Type"));
//   xTypeBox->setDisabled(true);
  xTypeBox->setMaximumWidth(400);
  connect(xTypeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateXNameBox(int)));

  xNameBox = new QComboBox( this );
  xNameBox->setObjectName(QString::fromUtf8("x Axis Name"));
  xNameBox->setMaximumWidth(400);
  xNameBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  connect(xNameBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateXDataBox(QString)));

  xDataBox = new QComboBox( this );
  xDataBox->setObjectName(QString::fromUtf8("x Axis Data"));
  xDataBox->setMaximumWidth(400);
  xDataBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);

  yTypeBox = new QComboBox( this );
  yTypeBox->setObjectName(QString::fromUtf8("y Axis Type"));
//   yTypeBox->setDisabled(true);
  yTypeBox->setMaximumWidth(400);
  connect(yTypeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateYNameBox(int)));

  yNameBox = new QComboBox( this );
  yNameBox->setObjectName(QString::fromUtf8("y Axis Name"));
  yNameBox->setMaximumWidth(400);
  yNameBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  connect(yNameBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateYDataBox(QString)));

  yDataBox = new QComboBox( this );
  yDataBox->setObjectName(QString::fromUtf8("y Axis Data"));
  yDataBox->setMaximumWidth(400);
  yDataBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);

}

void GraphWidget::createMenus()
{
  fileMenu = theWidget->menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(newAct);
  fileMenu->addAction(addGraphAct);
}

void GraphWidget::createToolBars()
{
  fileToolBar = new QToolBar(tr("File"));
  theWidget->addToolBar(Qt::TopToolBarArea, fileToolBar);
  fileToolBar->addAction(newAct);
  fileToolBar->addAction(addGraphAct);

  theWidget->addToolBarBreak();
  xToolBar = new QToolBar(tr("x-axis"));
  theWidget->addToolBar(Qt::LeftToolBarArea, xToolBar);
  xLabel = new QLabel(" x-axis:");
  xToolBar->addWidget(xLabel);
  xToolBar->addWidget(xTypeBox);
  xToolBar->addWidget(xNameBox);
  xToolBar->addWidget(xDataBox);
//   xToolBar->setOrientation(Qt::Vertical);

  theWidget->addToolBarBreak();
  yToolBar= new QToolBar(tr("y-axis"));
  theWidget->addToolBar(Qt::LeftToolBarArea, yToolBar);
  yLabel = new QLabel(" y-axis:");
  yToolBar->addWidget(yLabel);
  yToolBar->addWidget(yTypeBox);
  yToolBar->addWidget(yNameBox);
  yToolBar->addWidget(yDataBox);
//   yToolBar->setOrientation(Qt::Vertical);

}

void GraphWidget::updateTypeBox()
{
  if(timeLine.size() > 1){
    xTypeBox->addItem( "Time" );
    yTypeBox->addItem( "Time" );
    if( nodes.size() > 0 ){
      xTypeBox->addItem( "Node" );
      yTypeBox->addItem( "Node" );
    }
    if( rigids.size() > 0 ){
      xTypeBox->addItem( "RigidBody" );
      yTypeBox->addItem( "RigidBody" );
    }
    if( flexibles.size() > 0 ){
      xTypeBox->addItem( "FlexBody" );
      yTypeBox->addItem( "FlexBody" );
    }
    if( joints.size() > 0 ){
      xTypeBox->addItem( "Joint" );
      yTypeBox->addItem( "Joint" );
    }
  }
  updateXNameBox(0);
  updateYNameBox(0);
}


void GraphWidget::updateXNameBox(int index)
{
  if( xTypeBox->currentText() == "Time" ){
    xNameBox->clear( );
  }
  if( xTypeBox->currentText() == "Node" ){
    xNameBox->clear( );
    for(it = nodes.begin();
        it!= nodes.end();
        ++it
      )
    {
      xNameBox->addItem( it->first );
    }
  }
  if( xTypeBox->currentText() == "RigidBody" ){
    xNameBox->clear( );
    for(it = rigids.begin();
        it!= rigids.end();
        ++it
       )
    {
      xNameBox->addItem( it->first );
    }
  }
  if( xTypeBox->currentText() == "FlexBody" ){
    xNameBox->clear( );
    for(it = flexibles.begin();
        it!= flexibles.end();
        ++it
       )
    {
      xNameBox->addItem( it->first );
    }
  }
  if( xTypeBox->currentText() == "Joint" ){
    xNameBox->clear( );
    for(it = joints.begin();
        it!= joints.end();
        ++it
        )
    {
      xNameBox->addItem( it->first );
    }
  }
}

void GraphWidget::updateYNameBox(int index)
{
  if( xTypeBox->currentText() == "Time" ){
    xNameBox->clear( );
  }
  if( yTypeBox->currentText() == "Node" ){
    yNameBox->clear( );
    for(it = nodes.begin();
        it!= nodes.end();
        ++it
       )
    {
      yNameBox->addItem( it->first );
    }
  }
  if( yTypeBox->currentText() == "Rigid" ){
    yNameBox->clear( );
    for(it = rigids.begin();
        it!= rigids.end();
        ++it
       )
    {
      yNameBox->addItem( it->first );
    }
  }
  if( yTypeBox->currentText() == "FlexBody" ){
    yNameBox->clear( );
    for(it = flexibles.begin();
        it!= flexibles.end();
        ++it
       )
    {
      yNameBox->addItem( it->first );
    }
  }
  if( yTypeBox->currentText() == "Joint" ){
    yNameBox->clear( );
    for(it = joints.begin();
        it!= joints.end();
        ++it
        )
    {
      yNameBox->addItem( it->first );
    }
  }
}


void GraphWidget::updateXDataBox(QString name)
{
  if( xTypeBox->currentText() == "Node" ){
    xDataBox->clear( );
    for(itProp = nodes[xNameBox->currentText()].begin();
        itProp!= nodes[xNameBox->currentText()].end();
        ++itProp
       )
    {
      xDataBox->addItem( itProp->first );
    }
  }
  if( xTypeBox->currentText() == "RigidBody" ){
    xDataBox->clear( );
    for(itProp = rigids[xNameBox->currentText()].begin();
        itProp!= rigids[xNameBox->currentText()].end();
        ++itProp
       )
    {
      xDataBox->addItem( itProp->first );
    }
  }
  if( xTypeBox->currentText() == "FlexBody" ){
    xDataBox->clear( );
    for(itProp = flexibles[xNameBox->currentText()].begin();
        itProp!= flexibles[xNameBox->currentText()].end();
        ++itProp
       )
    {
      xDataBox->addItem( itProp->first );
    }
  }
  if( xTypeBox->currentText() == "Joint" ){
    xDataBox->clear( );
    for(itProp = joints[xNameBox->currentText()].begin();
        itProp!= joints[xNameBox->currentText()].end();
        ++itProp
        )
    {
      xDataBox->addItem( itProp->first );
    }
  }
}

void GraphWidget::updateYDataBox(QString name)
{
  if( yTypeBox->currentText() == "Node" ){
    yDataBox->clear( );
    for(itProp = nodes[yNameBox->currentText()].begin();
        itProp!= nodes[yNameBox->currentText()].end();
        ++itProp
       )
    {
      yDataBox->addItem( itProp->first );
    }
  }
  if( yTypeBox->currentText() == "RigidBody" ){
    yDataBox->clear( );
    for(itProp = rigids[yNameBox->currentText()].begin();
        itProp!= rigids[yNameBox->currentText()].end();
        ++itProp
       )
    {
      yDataBox->addItem( itProp->first );
    }
  }
  if( yTypeBox->currentText() == "FlexBody" ){
    yDataBox->clear( );
    for(itProp = flexibles[yNameBox->currentText()].begin();
        itProp!= flexibles[yNameBox->currentText()].end();
        ++itProp
       )
    {
      yDataBox->addItem( itProp->first );
    }
  }
  if( yTypeBox->currentText() == "Joint" ){
    yDataBox->clear( );
    for(itProp = joints[yNameBox->currentText()].begin();
        itProp!= joints[yNameBox->currentText()].end();
        ++itProp
        )
    {
      yDataBox->addItem( itProp->first );
    }
  }
}


void GraphWidget::newGraph()
{
  delete theGraphMap;
  theGraphMap = new QPixmap;
  theGraph->setPixmap( *theGraphMap );

  for(itPlots = vPlots.begin();
      itPlots!= vPlots.end();
      ++itPlots
     )
  {
    delete *itPlots;
  }
  vPlots.clear();

  xTypeBox->setDisabled(false);
  xNameBox->setDisabled(false);
  xDataBox->setDisabled(false);
}


void GraphWidget::addGraph()
{
  std::ostringstream curveName;
  if(yTypeBox->currentText() != "Time"){
    curveName << yTypeBox->currentText().toStdString() << "."
        << yNameBox->currentText().toStdString() << "."
        << yDataBox->currentText().toStdString();
  }
  else if(xTypeBox->currentText() != "Time"){
    curveName << xTypeBox->currentText().toStdString() << "."
        << xNameBox->currentText().toStdString() << "."
        << xDataBox->currentText().toStdString();
  }
  else curveName << "Time";

  createGraphFile(curveName.str());

  vPlots.push_back( new cpplot::Plot2D );
  vPlots.back()->setDataFile(curveName.str());
  vPlots.back()->setLineWidth(3);

  std::ostringstream graphicsName;
  graphicsName << this->windowTitle().toStdString() << ".gnu";
  cpplot::Graphics myGraphics( graphicsName.str() );

  std::ostringstream xTitleName;
  if(xTypeBox->currentText() != "Time"){
    xTitleName << xTypeBox->currentText().toStdString() << "."
        << xNameBox->currentText().toStdString() << "."
        << xDataBox->currentText().toStdString();
  }
  else xTitleName << "Time";
  myGraphics.setXtitle(xTitleName.str());
  myGraphics.setYtitle("");
  myGraphics.setYrange(0.0,0.0);
  myGraphics.setXrange(0.0,0.0);

  for(itPlots = vPlots.begin();
      itPlots!= vPlots.end();
      ++itPlots
     )
  {
    myGraphics.addPlot( *(*itPlots) );
  }
  myGraphics.setOutput(this->windowTitle().toStdString());
  myGraphics.getPlot();

  std::ostringstream systemCommand;
  systemCommand << "gnuplot " << graphicsName.str();
  system( systemCommand.str().c_str() );

  std::ostringstream graphicsFile;
  graphicsFile <<  this->windowTitle().toStdString() << ".png";
  theGraphMap->load( graphicsFile.str().c_str() );
  theGraph->setPixmap( *theGraphMap );

  xTypeBox->setDisabled(true);
  xNameBox->setDisabled(true);
  xDataBox->setDisabled(true);
}

void GraphWidget::createGraphFile( std::string curve_name )
{
  std::ofstream curveFile(curve_name.c_str());
  int counter = 0;
  if(xTypeBox->currentText() == "Time"){
    if(yTypeBox->currentText() == "Node"){
      for( itTime = timeLine.begin();
           itTime!= timeLine.end();
           ++itTime
         )
      {
        curveFile << *itTime << " ";
        curveFile <<
            nodes
            [yNameBox->currentText()]
            [yDataBox->currentText()]
            [counter];
        curveFile << std::endl;
        ++counter;
      }
    }
    else if(yTypeBox->currentText() == "RigidBody"){
      for( itTime = timeLine.begin();
           itTime!= timeLine.end();
           ++itTime
         )
      {
        curveFile << *itTime << " ";
        curveFile <<
            rigids
            [yNameBox->currentText()]
            [yDataBox->currentText()]
            [counter];
        curveFile << std::endl;
        ++counter;
      }
    }
    else if(yTypeBox->currentText() == "FlexBody"){
      for( itTime = timeLine.begin();
           itTime!= timeLine.end();
           ++itTime
         )
      {
        curveFile << *itTime << " ";
        curveFile <<
            flexibles
            [yNameBox->currentText()]
            [yDataBox->currentText()]
            [counter];
        curveFile << std::endl;
        ++counter;
      }
    }
  }
  else if(yTypeBox->currentText() == "Joint"){
    for( itTime = timeLine.begin();
        itTime!= timeLine.end();
        ++itTime
        )
    {
      curveFile << *itTime << " ";
      curveFile <<
      joints
      [yNameBox->currentText()]
      [yDataBox->currentText()]
      [counter];
      curveFile << std::endl;
      ++counter;
    }
  }
  else if(xTypeBox->currentText() == "Node"){
    if(yTypeBox->currentText() == "Time"){
      for( itTime = timeLine.begin();
           itTime!= timeLine.end();
           ++itTime
         )
      {
        curveFile <<
            nodes
            [xNameBox->currentText()]
            [xDataBox->currentText()]
            [counter];
        curveFile << " " << *itTime;
        curveFile << std::endl;
        ++counter;
      }
    }
    else if(yTypeBox->currentText() == "Node"){
      for( itTime = timeLine.begin();
           itTime!= timeLine.end();
           ++itTime
         )
      {
        curveFile <<
            nodes
            [xNameBox->currentText()]
            [xDataBox->currentText()]
            [counter];
        curveFile << " ";
        curveFile <<
            nodes
            [yNameBox->currentText()]
            [yDataBox->currentText()]
            [counter];
        curveFile << std::endl;
        ++counter;
      }
    }
    else if(yTypeBox->currentText() == "RigidBody"){
      for( itTime = timeLine.begin();
           itTime!= timeLine.end();
           ++itTime
         )
      {
        curveFile <<
            nodes
            [xNameBox->currentText()]
            [xDataBox->currentText()]
            [counter];
        curveFile << " ";
        curveFile <<
            rigids
            [yNameBox->currentText()]
            [yDataBox->currentText()]
            [counter];
        curveFile << std::endl;
        ++counter;
      }
    }
    else if(yTypeBox->currentText() == "FlexBody"){
      for( itTime = timeLine.begin();
           itTime!= timeLine.end();
           ++itTime
         )
      {
        curveFile <<
            nodes
            [xNameBox->currentText()]
            [xDataBox->currentText()]
            [counter];
        curveFile << " ";
        curveFile <<
            flexibles
            [yNameBox->currentText()]
            [yDataBox->currentText()]
            [counter];
        curveFile << std::endl;
        ++counter;
      }
    }
    else if(yTypeBox->currentText() == "Joint"){
      for( itTime = timeLine.begin();
          itTime!= timeLine.end();
          ++itTime
          )
      {
        curveFile <<
        nodes
        [xNameBox->currentText()]
        [xDataBox->currentText()]
        [counter];
        curveFile << " ";
        curveFile <<
        joints
        [yNameBox->currentText()]
        [yDataBox->currentText()]
        [counter];
        curveFile << std::endl;
        ++counter;
      }
    }
  }
  else if(xTypeBox->currentText() == "RigidBody"){
    if(yTypeBox->currentText() == "Time"){
      for( itTime = timeLine.begin();
           itTime!= timeLine.end();
           ++itTime
         )
      {
        curveFile <<
            rigids
            [xNameBox->currentText()]
            [xDataBox->currentText()]
            [counter];
        curveFile << " " << *itTime;
        curveFile << std::endl;
        ++counter;
      }
    }
    else if(yTypeBox->currentText() == "Node"){
      for( itTime = timeLine.begin();
           itTime!= timeLine.end();
           ++itTime
         )
      {
        curveFile <<
            rigids
            [xNameBox->currentText()]
            [xDataBox->currentText()]
            [counter];
        curveFile << " ";
        curveFile <<
            nodes
            [yNameBox->currentText()]
            [yDataBox->currentText()]
            [counter];
        curveFile << std::endl;
        ++counter;
      }
    }
    else if(yTypeBox->currentText() == "RigidBody"){
      for( itTime = timeLine.begin();
           itTime!= timeLine.end();
           ++itTime
         )
      {
        curveFile <<
            rigids
            [xNameBox->currentText()]
            [xDataBox->currentText()]
            [counter];
        curveFile << " ";
        curveFile <<
            rigids
            [yNameBox->currentText()]
            [yDataBox->currentText()]
            [counter];
        curveFile << std::endl;
        ++counter;
      }
    }
    else if(yTypeBox->currentText() == "FlexBody"){
      for( itTime = timeLine.begin();
           itTime!= timeLine.end();
           ++itTime
         )
      {
        curveFile <<
            rigids
            [xNameBox->currentText()]
            [xDataBox->currentText()]
            [counter];
        curveFile << " ";
        curveFile <<
            flexibles
            [yNameBox->currentText()]
            [yDataBox->currentText()]
            [counter];
        curveFile << std::endl;
        ++counter;
      }
    }
    else if(yTypeBox->currentText() == "Joint"){
      for( itTime = timeLine.begin();
          itTime!= timeLine.end();
          ++itTime
          )
      {
        curveFile <<
        rigids
        [xNameBox->currentText()]
        [xDataBox->currentText()]
        [counter];
        curveFile << " ";
        curveFile <<
        joints
        [yNameBox->currentText()]
        [yDataBox->currentText()]
        [counter];
        curveFile << std::endl;
        ++counter;
      }
    }
  }
  else if(xTypeBox->currentText() == "FlexBody"){
    if(yTypeBox->currentText() == "Time"){
      for( itTime = timeLine.begin();
           itTime!= timeLine.end();
           ++itTime
         )
      {
        curveFile <<
            flexibles
            [xNameBox->currentText()]
            [xDataBox->currentText()]
            [counter];
        curveFile << " " << *itTime;
        curveFile << std::endl;
        ++counter;
      }
    }
    else if(yTypeBox->currentText() == "Node"){
      for( itTime = timeLine.begin();
           itTime!= timeLine.end();
           ++itTime
         )
      {
        curveFile <<
            flexibles
            [xNameBox->currentText()]
            [xDataBox->currentText()]
            [counter];
        curveFile << " ";
        curveFile <<
            nodes
            [yNameBox->currentText()]
            [yDataBox->currentText()]
            [counter];
        curveFile << std::endl;
        ++counter;
      }
    }
    else if(yTypeBox->currentText() == "RigidBody"){
      for( itTime = timeLine.begin();
           itTime!= timeLine.end();
           ++itTime
         )
      {
        curveFile <<
            flexibles
            [xNameBox->currentText()]
            [xDataBox->currentText()]
            [counter];
        curveFile << " ";
        curveFile <<
            rigids
            [yNameBox->currentText()]
            [yDataBox->currentText()]
            [counter];
        curveFile << std::endl;
        ++counter;
      }
    }
    else if(yTypeBox->currentText() == "FlexBody"){
      for( itTime = timeLine.begin();
           itTime!= timeLine.end();
           ++itTime
         )
      {
        curveFile <<
            flexibles
            [xNameBox->currentText()]
            [xDataBox->currentText()]
            [counter];
        curveFile << " ";
        curveFile <<
            flexibles
            [yNameBox->currentText()]
            [yDataBox->currentText()]
            [counter];
        curveFile << std::endl;
        ++counter;
      }
    }
    else if(yTypeBox->currentText() == "Joint"){
      for( itTime = timeLine.begin();
          itTime!= timeLine.end();
          ++itTime
          )
      {
        curveFile <<
        flexibles
        [xNameBox->currentText()]
        [xDataBox->currentText()]
        [counter];
        curveFile << " ";
        curveFile <<
        joints
        [yNameBox->currentText()]
        [yDataBox->currentText()]
        [counter];
        curveFile << std::endl;
        ++counter;
      }
    }
  }
  else if(xTypeBox->currentText() == "Joint"){
    if(yTypeBox->currentText() == "Time"){
      for( itTime = timeLine.begin();
          itTime!= timeLine.end();
          ++itTime
          )
      {
        curveFile <<
        joints
        [xNameBox->currentText()]
        [xDataBox->currentText()]
        [counter];
        curveFile << " " << *itTime;
        curveFile << std::endl;
        ++counter;
      }
    }
    else if(yTypeBox->currentText() == "Node"){
      for( itTime = timeLine.begin();
          itTime!= timeLine.end();
          ++itTime
          )
      {
        curveFile <<
        joints
        [xNameBox->currentText()]
        [xDataBox->currentText()]
        [counter];
        curveFile << " ";
        curveFile <<
        nodes
        [yNameBox->currentText()]
        [yDataBox->currentText()]
        [counter];
        curveFile << std::endl;
        ++counter;
      }
    }
    else if(yTypeBox->currentText() == "RigidBody"){
      for( itTime = timeLine.begin();
          itTime!= timeLine.end();
          ++itTime
          )
      {
        curveFile <<
        joints
        [xNameBox->currentText()]
        [xDataBox->currentText()]
        [counter];
        curveFile << " ";
        curveFile <<
        rigids
        [yNameBox->currentText()]
        [yDataBox->currentText()]
        [counter];
        curveFile << std::endl;
        ++counter;
      }
    }
    else if(yTypeBox->currentText() == "FlexBody"){
      for( itTime = timeLine.begin();
          itTime!= timeLine.end();
          ++itTime
          )
      {
        curveFile <<
        joints
        [xNameBox->currentText()]
        [xDataBox->currentText()]
        [counter];
        curveFile << " ";
        curveFile <<
        flexibles
        [yNameBox->currentText()]
        [yDataBox->currentText()]
        [counter];
        curveFile << std::endl;
        ++counter;
      }
    }
    else if(yTypeBox->currentText() == "Joint"){
      for( itTime = timeLine.begin();
          itTime!= timeLine.end();
          ++itTime
          )
      {
        curveFile <<
        joints
        [xNameBox->currentText()]
        [xDataBox->currentText()]
        [counter];
        curveFile << " ";
        curveFile <<
        joints
        [yNameBox->currentText()]
        [yDataBox->currentText()]
        [counter];
        curveFile << std::endl;
        ++counter;
      }
    }
  }}

