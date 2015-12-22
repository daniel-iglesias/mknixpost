SOURCES += main.cpp \
compbar.cpp \
compmasspoint.cpp \
compnode.cpp \
mknixpost.cpp \
vtkinterface.cpp \
 compmeshfree.cpp \
 compmeshfree3D.cpp \
 graphwidget.cpp \
 graphics.cpp \
 plot2D.cpp
TEMPLATE = app
CONFIG += warn_on \
	  thread \
          qt \
 debug
TARGET = mknixpost

HEADERS += compbar.h \
compmasspoint.h \
compnode.h \
mknixpost.h \
vtkinterface.h \
 compmeshfree.h \
 compmeshfree3D.h \
 graphwidget.h \
 graphics.h \
 plot2D.h
RESOURCES += application.qrc

QT += core gui widgets


QMAKE_LIBDIR += /usr/local/lib/vtk-5.8

INCLUDEPATH += /usr/local/include/vtk-5.8/ \
               /usr/include/qt/Qt/QtGui/ \
               /usr/include/qt/Qt/

LIBS += -lQVTK \
-lvtkHybrid \
-lvtkRendering \
-lvtkftgl \
-lvtkFiltering \
-lvtkGraphics \
-lvtkIO \
-lvtkCommon \
-lvtkWidgets \
-lvtkImaging \
-lvtkViews \
-lvtkInfovis \
#-lvtkParallel \
-lvtkverdict \
-lvtkfreetype \
-lvtkmetaio \
-lvtksys \
-lvtksqlite \
-lvtkpng \
-lvtkDICOMParser \
-lvtktiff \
-lvtkjpeg \
-lvtkexpat \
-lvtkexoIIc \
-lvtkNetCDF \
-lvtklibxml2 \
-lvtkzlib
CONFIG -= release

