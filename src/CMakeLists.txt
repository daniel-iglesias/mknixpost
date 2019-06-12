# compile in debug mode
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

FIND_PACKAGE(VTK REQUIRED)
IF(VTK_FOUND)
  INCLUDE(${VTK_USE_FILE})
  INCLUDE_DIRECTORIES(${VTK_INCLUDE_DIR})
  option(VTK_LEGACY_REMOVE "Remove all legacy code completely." ON)
  option(VTK_LEGACY_SILENT "Silence all legacy code messages." ON)
  mark_as_advanced(VTK_LEGACY_REMOVE VTK_LEGACY_SILENT)
ELSE(VTK_FOUND)
  MESSAGE(FATAL_ERROR "Please set VTK_DIR.")
ENDIF(VTK_FOUND)

OPTION(QT5 "QT5" OFF)

if(QT5)
  MESSAGE("Using Qt5")
  FIND_PACKAGE(Qt5Widgets REQUIRED)
  FIND_PACKAGE(Qt5Gui REQUIRED)
else()
  MESSAGE("Using Qt4")
  FIND_PACKAGE(Qt4 REQUIRED)

  SET(QT_USE_QTOPENGL ON)
  SET(QT_USE_QTWEBKIT OFF)

  INCLUDE(${QT_USE_FILE})
  ADD_DEFINITIONS(${QT_DEFINITIONS})
endif()

FIND_PACKAGE(OpenGL REQUIRED)

# Make sure the compiler can find include files from our Hello library.
INCLUDE_DIRECTORIES(
  ${MknixPost_SOURCE_DIR}/src 
  ${VTK_INCLUDE_DIR}
)

# Make sure the linker can find the Hello library once it is built.
#link_directories (${MknixPost_BINARY_DIR}/libs)
SET(MknixPostSrcs 
  main.cpp 
  compbar.cpp
  compmasspoint.cpp
  compnode.cpp
  comprigid2D.cpp
  comprigid3D.cpp
  compradmap.cpp
  mknixpost.cpp
  vtkinterface.cpp
  compmesh2D.cpp
  compmesh3D.cpp
  compmeshfree.cpp
  compmeshfree3D.cpp
  graphwidget.cpp
  graphics.cpp
  plot2D.cpp
)

SET(MknixPost_HEADERS 
  mknixpost.h
  graphwidget.h
)

SET(MknixPost_RESOURCES
  application.qrc
)

if(QT5)
  QT5_WRAP_CPP(MknixPost_HEADERS_MOC ${MknixPost_HEADERS})
  QT5_ADD_RESOURCES(MknixPost_RESOURCES_RCC ${MknixPost_RESOURCES})
else()
  QT4_WRAP_CPP(MknixPost_HEADERS_MOC ${MknixPost_HEADERS})
  QT4_ADD_RESOURCES(MknixPost_RESOURCES_RCC ${MknixPost_RESOURCES})
endif()

# Add executable called "helloDemo" that is built from the source files
# "demo.cxx" and "demo_b.cxx".  The extensions are automatically found.

ADD_EXECUTABLE(mknixpost
  ${MknixPostSrcs}
  ${MknixPost_HEADERS_MOC}
  ${MknixPost_RESOURCES_RCC}
)

if(QT5)
  QT5_USE_MODULES(mknixpost Widgets Gui)
endif()

INSTALL(TARGETS mknixpost DESTINATION bin)

link_directories(${VTK_DIR})

# Link the executable to the VTK libraries.
TARGET_LINK_LIBRARIES(mknixpost
  ${VTK_LIBRARIES}
  ${QT_LIBRARIES}
  #QVTK
)
