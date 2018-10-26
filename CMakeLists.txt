#*************************************************************************
# sparse: Efficient Computation of the Flow Map from Sparse Samples
#
# Author: Samer S. Barakat
#
# Copyright (c) 2010-2013, Purdue University
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#**************************************************************************
cmake_minimum_required(VERSION 2.8)

# Run cmake --help-policy CMP<num> to see documentation.
if( COMMAND cmake_policy )
  cmake_policy( SET CMP0003 NEW )
  cmake_policy( SET CMP0012 NEW )
endif( COMMAND cmake_policy )

project( SPARSE_FLOW_MAP )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake" )

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Debug CACHE STRING
      "Choose the type of build, options are: Debug Release Public"
      FORCE)
endif()

option( BUILD_OPTION_OPENMP "use openmp parallelization" OFF )

set( LIBMODE SHARED )

#######################################################################

set( ASPSS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/ASPSS )
set( SPARSE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/sparse )

# Qt, atlas, CUDA, nvis, vtk, celltree, ITK, CGAL, alglib, MPI, MapReduce, NetCDF, HDF5, gsl,


find_package( GSL REQUIRED )
find_package( Qt5 COMPONENTS Core Gui REQUIRED )
find_package( Octave )
find_package( LAPACK REQUIRED )  # linear algebra
find_package( BLAS   REQUIRED )
find_package( Teem   REQUIRED )  # I/O / image processing
find_package( ITK    REQUIRED )
find_package( Atlas  )
find_package( GSL    REQUIRED )       # numerical analysis
find_package( HDF5   REQUIRED )      # I/O
find_package( MPI    REQUIRED )       # I/O
find_package( NetCDF REQUIRED )    # I/O
find_package( VTK    REQUIRED )       # graphics / visualization
find_package( CUDA   REQUIRED )
find_package( CGAL   REQUIRED )
find_package( VTK    REQUIRED )

set( NVIS_DIR ${CMAKE_SOURCE_DIR}/nvis )


# mandatory includes
include_directories( ${Teem_INCLUDE_DIRS} )
include_directories( ${SAMER_SPARSE_SOURCE_DIR} )
include_directories( ${HDF5_INCLUDE_DIRS} )
include_directories( ${MPI_INCLUDE_DIRS} )
include_directories( ${Atlas_INCLUDE_DIRS} )
include_directories( ${GSL_INCLUDE_DIRS} )
include_directories( ${ITK_INCLUDE_DIRS} )
include_directories( ${CGAL_INCLUDE_DIRS} )
include_directories( . )
include_directories( alglib )
include_directories( ${NVIS_DIR} )

message( "Qt5Core_INCLUDE_DIRS: " ${Qt5Core_INCLUDE_DIRS} )

message( "Qt5GUI_INCLUDE_DIRS: " ${Qt5Gui_INCLUDE_DIRS} )
message( "MPI INCLUDE: " ${MPI_INCLUDE_DIRS} )
message( "MPI LIBS: " ${MPI_LIBRARIES} )


string( STRIP ${MPI_LIBRARIES} MPI_LIBRARIES )

# mandatory includes
include_directories( ${ASPSS_SOURCE_DIR} )
include_directories( ${GSL_INCLUDE_DIRS} )
include_directories( ${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} )
include_directories( . )

if( OCTAVE_FOUND )
    include_directories( ${OCTAVE_INCLUDE_DIRS} )
    set( EXPE_HAVE_OCTAVE 1 )
endif()

#######################################################################

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-c++17-extensions -Wno-unused-result -Wno-deprecated-declarations -Wno-deprecated-register" )

if( BUILD_OPTION_OPENMP )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp" )
endif()

#######################################################################
# set build product destinations

set( LIBRARY_OUTPUT_PATH    ${CMAKE_BINARY_DIR}/lib )
set( EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin )

link_directories( ${LIBRARY_OUTPUT_PATH} )

add_subdirectory( ASPSS )
add_subdirectory( src )

install( DIRECTORY bin DESTINATION . USE_SOURCE_PERMISSIONS)
install( DIRECTORY scripts DESTINATION . USE_SOURCE_PERMISSIONS)
install( DIRECTORY utils/GLSLShaderCode DESTINATION utils USE_SOURCE_PERMISSIONS)




######################################################################
# install all binaries
######################################################################
foreach( target ${SAMER_SPARSE_TARGETS} )
  install( TARGETS ${target}
        RUNTIME DESTINATION bin
  )
endforeach( target ${SAMER_SPARSE_TARGETS} )
