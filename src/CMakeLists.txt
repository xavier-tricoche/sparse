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
set( SAMER_SPARSE_SPARSE_SOURCE_DIR ${CMAKE_SOURCE_DIR} )

set( APSS_INCLUDE_DIR    ${CMAKE_SOURCE_DIR}/../APSS )

# Qt, atlas, CUDA, nvis, vtk, celltree, ITK, CGAL, alglib, MPI, MapReduce, NetCDF, HDF5, gsl,

message( "CUDA: " ${CUDA_SDK_ROOT_DIR} )

# mandatory includes
include_directories( ${Teem_INCLUDE_DIRS} )
include_directories( ${SAMER_SPARSE_SOURCE_DIR} )
include_directories( ${HDF5_INCLUDE_DIRS} )
include_directories( ${MPI_INCLUDE_DIRS} )
include_directories( ${Atlas_INCLUDE_DIRS} )
include_directories( ${GSL_INCLUDE_DIRS} )
include_directories( ${ITK_INCLUDE_DIRS} )
include_directories( ${CGAL_INCLUDE_DIRS} )
include_directories( ${CUDA_SDK_ROOT_DIR} ${CUDA_TOOLKIT_ROOT_DIR} ${CUDA_INCLUDE_DIRS} ${CUDA_SDK_ROOT_DIR}/common/inc ${CUDA_TOOLKIT_ROOT_DIR}/include )
include_directories( . )
include_directories( util )

include( ${VTK_USE_FILE} )


file( GLOB ALGLIB_SRC alglib/*.cpp )

set( SAMER_SPARSE_LIB_SRC
     Sample_point.cpp
     Timer.cpp
     SmoothStepFitting1D.cpp
     DiscreteSibson.cpp
     ${ALGLIB_SRC}
)

add_library( sparse_sampling ${LIBMODE} ${SAMER_SPARSE_LIB_SRC} )
install( TARGETS sparse_sampling
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

add_executable( sparse_flow_map main.cpp )
target_link_libraries( sparse_flow_map
    sparse_sampling
    ${VTK_LIBRARIES}
    ${Teem_LIBRARIES}
    ${NetCDF_LIBRARIES}
    ${Atlas_LIBRARIES}
    ${MPI_LIBRARIES}
    ${HDF5_LIBRARIES}
    ${GSL_LIBRARIES}
    ${LAPACK_LIBRARIES}
    ${ITK_LIBRARIES}
    ${CGAL_LIBRARIES}
    ${CUDA_LIBRARIES}
    ${VTK_LIBRARIES}
    aspss )
