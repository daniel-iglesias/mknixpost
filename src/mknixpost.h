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


#ifndef MKNIXPOST_H
#define MKNIXPOST_H

 #include <QMainWindow>
 class QHBoxLayout;
 class QAction;
 class QActionGroup;
 class QListWidget;
 class QSlider;
 class QMenu;
 class QComboBox;
 class QTextEdit;
 class QVTKWidget;
 class QTimer;
 class QDockWidget;

 class vtkRenderer;
 class vtkAxesActor;
 class vtkOrientationMarkerWidget;

 class VTKInterface;
 class GraphWidget;

 class MainWindow : public QMainWindow
 {
     Q_OBJECT

 public:
     MainWindow();
     ~MainWindow();

 private slots:
     void fileOpen();
     void fileClose();
     void fileRefresh();
     void fileSave();
     void filePrint();
     void editUndo();
     void viewToggleAxis();
     void viewToggleContours();
     void viewContour(int);
     void viewEnvironment(int);
     void viewNewGraph();
     void viewAnimate();
     void viewRec();
     void nextFrame();
     void setFrame(int);
     void helpAbout();

 private:
     void createActions();
     void createMenus();
     void createToolBars();
     void createStatusBar();
     void createDockWindows();
     void setUpVTKInterface();
     void load( );

     QHBoxLayout *hboxLayout;
     QTextEdit *textEdit;
     QVTKWidget *vtkWidget;
     QVTKWidget *vtkWidgetLittle;
     QListWidget *componentsList;

     QMenu *fileMenu;
     QMenu *editMenu;
     QMenu *viewMenu;
     QMenu *animationMenu;
     QMenu *helpMenu;
     QToolBar *fileToolBar;
     QToolBar *editToolBar;
     QToolBar *viewToolBar;
     QToolBar *animationToolBar;
     QAction *openAct;
     QAction *refreshAct;
     QAction *saveAct;
     QAction *closeAct;
     QAction *printAct;
     QAction *undoAct;
     QAction *axisAct;
     QAction *contoursAct;
     QAction *graphAct;
     QComboBox *contourType;
     QComboBox *environmentType;
     QAction *animateForwAct;
     QAction *animateBackAct;
     QAction *animateStopAct;
     QAction *animatePauseAct;
     QAction *animateRecAct;
     QSlider *animateTimeLine;
     QActionGroup *animateGroup;
     QAction *aboutAct;
     QAction *aboutQtAct;
     QAction *quitAct;

     QTimer* timer;

     QDockWidget *dock;
     
     vtkRenderer* ren;
     vtkRenderer* renLittle;
     vtkAxesActor* axes;
     vtkOrientationMarkerWidget* widget;

     std::vector< GraphWidget* > vGraphs;

     QString fileName;
     VTKInterface * theInterface;
     bool fileOpened;
 };

#endif
