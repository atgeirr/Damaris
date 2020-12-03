cmake_minimum_required (VERSION 3.12) # Required because of policy CMP0074

if(${CMAKE_VERSION} VERSION_EQUAL "3.18") 
    message("WARNING: CMake v3.18 is known to have issues finding OpenMPI libraries")
    message("WARNING: CMake v3.14.1 was found to work fine.")
endif()
#----------------------------------------------------------------------------#
# Starting project definition
#----------------------------------------------------------------------------#
project(DAMARIS VERSION 1.3.2)

#
# library version set here (e.g. for shared libs).
#
# set(DAMARIS_VERSION_MAJOR 1)
# set(DAMARIS_VERSION_MINOR 3)
# set(DAMARIS_VERSION_PATCH 2)
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
option(GENERATE_MODEL  "Regenerates the XSD model instead of using existing ones." OFF)

#----------------------------------------------------------------------------#
# Setting the C++ compiler options
#----------------------------------------------------------------------------#
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
include_directories(${DAMARIS_SOURCE_DIR}/include/damaris)
include_directories(${CMAKE_BINARY_DIR}/include)

#----------------------------------------------------------------------------#
# Set everything related to the Boost libraries
#----------------------------------------------------------------------------#
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
find_package(Threads REQUIRED)
find_package(Boost 1.65.0 REQUIRED COMPONENTS thread log log_setup filesystem system date_time)
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
    set(PARAVIEW_LIBRARIES "vtkUtilitiesPythonInitializer" "vtkPVPythonCatalyst" "vtkParallelMPI")
  else()
    message(STATUS "ParaView >= 5.7 found")
    set(HAVE_PARAVIEW_ENABLED 1)
    set(PARAVIEW_LIBRARIES "vtkUtilitiesPythonInitializer" "vtkPVPythonCatalyst" "vtkParallelMPI")
    # set(PARAVIEW_LIBRARIES "ParaView::PythonCatalyst" "VTK::mpi")
    # message(STATUS "ParaView_LIBRARIES: ${ParaView_LIBRARIES}")
    # message(STATUS "VTK_LIBRARIES: ${VTK_LIBRARIES}")
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

if(ENABLE_EXAMPLES)
  add_subdirectory (examples)
endif(ENABLE_EXAMPLES)

if(ENABLE_TESTS)
  add_subdirectory (test)
endif(ENABLE_TESTS)
