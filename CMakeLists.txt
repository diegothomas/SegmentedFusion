######################################
## Compiler setting
######################################

cmake_minimum_required( VERSION 3.2 )
set( CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation Directory" )
set(CMAKE_VERBOSE_MAKEFILE 1)
set( PROJ_NAME PROJ_TEST )
project( ${PROJ_NAME} )
message("CmakeCurrentListDIR : ${CMAKE_CURRENT_LIST_DIR}")
######################################
## External libraries
######################################

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake )

# OpenCV
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

# #OpenCL
find_package(OpenCL)
include_directories(${OpenCL_INCLUDE_DIRS})
#target_link_libraries(${PROJ_NAME} ${OpenCL_LIBRARIES})

# #OpenGL 
find_package(OpenGL REQUIRED)
#find_package(GLEW)
#find_package(GLUT)
include_directories(D:/libs/opengl/freeglut-3.0.0/include)
link_directories(D:/libs/opengl/freeglut-3.0.0/build/lib)
include_directories(D:/Libs/opengl/glew-2.1.0/include)

# Kinect
#find_package( KinectSDK REQUIRED )
# Find Kinect SDK v2
find_package( KinectSDK2 REQUIRED )
include_directories( ${KinectSDK2_INCLUDE_DIRS} )
link_directories( ${KinectSDK2_LIBRARY_DIRS} )
# Eigen 
#include_directories(D:/libs/eigen/eigen)
#include_directories(C:/libs/eigen-3.2)
include_directories("$ENV{EIGEN_ROOT}")
message("Eigen path : $ENV{EIGEN_ROOT}")

# infiniTAM
include_directories(${CMAKE_CURRENT_LIST_DIR}/libs/infiniTAM/include)
link_directories(${CMAKE_CURRENT_LIST_DIR}/libs/infiniTAM)


# CUDA
include_directories("C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v9.1/include")
link_directories("C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v9.1/lib/x64")


######################################
## Sources
######################################

set( PROJ_SHARED_DIR ${CMAKE_SOURCE_DIR}/src )

# make DataDirectory.h from DataDirectory.h.in
set( PROJ_SHARED_DATADIR_DIR ${PROJ_SHARED_DIR}/datadir )
set( PROJ_SHARED_DATADIR_INCLUDE ${PROJ_SHARED_DATADIR_DIR}/DirectoryConfig.h )
configure_file ( "${CMAKE_SOURCE_DIR}/DataDirectory.h.in" ${PROJ_SHARED_DATADIR_INCLUDE} )

include_directories( ${PROJ_SHARED_DATADIR_DIR} )
include_directories( "D:/libs" )

link_directories("D:/programs/libs")
link_directories("C:/programs/libs")




######################################
## Project
######################################

set( PROJ_SUB_DIR ${CMAKE_SOURCE_DIR}/proj )
add_subdirectory( ${PROJ_SUB_DIR} )
