cmake_minimum_required (VERSION 2.8)

#-----------------------------------------------------------#
# Enabling Fortran
#-----------------------------------------------------------#
enable_language (Fortran OPTIONAL)

#-----------------------------------------------------------#
# Force MPI compilers
#   starting from Damaris 0.4, MPI compiler is mandatory
#-----------------------------------------------------------#
include(CMakeForceCompiler)
CMAKE_FORCE_CXX_COMPILER(mpicxx "MPI C++ Compiler")
CMAKE_FORCE_C_COMPILER(mpicc "MPI C Compiler")
#CMAKE_FORCE_FORTRAN_COMPILER(mpif90 "Fortran Compiler")

#-----------------------------------------------------------#
# Starting project definition
#-----------------------------------------------------------#
project(DAMARIS)

#-----------------------------------------------------------#
# EXTERNAL_ROOT is a variable set once and for all to
# indicate where are all the required libraries. By default
# CMake will look for libraries in this directory.
#-----------------------------------------------------------#
set(EXTERNAL_ROOT ~/deploy)

#-----------------------------------------------------------#
add_definitions (-O2 -Wall -rdynamic)
add_definitions (-D__ENABLE_MPI) # this line to be deleted
#-----------------------------------------------------------#

#-----------------------------------------------------------#
# Some variables to set the level of debugging informations.
# Recommanded : -D__INFO
#-----------------------------------------------------------#
add_definitions (-D__INFO)
#add_definitions (-D__TRACE)
#add_definitions (-D__DEBUG_ALL)
add_definitions (-D__BENCHMARK)

#-----------------------------------------------------------#
# Set the location of CMake Modules
#-----------------------------------------------------------#
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
		"${DAMARIS_SOURCE_DIR}/cmake/Modules/")

#-----------------------------------------------------------#
include_directories(${DAMARIS_SOURCE_DIR})
include_directories(${DAMARIS_SOURCE_DIR}/include)

#-----------------------------------------------------------#
# Set everything related to the Boost libraries
#-----------------------------------------------------------#
set(BOOST_ROOT ${EXTERNAL_ROOT})
find_package(Boost REQUIRED)
set(BOOST_LIB boost_filesystem boost_system boost_program_options)
include_directories(${BOOST_INCLUDE_DIR})
link_directories(${BOOST_LIBRARY_DIR})

#-----------------------------------------------------------#
# Set everything related to the XSD libraries
#   (actually only include directory required)
#-----------------------------------------------------------#
set(XSD_ROOT ${EXTERNAL_ROOT})
set(XSD_INCLUDE_DIR "${XSD_ROOT}/include")
include_directories(${XSD_INCLUDE_DIR})

#-----------------------------------------------------------#
# Set everything related to the Xerces-C library
#-----------------------------------------------------------#
set(XERCESC_ROOT ${EXTERNAL_ROOT})
set(XERCESC_LIBRARY_DIR "${XERCESC_ROOT}/lib")
set(XERCESC_INCLUDE_DIR "${XERCESC_ROOT}/include")
set(XERCESC_LIB xerces-c)
include_directories(${XERCESC_INCLUDE_DIR})
link_directories(${XERCESC_LIBRARY_DIR})

#-----------------------------------------------------------#
# Set everything related to Python
#-----------------------------------------------------------#
set(PYTHON_ROOT ${EXTERNAL_ROOT})
set(NUMPY_INCLUDE_DIR ${EXTERNAL_ROOT}/lib/python2.6/site-packages/numpy/core/include)
find_package(PythonLibs)

if (PYTHON_INCLUDE_PATH)
  find_package(Boost 1.47 QUIET COMPONENTS python)
    if (Boost_PYTHON_LIBRARY)
	message(STATUS "Building Python support. Python lib: ${Boost_PYTHON_LIBRARY}")
	message(STATUS "Python include: ${PYTHON_INCLUDE_PATH}")
    include_directories(${PYTHON_INCLUDE_PATH})
	include_directories(${NUMPY_INCLUDE_DIR})
    link_directories(${PYTHON_LIBRARY_DIR})
    add_definitions(-D__ENABLE_PYTHON)
    # add the Boost Python library to the current Boost libraries
    set(BOOST_LIB ${BOOST_LIB} boost_python)
    set(PYTHON_LIB python2.6 util)
  else (Boost_PYTHON_LIBRARY)
    message(STATUS "Boost Python not found. Python support will not be built.")
  endif (Boost_PYTHON_LIBRARY)
else (PYTHON_INCLUDE_PATH)
  message(STATUS "Python headers not found. Python support will not be built.")
endif (PYTHON_INCLUDE_PATH)

#-----------------------------------------------------------#
# Set everything related to the HDF5 library 
# Just comment the first line if you don't want HDF5 support
#-----------------------------------------------------------#
#set(HDF5_ROOT ${EXTERNAL_ROOT})
set(HDF5_LIBRARY_DIR "${HDF5_ROOT}/lib")
set(HDF5_INCLUDE_DIR "${HDF5_ROOT}/include")
set(HDF5_LIB hdf5)
if(HDF5_ROOT)
  include_directories(${HDF5_INCLUDE_DIR})
  link_directories(${HDF5_LIBRARY_DIR})
  add_definitions(-D__ENABLE_HDF5)
endif(HDF5_ROOT)

#-----------------------------------------------------------#
# Set everything related to VisIt 
#-----------------------------------------------------------#
set(VISIT_ROOT "~/Work/Visu2/visit2.5.0/src")
if(VISIT_ROOT)
  set(VISIT_INCLUDE_DIR "${VISIT_ROOT}/sim/V2/lib")
  set(VISIT_LIBRARY_DIR "${VISIT_ROOT}/lib")
  include_directories(${VISIT_INCLUDE_DIR})
  link_directories(${VISIT_LIBRARY_DIR})
  add_definitions(-D__ENABLE_VISIT)
  set(VISIT_LIB simV2)
endif(VISIT_ROOT)
#-----------------------------------------------------------#
# Set everything related to Fortran
#-----------------------------------------------------------#
if(CMAKE_Fortran_COMPILER)
  include(FortranCInterface)
  FortranCInterface_HEADER(${DAMARIS_SOURCE_DIR}/core/FCMangle.h
			   MACRO_NAMESPACE "FC_FUNC_"
                           SYMBOL_NAMESPACE "FC_SYM_")
  add_definitions(-D__ENABLE_FORTRAN)
  message(STATUS "Fortran binding will be enabled, compiler is ${CMAKE_Fortran_COMPILER}")
else(CMAKE_Fortran_COMPILER)
  message(STATUS "Fortran binding will not be build")
endif(CMAKE_Fortran_COMPILER)

#-----------------------------------------------------------#
# Add the proper subdirectories
#-----------------------------------------------------------#
add_subdirectory (client)
add_subdirectory (server)
add_subdirectory (test)
add_subdirectory (examples)

if(VISIT_ROOT)
  add_subdirectory (visit/test)
endif(VISIT_ROOT)
