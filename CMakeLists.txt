cmake_minimum_required (VERSION 3.12) # Required because of policy CMP0074

#----------------------------------------------------------------------------#
# Starting project definition
#----------------------------------------------------------------------------#

project(DAMARIS VERSION 1.7.1)


#
# library version set here (e.g. for shared libs).
#
# set(DAMARIS_VERSION_MAJOR 1)
# set(DAMARIS_VERSION_MINOR 6)
# set(DAMARIS_VERSION_PATCH 0)

# set(DAMARIS_VERSION "${DAMARIS_VERSION_MAJOR}.${DAMARIS_VERSION_MINOR}.${DAMARIS_VERSION_PATCH}")


#----------------------------------------------------------------------------#
# Set the location of CMake Modules
#----------------------------------------------------------------------------#
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
        "${DAMARIS_SOURCE_DIR}/cmake/Modules/")


#----------------------------------------------------------------------------#
# Setup RPATH
#----------------------------------------------------------------------------#
set (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#----------------------------------------------------------------------------#
# Options thet user may set 
#----------------------------------------------------------------------------#
option(ENABLE_TESTS    "Build tests. May require CppUnit_ROOT" OFF)
option(ENABLE_EXAMPLES "Build examples." OFF)
option(ENABLE_FORTRAN  "Build Fortran support. Requires CMAKE_Fortran_COMPILER to be set to an MPI-enabled compiler." OFF)
option(ENABLE_VISIT    "Build VisIt support. May requires VisIt_ROOT to be set." OFF)
option(ENABLE_CATALYST "Build Catalyst support. May requires ParaView_DIR to be set." OFF)
option(ENABLE_HDF5     "Build with HDF5 support. May requires HDF5_ROOT to be set." OFF)
option(ENABLE_PYTHON   "Build with Python+ NumPy scripting capabilties." OFF)
option(ENABLE_PYTHONMOD"Build external Python module to import Damaris communicator into mpi4py." OFF)
option(ENABLE_DOCS     "Configure DOxygen documentation build feature." OFF)
option(GENERATE_MODEL  "Regenerates the XSD model instead of using existing ones." OFF)

#----------------------------------------------------------------------------#
# Setting the C++ compiler options
#----------------------------------------------------------------------------#
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#----------------------------------------------------------------------------#
# MPI is currently mandatory
#----------------------------------------------------------------------------#
find_package(MPI)

#----------------------------------------------------------------------------#
# Enabling Fortran
#----------------------------------------------------------------------------#
if(ENABLE_FORTRAN)
  include(CheckLanguage)
  check_language(Fortran)
  if(CMAKE_Fortran_COMPILER)
    enable_language(Fortran)
    include(FortranCInterface)
    FortranCInterface_HEADER(${DAMARIS_SOURCE_DIR}/include/DamarisFCMangle.h
            MACRO_NAMESPACE "FC_FUNC_"
            SYMBOL_NAMESPACE "FC_SYM_")
    add_definitions(-D__ENABLE_FORTRAN)
  else(CMAKE_Fortran_COMPILER)
    message(FATAL "Fortran support requested but no Fortran compiler was found.")
  endif(CMAKE_Fortran_COMPILER)
endif(ENABLE_FORTRAN)

#----------------------------------------------------------------------------#
set(CMAKE_CXX_FLAGS "-Wno-unused-variable -Wno-unused-parameter -Wall -rdynamic")
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra") # for development only

#----------------------------------------------------------------------------#
# Some variables to set the level of debugging informations.
# Recommanded : -D__INFO
#----------------------------------------------------------------------------#
add_definitions (-D__INFO)
#add_definitions (-D__TRACE)
#add_definitions (-D__DEBUG_ALL)
#add_definitions (-D__BENCHMARK)

#----------------------------------------------------------------------------#
include_directories(${DAMARIS_SOURCE_DIR}/include)
include_directories(${CMAKE_BINARY_DIR}/include/damaris)
include_directories(${DAMARIS_SOURCE_DIR}/include/damaris)
include_directories(${CMAKE_BINARY_DIR}/include)

#----------------------------------------------------------------------------#
# Set everything related to the Boost libraries (except boost::python and boost::numpy)
#----------------------------------------------------------------------------#
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
find_package(Threads REQUIRED)
find_package(Boost 1.65.0 REQUIRED COMPONENTS thread log log_setup filesystem system date_time )
message(WARNING "WARNING Boost BOOST_BIND_GLOBAL_PLACEHOLDERS defined to remove warnings - perhaps check for issue?")
add_definitions("-DBOOST_BIND_GLOBAL_PLACEHOLDERS")
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND DEP_LIBRARIES Boost::thread Boost::log Boost::log_setup Boost::filesystem Boost::date_time)

#----------------------------------------------------------------------------#
# Set everything related to the XSD libraries
#----------------------------------------------------------------------------#
find_package(XSD REQUIRED)
include_directories(${XSD_INCLUDE_DIR})

#----------------------------------------------------------------------------#
# Set everything related to the Xerces-C library
# Note: even though cmake provides a FindXercesC.cmake, we provide our own
# because the new one doesn't seem to have the possibility to set XercesC_ROOT.
#----------------------------------------------------------------------------#
find_package(XercesC REQUIRED)
include_directories(${XercesC_INCLUDE_DIRS})
list(APPEND DEP_LIBRARIES XercesC::XercesC)

#----------------------------------------------------------------------------#
# Set everything related to VisIt 
#----------------------------------------------------------------------------#
if(ENABLE_VISIT)
  if(DEFINED VISIT_INCLUDE) # this is for when VisIt installs Damaris
    # the user is not supposed to set VISIT_INCLUDE himself; it is set by
    # the build_visit script
    message(STATUS "VisIt not found, but include dir forced to ${VISIT_INCLUDE}")
    include_directories(${VISIT_INCLUDE})
    set(HAVE_VISIT_ENABLED 1)
  else(DEFINED VISIT_INCLUDE) 
    find_package(VisIt REQUIRED)
    include_directories(${VisIt_INCLUDE_DIRS})
    list(APPEND DEP_LIBRARIES ${VisIt_LIBRARIES})
    set(HAVE_VISIT_ENABLED 1)
  endif(DEFINED VISIT_INCLUDE)
endif(ENABLE_VISIT)

#----------------------------------------------------------------------------#
# Set everything related to HDF5
#----------------------------------------------------------------------------#
if(ENABLE_HDF5)
  find_package(HDF5 REQUIRED COMPONENTS C)
  set(HAVE_HDF5_ENABLED 1)
  list(APPEND DEP_LIBRARIES ${HDF5_LIBRARIES})
  include_directories(${HDF5_INCLUDE_DIRS})
endif(ENABLE_HDF5)

#----------------------------------------------------------------------------#
# Set everything related to Python
#----------------------------------------------------------------------------#

if(ENABLE_PYTHON)
  # set(BOOST_ROOT /g100_scratch/userinternal/amemmol1/modules/easybuild/ebinstallation/software/Boost.Python/1.67.0-foss-2020a-Python-3.8.2)
  find_package(Boost COMPONENTS python REQUIRED)
  find_package(Boost COMPONENTS numpy REQUIRED)
  # find_package(Python 3 COMPONENTS Interpreter Development REQUIRED)  # fails at build time with fatal error: pyconfig.h: No such file or directory
  # find_package(PythonInterp 3 )
  # find_package(PythonLibs 3 REQUIRED )
  find_package(Python 3 COMPONENTS  Interpreter Development)
  if(Python_FOUND)
    # message("PythonInterp_FOUND:${PythonInterp_FOUND}")
    message("Python_FOUND:${Python_FOUND}")
    execute_process(
      COMMAND Python3::Interpreter -c "import numpy; print(numpy.get_include())"
      OUTPUT_VARIABLE NUMPY_INCLUDE_PATH
      ERROR_QUIET
    )
    list(APPEND DEP_LIBRARIES ${Boost_LIBRARIES}  ${PYTHON_LIBRARIES} ${Python_LIBRARIES} ${Boost_LIBRARIES} )
    include_directories(${Python_INCLUDE_DIRS})
     set(HAVE_PYTHON_ENABLED 1)
  endif(Python_FOUND)
  
  
endif(ENABLE_PYTHON)

#----------------------------------------------------------------------------#
# Test that the pydamaris Python module has an install path
#----------------------------------------------------------------------------#
if(ENABLE_PYTHONMOD)
  if ( "${PYTHON_MODULE_INSTALL_PATH}" STREQUAL "" )
     message(
                FATAL_ERROR 
                "ENABLE_PYTHONMOD is TRUE. Please set -DPYTHON_MODULE_INSTALL_PATH=<path> to the path to install the damaris4py Python module"
            )
  endif()
endif(ENABLE_PYTHONMOD)

#----------------------------------------------------------------------------#
# Set everything related to ParaView
#----------------------------------------------------------------------------#
if(ENABLE_CATALYST)
  # find_package(ParaView 4.2 REQUIRED COMPONENTS vtkPVPythonCatalyst vtkParallelMPI vtkUtilitiesPythonInitializer)  
  find_package(ParaView 4.2 REQUIRED)
  if(ParaView_VERSION VERSION_LESS "5.7")
    message(STATUS "ParaView < 5.7 found")
    find_package(ParaView 4.2 REQUIRED COMPONENTS vtkPVPythonCatalyst vtkParallelMPI vtkUtilitiesPythonInitializer) 
    include("${PARAVIEW_USE_FILE}")
    set(HAVE_PARAVIEW_ENABLED 1)
    set(PARAVIEW_VERSION ${ParaView_VERSION})
    set(PARAVIEW_LIBRARIES "vtkUtilitiesPythonInitializer" "vtkPVPythonCatalyst" "vtkParallelMPI")
    message(STATUS "ParaView_LIBRARIES: ${ParaView_LIBRARIES}")
    message(STATUS "VTK_LIBRARIES: ${VTK_LIBRARIES}")
  else()
    message(STATUS "ParaView >= 5.7 found")
    set(HAVE_PARAVIEW_ENABLED 1)
    set(PARAVIEW_VERSION ${ParaView_VERSION})
    find_package(ParaView 4.2  COMPONENTS Catalyst PythonCatalyst ) 
    set(PARAVIEW_LIBRARIES ParaView::Catalyst ParaView::PythonCatalyst VTK::mpi)
    message(STATUS "ParaView_LIBRARIES: ${ParaView_LIBRARIES}")
    # -DParaView_DIR=${INSTALL_PREFIX}/lib/cmake/paraview-5.8
    get_filename_component(PARENT_DIR ${ParaView_DIR} DIRECTORY)
    get_filename_component(PV_VERSION_DIR ${ParaView_DIR} NAME)
    set(PARAVIEW_INCLUDE_DIRS "${PARENT_DIR}/../../include/${PV_VERSION_DIR}")
    # include_directories(${ParaView_INCLUDE_DIRS})
    # include_directories(${VTK_INCLUDE_DIRS})
    message(STATUS "PARAVIEW_INCLUDE_DIRS: ${PARAVIEW_INCLUDE_DIRS}")
    message(STATUS "ParaView_INCLUDE_DIRS: ${ParaView_INCLUDE_DIRS}")
    message(STATUS "VTK_INCLUDE_DIRS: ${VTK_INCLUDE_DIRS}")
  endif()
  list(APPEND DEP_LIBRARIES ${PARAVIEW_LIBRARIES})
  add_definitions("-DUSE_CATALYST")
endif(ENABLE_CATALYST)

#----------------------------------------------------------------------------#
# Set everything related to CppUnit 
#----------------------------------------------------------------------------#
if(ENABLE_TESTS)
  find_package(CppUnit)
  include_directories(${CPPUNIT_INCLUDE_DIR})
  include_directories(${ParaView_INCLUDE_DIRS})
  enable_testing()
endif(ENABLE_TESTS)

#----------------------------------------------------------------------------#
# Other dependencies required
#----------------------------------------------------------------------------#
list(APPEND DEP_LIBRARIES "stdc++" "rt" "dl" "pthread" "m")

function(add_executable TARGET) 
  _ADD_EXECUTABLE(${TARGET} ${ARGN})
  set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")
endfunction() 

#----------------------------------------------------------------------------#
# Configure Damaris
#----------------------------------------------------------------------------#
CONFIGURE_FILE(${DAMARIS_SOURCE_DIR}/include/Damaris.h.in 
           ${CMAKE_BINARY_DIR}/include/Damaris.h)
           

#----------------------------------------------------------------------------#
# Add the proper subdirectories
#----------------------------------------------------------------------------#
add_subdirectory (lib)

if(ENABLE_PYTHONMOD)
add_subdirectory (python/damaris4py/damaris4py)
endif(ENABLE_PYTHONMOD)

if(ENABLE_EXAMPLES)
  add_subdirectory (examples)
endif(ENABLE_EXAMPLES)

if(ENABLE_TESTS)
  add_subdirectory (test)
endif(ENABLE_TESTS)

if(ENABLE_DOCS)
  add_subdirectory (docs)
endif(ENABLE_DOCS)

