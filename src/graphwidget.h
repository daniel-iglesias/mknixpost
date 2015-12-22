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
#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QDockWidget>
#include <QMainWindow>
#include <map>

class QMainWindow;
class QLabel;
class QPixmap;
class QComboBox;

namespace cpplot{
  class Plot2D;
}

/**
	@author Daniel Iglesias <diglesias@mecanica.upm.es>
*/
class GraphWidget : public QDockWidget
{
  typedef std::map< QString, std::vector<double> > Properties;

  Q_OBJECT

  public:
    GraphWidget();

    GraphWidget(const QString & title, QWidget * parent );

    ~GraphWidget();

    void readFile( const QString& );

  private:
    void createActions();
    void createMenus();
    void createToolBars();
    void updateTypeBox();
    void createGraphFile( std::string );

  private slots:
    void newGraph();
    void addGraph();
    void updateXNameBox(int);
    void updateYNameBox(int);
    void updateXDataBox(QString);
    void updateYDataBox(QString);

  private:
    QMainWindow * theWidget;
    QLabel * theGraph;
    QPixmap * theGraphMap;

    QMenu *fileMenu;
    QToolBar * fileToolBar;
    QToolBar * xToolBar;
    QToolBar * yToolBar;

    QAction *newAct;
    QComboBox *xTypeBox;
    QComboBox *xNameBox;
    QComboBox *xDataBox;
    QComboBox *yTypeBox;
    QComboBox *yNameBox;
    QComboBox *yDataBox;
    QAction *addGraphAct;

    QLabel * xLabel;
    QLabel * yLabel;

    int dimension;
    std::vector< double > timeLine;
    std::vector< double >::iterator itTime;
    
    std::map<QString, Properties> nodes;
    std::map<QString, Properties> rigids;
    std::map<QString, Properties> flexibles;
    std::map<QString, Properties> joints;
    std::map<QString, Properties>::iterator it;
    Properties::iterator itProp;

    std::map< QString, std::vector<QString> > flexNodes;

    std::vector<cpplot::Plot2D*> vPlots;
    std::vector<cpplot::Plot2D*>::iterator itPlots;

};

#endif
