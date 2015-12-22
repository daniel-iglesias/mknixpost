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

#include "mknixpost.h"

#include <sstream>

#if QT_VERSION >= 0x050000
#  include <QtWidgets>
#else
#  include <QtGui/QtGui>
#endif

#include "QVTKWidget.h"

#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkTextProperty.h>
#include <vtkCaptionActor2D.h>
#include <vtkMapper.h>

#include "vtkinterface.h"
#include "graphwidget.h"

MainWindow::MainWindow()
  : ren(0)
    , renLittle(0)
 {
     this->resize(1200,700);

     vtkWidget = new QVTKWidget;
     QSizePolicy sizePolicy2(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
     sizePolicy2.setHorizontalStretch(1);
     sizePolicy2.setVerticalStretch(1);
     sizePolicy2.setHeightForWidth(vtkWidget->sizePolicy().hasHeightForWidth());
//     vtkWidget->setSizePolicy(sizePolicy2);
     vtkWidget->setMinimumSize(QSize(100, 100));

     setCentralWidget(vtkWidget);

     createActions();
     createMenus();
     createToolBars();
     createStatusBar();
     createDockWindows();

     setUpVTKInterface();
     setWindowTitle(tr("MkniXPost"));
 }


MainWindow::~MainWindow()
{
  delete [] dock; dock=0;
}


void MainWindow::fileOpen()
{
  if (fileOpened) fileClose();
  fileName = QFileDialog::getOpenFileName( this,
                                             tr("open file dialog"),
                                             "",
                                             tr("MecBryan geometry (*.mec);;All files(*.*)")
                                           );
  if ( !fileName.isEmpty() ){
    load( );
    statusBar()->showMessage( tr("Model loaded"), 2000 );
  }
  else
    statusBar()->showMessage( tr("Loading aborted"), 2000 );

//   if(!axisAct->isChecked())
//     axisAct->toggle();
  fileOpened = true;
}

void MainWindow::load( )
{
  QFile f( fileName );
//   if ( !f.open( QIODevice::ReadOnly ) )
//     return;

  theInterface->readFile( fileName );

  std::vector<QString> tempStrings = theInterface->getResultsNames();
  std::vector<QString>::iterator itString;
  for( itString = tempStrings.begin();
       itString!= tempStrings.end();
       ++itString
     )
  {
    contourType->addItem( *itString );
  }
  
  tempStrings.clear();
  tempStrings = theInterface->getComponentsNames();
  for( itString = tempStrings.begin();
      itString!= tempStrings.end();
      ++itString
      )
  {
    componentsList->addItem( *itString );
  }
  dock->show();

  tempStrings.clear();
  tempStrings = theInterface->getEnvironmentsNames();
  for( itString = tempStrings.begin();
      itString!= tempStrings.end();
      ++itString
      )
  {
    environmentType->addItem( *itString );
  }

  //   renLittle = ren;
     animateTimeLine->setMaximum( theInterface->getNumberOfSteps()-1 );
     animateTimeLine->setPageStep( theInterface->getNumberOfSteps()/10 );
     animateTimeLine->setValue(0);
     animateTimeLine->setSliderPosition(0);
     animationMenu->setEnabled(true);
     animationToolBar->setVisible(true);
     animationToolBar->setEnabled(true);
}

void MainWindow::setUpVTKInterface()
 {
   ren = vtkRenderer::New();
//    ren->SetBackground( 0.1, 0.2, 0.4 ); // Blue
//  theRenderer->SetBackground( 1, 1, 1); // white
   ren->GradientBackgroundOn();
   ren->SetBackground(0.41,0.56,1.0);
   ren->SetBackground2(0.8,0.86,1);
   renLittle = vtkRenderer::New();
   theInterface = new VTKInterface( ren );
   vtkWidget->GetRenderWindow()->AddRenderer( ren );
//   vtkWidgetLittle->GetRenderWindow()->AddRenderer( renLittle );
 }

void MainWindow::fileClose()
{
//   delete theInterface;
//   setUpVTKInterface();
//   if(axisAct->isChecked())
//     axisAct->toggle();
//   fileOpened = false;

  viewContour(0);
  contoursAct->setChecked(false);
  std::vector<QString> tempStrings = theInterface->getResultsNames();
  contourType->setCurrentIndex(0);
  for( int i=tempStrings.size(); i>0; --i ){
    contourType->removeItem( i );
  }
  tempStrings.clear();
  tempStrings = theInterface->getResultsNames();
  environmentType->setCurrentIndex(0);
  for( int i=tempStrings.size(); i>0; --i ){
    environmentType->removeItem( i );
  }
  viewToggleContours();
  theInterface->closeFile() ;
  ren->GetRenderWindow()->Render();
  fileOpened = false;
  animationMenu->setEnabled(false);
  animationToolBar->setEnabled(false);
  animationToolBar->setVisible(false);
}

void MainWindow::fileRefresh()
{
  if (fileOpened){
    fileClose();
    if ( !fileName.isEmpty() ){
      load( );
      statusBar()->showMessage( tr("Model refreshed"), 2000 );
    }
  }
  fileOpened = true;
}

 void MainWindow::filePrint()
 {
     statusBar()->showMessage(tr("Ready"), 2000);
 }

 void MainWindow::fileSave()
 {
     QString fileName = QFileDialog::getSaveFileName(this,
                         tr("Choose a file name"), ".",
                         tr("HTML (*.html *.htm)"));
     if (fileName.isEmpty())
         return;
     QFile file(fileName);
     if (!file.open(QFile::WriteOnly | QFile::Text)) {
         QMessageBox::warning(this, tr("Dock Widgets"),
                              tr("Cannot write file %1:\n%2.")
                              .arg(fileName)
                              .arg(file.errorString()));
         return;
     }

     QApplication::setOverrideCursor(Qt::WaitCursor);
     QApplication::restoreOverrideCursor();

     statusBar()->showMessage(tr("Saved '%1'").arg(fileName), 2000);
 }

 void MainWindow::editUndo()
 {
 }

 void MainWindow::helpAbout()
 {
    QMessageBox::about(this, tr("MkniXPost v0.4"),
             tr("MkniXPost is the post-processor for MkniX multibody simulation package.\n\n"
                "Version 0.4 \n\n"
                "Authors: \nDaniel Iglesias\nRoberto Ortega"));
 }

 void MainWindow::createActions()
 {

     openAct = new QAction(QIcon(":/images/fileopen.png"), tr("&Open File..."),
                                this);
     openAct->setShortcut(tr("Ctrl+O"));
     openAct->setStatusTip(tr("Open a Geometry/Results"));
     connect(openAct, SIGNAL(triggered()), this, SLOT(fileOpen()));

     refreshAct = new QAction(QIcon(":/images/filerefresh.png"), tr("&Refresh Data..."),
                                this);
     refreshAct->setShortcut(tr("F5"));
     openAct->setStatusTip(tr("Refresh Geometry/results"));
     connect(refreshAct, SIGNAL(triggered()), this, SLOT(fileRefresh()));

     saveAct = new QAction(QIcon(":/images/filesave.png"), tr("&Save..."), this);
     saveAct->setShortcut(tr("Ctrl+S"));
     saveAct->setStatusTip(tr("Save the file"));
     connect(saveAct, SIGNAL(triggered()), this, SLOT(fileSave()));

     closeAct = new QAction(QIcon(":/images/fileclose.png"), tr("&Close"), this);
     closeAct->setShortcut(tr("Ctrl+W"));
     closeAct->setStatusTip(tr("Close the file"));
     connect(closeAct, SIGNAL(triggered()), this, SLOT(fileClose()));

     printAct = new QAction(QIcon(":/images/fileprint.png"), tr("&Print..."), this);
     printAct->setShortcut(tr("Ctrl+P"));
     printAct->setStatusTip(tr("Print the current caption"));
     connect(printAct, SIGNAL(triggered()), this, SLOT(filePrint()));

     undoAct = new QAction(QIcon(":/images/editundo.png"), tr("&Undo"), this);
     undoAct->setShortcut(tr("Ctrl+Z"));
     undoAct->setStatusTip(tr("Undo the last action"));
     connect(undoAct, SIGNAL(triggered()), this, SLOT(editUndo()));

     quitAct = new QAction(QIcon(":/images/filequit.png"), tr("&Quit"), this);
     quitAct->setShortcut(tr("Ctrl+Q"));
     quitAct->setStatusTip(tr("Quit the application"));
     connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

     aboutAct = new QAction(tr("&About"), this);
     aboutAct->setStatusTip(tr("Show the application's About box"));
     connect(aboutAct, SIGNAL(triggered()), this, SLOT(helpAbout()));

     aboutQtAct = new QAction(tr("About &Qt"), this);
     aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
     connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

     axisAct = new QAction( QIcon(":/images/viewaxis.png"), tr("Toggle Axis"), this);
     axisAct->setCheckable( true );
     axisAct->setStatusTip(tr("Toggles reference axis"));
     connect( axisAct, SIGNAL(triggered()), this, SLOT( viewToggleAxis() ) );

     contoursAct = new QAction( QIcon(":/images/viewscalar.png"), tr("Toggle Contours"), this);
     contoursAct->setCheckable( true );
     contoursAct->setStatusTip(tr("Toggles Contours of Scalar Values in Nodes"));
     connect( contoursAct, SIGNAL(triggered()), this, SLOT( viewToggleContours() ) );

     contourType = new QComboBox( this );
     contourType->setObjectName(QString::fromUtf8("Contour Type"));
     contourType->setDisabled(true);
     contourType->setMaximumWidth(400);
     contourType->addItem(QString::fromUtf8("No Contour"));
     connect( contoursAct, SIGNAL(toggled(bool)), contourType, SLOT(setEnabled(bool)) );
     connect( contourType, SIGNAL(currentIndexChanged(int)), this, SLOT(viewContour(int)) );

     environmentType = new QComboBox( this );
     environmentType->setObjectName(QString::fromUtf8("Environemt Type"));
     environmentType->setDisabled(true);
     environmentType->setMaximumWidth(400);
     environmentType->addItem(QString::fromUtf8("No Environment"));
     connect( contoursAct, SIGNAL(toggled(bool)), environmentType, SLOT(setEnabled(bool)) );
     connect( environmentType, SIGNAL(currentIndexChanged(int)), this, SLOT(viewEnvironment(int)) );
     
     graphAct = new QAction( QIcon(":/images/viewgraph.png"), tr("New Graph"), this);
     graphAct->setCheckable( false );
     graphAct->setStatusTip(tr("Creates a new graph widget"));
     connect( graphAct, SIGNAL(triggered()), this, SLOT( viewNewGraph() ) );

     animateForwAct = new QAction( QIcon(":/images/animationplayf.png"), tr("Animate forward"), this);
     animateForwAct->setCheckable( true );
     animateForwAct->setStatusTip(tr("Run animation forward"));
     connect( animateForwAct, SIGNAL(triggered()), this, SLOT(viewAnimate()) );

     animateBackAct = new QAction( QIcon(":/images/animationplayb.png"), tr("Animate backward"), this);
     animateBackAct->setCheckable( true );
     animateBackAct->setStatusTip(tr("Run animation backward"));
     connect( animateBackAct, SIGNAL(triggered()), this, SLOT(viewAnimate()) );

     animatePauseAct = new QAction( QIcon(":/images/animationpause.png"), tr("Pause animation"), this);
     animatePauseAct->setCheckable( true );
     animatePauseAct->setStatusTip(tr("Stops animation in current frame"));
     connect( animatePauseAct, SIGNAL(triggered()), this, SLOT(viewAnimate()) );

     animateRecAct = new QAction( QIcon(":/images/animationrec.png"), tr("Record animation"), this);
     animateRecAct->setCheckable( true );
     animateRecAct->setStatusTip(tr("Records animation"));
     connect( animateRecAct, SIGNAL(triggered()), this, SLOT(viewRec()) );

     animateStopAct = new QAction( QIcon(":/images/animationstop.png"), tr("Stop animation"), this);
     animateStopAct->setCheckable( true );
     animateStopAct->setStatusTip(tr("Stops animation and goes back to start"));
     connect( animateStopAct, SIGNAL(triggered()), this, SLOT(viewAnimate()) );

     animateGroup = new QActionGroup(this);
     animateGroup->addAction(animateForwAct);
     animateGroup->addAction(animateBackAct);
     animateGroup->addAction(animatePauseAct);
     animateGroup->addAction(animateStopAct);
     animateStopAct->setChecked( true );

     animateTimeLine = new QSlider( this );
     animateTimeLine->setObjectName(QString::fromUtf8("animateTimeLine"));
     animateTimeLine->setOrientation(Qt::Horizontal);
//      animateTimeLine->setMaximumWidth(400);
     connect( animateTimeLine, SIGNAL(valueChanged(int)), this, SLOT(setFrame(int)) );

     timer = new QTimer( this );
     timer->setInterval( 100 );
     connect(timer, SIGNAL(timeout()), this, SLOT(nextFrame()));
 }

 void MainWindow::createMenus()
 {
     fileMenu = menuBar()->addMenu(tr("&File"));
     fileMenu->addAction(openAct);
     fileMenu->addAction(refreshAct);
     fileMenu->addAction(saveAct);
     fileMenu->addAction(closeAct);
     fileMenu->addAction(printAct);
     fileMenu->addSeparator();
     fileMenu->addAction(quitAct);

     editMenu = menuBar()->addMenu(tr("&Edit"));
     editMenu->addAction(undoAct);

     viewMenu = menuBar()->addMenu(tr("&View"));
     viewMenu->addAction(axisAct);
     viewMenu->addAction(graphAct);
     viewMenu->addAction(contoursAct);

     animationMenu = viewMenu->addMenu(tr("&Animate"));
     animationMenu->setEnabled(false);
     animationMenu->addAction(animateStopAct);
     animationMenu->addAction(animateForwAct);
     animationMenu->addAction(animatePauseAct);
     animationMenu->addAction(animateBackAct);

     menuBar()->addSeparator();

     helpMenu = menuBar()->addMenu(tr("&Help"));
     helpMenu->addAction(aboutAct);
     helpMenu->addAction(aboutQtAct);
 }

 void MainWindow::createToolBars()
 {
     fileToolBar = addToolBar(tr("File"));
     fileToolBar->addAction(openAct);
     fileToolBar->addAction(refreshAct);
     fileToolBar->addAction(saveAct);
     fileToolBar->addAction(closeAct);
     fileToolBar->addAction(printAct);
     fileToolBar->addSeparator();
     fileToolBar->addAction(quitAct);

     editToolBar = addToolBar(tr("Edit"));
     editToolBar->addAction(undoAct);

     viewToolBar = addToolBar(tr("View"));
     viewToolBar->addAction(axisAct);
     viewToolBar->addAction(graphAct);
     viewToolBar->addAction(contoursAct);
     viewToolBar->addWidget(contourType);
     viewToolBar->addWidget(environmentType);

     this->addToolBarBreak();

     animationToolBar = addToolBar(tr("Animation"));
     animationToolBar->setEnabled(false);
     animationToolBar->setVisible(false);
     animationToolBar->addAction(animateStopAct);
     animationToolBar->addAction(animateBackAct);
     animationToolBar->addAction(animatePauseAct);
     animationToolBar->addAction(animateRecAct);
     animationToolBar->addAction(animateForwAct);
     animationToolBar->addWidget(animateTimeLine);
}

 void MainWindow::createStatusBar()
 {
     statusBar()->showMessage(tr("Ready"));
 }

 void MainWindow::createDockWindows()
 {
     dock = new QDockWidget(tr("View-1"), this);
     dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
     dock->setGeometry(this->width(),
                       int(this->height()*.3),
                       this->width()/3,
                       this->height()/3
                      );
     vtkWidgetLittle = new QVTKWidget(dock);
     vtkWidgetLittle->setMinimumSize(QSize(100, 100));
     vtkWidgetLittle->resize(this->width()/3, this->height()/3);
     dock->setWidget(vtkWidgetLittle);

     addDockWidget(Qt::RightDockWidgetArea, dock);
     viewMenu->addAction(dock->toggleViewAction());
     dock->hide();
     dock->setFloating(true);

     dock = new QDockWidget(tr("Components"), this);
     componentsList = new QListWidget(dock);
//     componentsList->addItems(QStringList()
//             << "BAR: BarraM"
//             << "SPHERICAL: suelo");
     componentsList->resize(this->width()/4, 2*this->height()/3);
     dock->setWidget(componentsList);
     addDockWidget(Qt::RightDockWidgetArea, dock);
     viewMenu->addAction(dock->toggleViewAction());
     dock->hide();
 }

void MainWindow::viewToggleAxis()
{
  if(axisAct->isChecked()){

    /* Define the axis */
    axes = vtkAxesActor::New();
    widget = vtkOrientationMarkerWidget::New();

    double a[3];

    axes->SetShaftTypeToCylinder();
    axes->SetXAxisLabelText( "X" );
    axes->SetYAxisLabelText( "Y" );
    axes->SetZAxisLabelText( "Z" );
    axes->SetTotalLength( 20.0, 20.0, 20.0 );
    axes->SetCylinderRadius( 1.0 * axes->GetCylinderRadius() );
    axes->SetConeRadius    ( 0.7 * axes->GetConeRadius() );
    axes->GetNormalizedTipLength(a);
    axes->SetNormalizedTipLength(2.0*a[0],2.0*a[1],2.0*a[2]);

    vtkTextProperty* tprop = axes->GetXAxisCaptionActor2D()->GetCaptionTextProperty();
    tprop->ItalicOff();
    tprop->ShadowOff();
//     tprop->SetFontFamilyToCourier();
    tprop->SetColor(1,1,1);

    axes->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->ShallowCopy( tprop );
    axes->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->ShallowCopy( tprop );

  // this static function improves the appearance of the text edges
  // since they are overlaid on a surface rendering of the cube's faces
    vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();

  // set up the widget
  //   widget->SetOutlineColor( 0.9300, 0.5700, 0.1300 );
    widget->SetOrientationMarker( axes );
    widget->SetInteractor( vtkWidget->GetInteractor() );
    widget->SetViewport( -.1, -.1, 0.27, 0.35 );
    widget->SetEnabled( 1 );
    widget->InteractiveOff();
//     widget->InteractiveOn();

//     cout << "Activating axis..." << endl;
  }
  else if( !axisAct->isChecked() ){
    axes->Delete();
    widget->SetEnabled( 0 );
    ren->GetRenderWindow()->Render();
//     widget->Delete();
//     cout << "Deactivating axis..." << endl;
  }
    ren->GetRenderWindow()->Render();

}

void MainWindow::viewToggleContours()
{
  theInterface->toggleContourBar( contoursAct->isChecked() );
  if( contoursAct->isChecked() ) viewContour( contourType->currentIndex() );
  else viewContour( 0 );
}

void MainWindow::viewContour( int index )
{
  if( contourType->count() > 1)
    theInterface->viewContour( index );
}

void MainWindow::viewEnvironment( int index )
{
  if( environmentType->count() > 1)
    theInterface->viewEnvironment( index );
}


void MainWindow::viewNewGraph()
{
  std::ostringstream size;
  size << "Graph-" << vGraphs.size();
//   graphTitle.append( size.str() );
  QString graphTitle( size.str().c_str() );
  vGraphs.push_back( new GraphWidget(graphTitle, this) );

  vGraphs.back()->readFile( fileName );

  vGraphs.back()->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  addDockWidget(Qt::BottomDockWidgetArea, vGraphs.back());
  if(this->isMaximized()){
    vGraphs.back()->setFloating(true);
    vGraphs.back()->setGeometry(this->width()*.55,
                                int(this->height()*.1),
                                600,
                                this->height()/10
                              );
  }
  else{
    vGraphs.back()->setFloating(true);
    vGraphs.back()->setGeometry(this->width(),
                                int(this->height()*.3),
                                100,
                                350
                               );
  }
  vGraphs.back()->show();
}


void MainWindow::viewAnimate()
{
    if ( animateForwAct->isChecked() ) timer->start(1);
    else if ( animateBackAct->isChecked() ) timer->start(1);
    else if ( animatePauseAct->isChecked() ) timer->stop();
    else{
      timer->stop();
      animateTimeLine->setValue( 0 );
//       theInterface->stepFirst();
    }
}

void MainWindow::viewRec()
{
  theInterface->viewRec( animateRecAct->isChecked() );
}

void MainWindow::nextFrame()
{
  if ( animateForwAct->isChecked() ){
//     theInterface->stepForward();
    animateTimeLine->setValue( animateTimeLine->value()+1 );
  }
  else{
//     theInterface->stepBack();
    animateTimeLine->setValue( animateTimeLine->value()-1 );
  }
}

void MainWindow::setFrame( int frame )
{
  theInterface->setStep( frame );
}
