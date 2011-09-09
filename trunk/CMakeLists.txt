cmake_minimum_required (VERSION 2.8)

enable_language (Fortran OPTIONAL)

include(CMakeForceCompiler)
CMAKE_FORCE_CXX_COMPILER(mpicxx "MPI C++ Compiler")
CMAKE_FORCE_C_COMPILER(mpicc "MPI C Compiler")

project(DAMARIS)

set(EXTERNAL_ROOT ~/deploy)

add_definitions (-O2 -Wall -D_BENCHMARK)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${DAMARIS_SOURCE_DIR}/cmake/Modules/")

set(BOOST_ROOT ${EXTERNAL_ROOT})

#set(HDF5_INCLUDE_DIR "${EXTERNAL_ROOT}/include")
#set(HDF5_LIBRARY_DIR "${EXTERNAL_ROOT}/lib")
#FIND_PACKAGE(HDF5 "1.8.6" REQUIRED NO_MODULE)
#set(ZLIB_LIBRARY_DIR "${EXTERNAL_ROOT}/lib")
#set(ZLIB_INCLUDE_DIR "${EXTERNAL_ROOT}/include")

set(XERCESC_LIBRARY_DIR "${EXTERNAL_ROOT}/lib")
set(XERCESC_INCLUDE_DIR "${EXTERNAL_ROOT}/include")
#set(XERCES_ROOT ${EXTERNAL_ROOT})

find_package(Boost REQUIRED)
#find_package(XercesC REQUIRED)
set(BOOST_LIBRARIES boost_filesystem boost_system boost_program_options)

include_directories(${Boost_INCLUDE_DIR})
include_directories(${XercesC_INCLUDE_DIR})
#include_directories(${HDF5_INCLUDE_DIR})
#include_directories(${ZLIB_INCLUDE_DIR})
include_directories(${DAMARIS_SOURCE_DIR})

link_directories(${XERCESC_LIBRARY_DIR})
#link_directories(${HDF5_LIBRARY_DIR})
#link_directories(${ZLIB_LIBRARY_DIR})

if(CMAKE_Fortran_COMPILER)
  message(STATUS "Fortran binding will be enabled, compiler is ${CMAKE_Fortran_COMPILER}")
  include(FortranCInterface)
  add_definitions(-D__ENABLE_FORTRAN)
  FortranCInterface_HEADER(${DAMARIS_SOURCE_DIR}/common/FCMangle.h
			   MACRO_NAMESPACE "FC_FUNC_"
                           SYMBOL_NAMESPACE "FC_SYM_")
else(CMAKE_Fortran_COMPILER)
  message(STATUS "Fortran binding will not be build")
endif(CMAKE_Fortran_COMPILER)

add_subdirectory (client)
add_subdirectory (server)
add_subdirectory (test)
