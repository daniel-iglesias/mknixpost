# The name of our project is "HELLO".  CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
project (MkniPost)

cmake_minimum_required(VERSION 2.6)

include(CPack)

#FIND_PATH(VTK_INCLUDE_DIR vtkpoints.h /usr/local/include/vtk-5.2/)

#set (VTK_DIR /usr/local/lib/vtk-5.8)

# Recurse into the "Hello" and "Demo" subdirectories.  This does not actually
# cause another cmake executable to run.  The same process will walk through
# the project's entire directory structure.
add_subdirectory (src)
#add_subdirectory ()


