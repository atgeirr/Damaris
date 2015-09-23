cmake_minimum_required (VERSION 2.8)

#----------------------------------------------------------------------------#
# Enabling Fortran
#----------------------------------------------------------------------------#
enable_language (Fortran OPTIONAL)

#----------------------------------------------------------------------------#
# Force MPI compilers
#   starting from Damaris 0.4, MPI compiler is mandatory
#----------------------------------------------------------------------------#
include(CMakeForceCompiler)
if(NOT DEFINED PAR_CXX_COMPILER)
    CMAKE_FORCE_CXX_COMPILER(mpicxx "MPI C++ Compiler")
    CMAKE_FORCE_C_COMPILER(mpicc "MPI C Compiler")
else(NOT DEFINED PAR_CXX_COMPILER)
    CMAKE_FORCE_CXX_COMPILER(${PAR_CXX_COMPILER} "MPI C++ Compiler")
    message(STATUS "CXX compiler set to ${PAR_CXX_COMPILER}")
endif(NOT DEFINED PAR_CXX_COMPILER)
#CMAKE_FORCE_FORTRAN_COMPILER(mpif90 "Fortran Compiler")
include(CheckIncludeFiles)

#----------------------------------------------------------------------------#
# Starting project definition
#----------------------------------------------------------------------------#
project(DAMARIS)

#----------------------------------------------------------------------------#
# EXTERNAL_ROOT is a variable set once and for all to
# indicate where are all the required libraries. By default
# CMake will look for libraries in this directory.
#----------------------------------------------------------------------------#
if(NOT DEFINED EXTERNAL_ROOT)
    set(EXTERNAL_ROOT ~/local)
endif(NOT DEFINED EXTERNAL_ROOT)

if(NOT DEFINED XERCESC_ROOT)
    set(XERCESC_ROOT ${EXTERNAL_ROOT})
endif(NOT DEFINED XERCESC_ROOT)

if(NOT DEFINED XSD_ROOT)
    set(XSD_ROOT ${EXTERNAL_ROOT})
endif(NOT DEFINED XSD_ROOT)

if(NOT DEFINED BOOST_ROOT)
    set(BOOST_ROOT ${EXTERNAL_ROOT})
endif(NOT DEFINED BOOST_ROOT)

if(NOT DEFINED VisIt_ROOT)
    set(VisIt_ROOT ${EXTERNAL_ROOT}/visit) # optional
endif(NOT DEFINED VisIt_ROOT)

if(NOT DEFINED CppUnit_ROOT)
    set(CppUnit_ROOT ${EXTERNAL_ROOT}) # optional
endif(NOT DEFINED CppUnit_ROOT)
#set(PYTHON_ROOT ${EXTERNAL_ROOT}) # not supported anymore 

#----------------------------------------------------------------------------#
# Installation directory, /usr/local by default
#----------------------------------------------------------------------------#
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
	set(CMAKE_INSTALL_PREFIX /usr/local)
endif(NOT DEFINED CMAKE_INSTALL_PREFIX)
message(STATUS "Damaris will be installed in ${CMAKE_INSTALL_PREFIX}")

#----------------------------------------------------------------------------#
add_definitions (-g -O2 -Wno-unused-variable -Wno-unused-parameter -Wall -rdynamic)
add_definitions (-Wextra) # for development only
#----------------------------------------------------------------------------#

#----------------------------------------------------------------------------#
# Some variables to set the level of debugging informations.
# Recommanded : -D__INFO
#----------------------------------------------------------------------------#
add_definitions (-D__INFO)
#add_definitions (-D__TRACE)
#add_definitions (-D__DEBUG_ALL)
#add_definitions (-D__BENCHMARK)

#----------------------------------------------------------------------------#
# Check if we are on BlueGene/P
# uncomment the following line (set(BGP)) if you run on BGP.
#----------------------------------------------------------------------------#
#set(BGP "yes")
if(BGP)
	set(BGP_FLOOR "/bgsys/drivers/ppcfloor")
	include_directories(${BGP_FLOOR}/arch/include)
	add_definitions(-DBGP)
endif(BGP)

#----------------------------------------------------------------------------#
# Set the location of CMake Modules
#----------------------------------------------------------------------------#
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
		"${DAMARIS_SOURCE_DIR}/cmake/Modules/")

#----------------------------------------------------------------------------#
include_directories(${DAMARIS_SOURCE_DIR}/include)
include_directories(${DAMARIS_SOURCE_DIR}/include/damaris)

#----------------------------------------------------------------------------#
# Set everything related to the Boost libraries
#----------------------------------------------------------------------------#
set(Boost_USE_STATIC_LIBS ON)
SET(Boost_ADDITIONAL_VERSIONS "1.51.0" "1.52.0" 
			"1.53.0" "1.54.0" "1.55.0" "1.56.0" "1.57.0" "1.58.0"
			"1.60.0" "1.61.0" "1.61.0" "1.62.0" "1.63.0" "1.64.0")
find_package(Boost REQUIRED)
find_package(Boost 1.51.0 COMPONENTS filesystem system date_time)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND DEP_LIBRARIES ${Boost_LIBRARIES})

#----------------------------------------------------------------------------#
# Set everything related to the XSD libraries
#   (actually only include directory required)
#----------------------------------------------------------------------------#
if(EXISTS "${DAMARIS_SOURCE_DIR}/include/damaris/model/Model.hpp")
	set(MODEL_PRESENT 1)
endif()

if(NOT (MODEL_PRESENT EQUAL 1))
	message(STATUS "XSD model not generated, searching for XSD binary...")
	find_package(XSD REQUIRED)
else()
	message(STATUS "XSD model present, searching for XSD headers...")
	if(NOT DEFINED XSD_INCLUDE_DIR)
		set(XSD_INCLUDE_DIR ${XSD_ROOT}/include)
		message(STATUS "XSD include dir is ${XSD_INCLUDE_DIR}")
	endif()
endif()

if(NOT EXISTS "${XSD_INCLUDE_DIR}/xsd/cxx/config.hxx")
	message(FATAL_ERROR "XSD headers not found.")
endif()

include_directories(${XSD_INCLUDE_DIR})

#----------------------------------------------------------------------------#
# Set everything related to the Xerces-C library
#----------------------------------------------------------------------------#
find_package(XercesC REQUIRED)
include_directories(${XERCESC_INCLUDE})
list(APPEND DEP_LIBRARIES ${XERCESC_LIBRARY})

#----------------------------------------------------------------------------#
# Set everything related to Python
#----------------------------------------------------------------------------#
if(PYTHON_ROOT) # Python is disabled for now

find_package(PythonLibs)
if (PYTHONLIBS_FOUND)
	find_package(NumPy)
endif (PYTHONLIBS_FOUND)

if (NUMPY_FOUND) 
	find_package(Boost 1.51 QUIET COMPONENTS python)
	if(Boost_PYTHON_LIBRARY)
		message(STATUS 
			"Building Python support (${Boost_PYTHON_LIBRARY})")
		message(STATUS "Python include: ${PYTHON_INCLUDE_PATH}")
		message(STATUS "NumPy include: ${NUMPY_INCLUDE_DIRS}")
		include_directories(${PYTHON_INCLUDE_DIRS})
		include_directories(${NUMPY_INCLUDE_DIRS})
		add_definitions(-D__ENABLE_PYTHON)
		# add the Boost Python library to the current Boost libraries
		#    set(PYTHON_LIBRARIES python2.6 util)
		list(APPEND DEP_LIBRARIES ${Boost_PYTHON_LIBRARY} python2.6 util)
	else (Boost_PYTHON_LIBRARY)
		message(STATUS "Boost Python not found.")
	endif(Boost_PYTHON_LIBRARY)
else (NUMPY_FOUND)
	message(STATUS "Python headers not found.")
endif (NUMPY_FOUND)

endif(PYTHON_ROOT)

#----------------------------------------------------------------------------#
# Set everything related to VisIt 
#----------------------------------------------------------------------------#
find_package(VisIt)
if(VisIt_FOUND)
	include_directories(${VisIt_INCLUDE_DIRS})
	message(STATUS "VisIt found: ${VisIt_INCLUDE_DIRS}")
	list(APPEND DEP_LIBRARIES ${VisIt_LIBRARIES})
        set(HAVE_VISIT_ENABLED 1)
else(VisIt_FOUND)
    if(DEFINED VISIT_INCLUDE) # this is for when VisIt installs Damaris
    # the user is not supposed to set VISIT_INCLUDE himself; it is set by
    # the build_visit script
        message(STATUS "VisIt not found, but include dir forced to ${VISIT_INCLUDE}")
        include_directories(${VISIT_INCLUDE})
	set(HAVE_VISIT_ENABLED 1)
    else(DEFINED VISIT_INCLUDE)	
        message(STATUS "VisIt not found")
    endif(DEFINED VISIT_INCLUDE)
endif(VisIt_FOUND)

#----------------------------------------------------------------------------#
# Set everything related to CppUnit 
#----------------------------------------------------------------------------#
find_package(CppUnit)
if(CPPUNIT_FOUND)
	message(STATUS "CppUnit found, unit tests will be compiled")
	include_directories(${CPPUNIT_INCLUDE_DIR})
	enable_testing()
else(CPPUNIT_FOUND)
	message(STATUS "CppUnit not found, unit tests will not be compiled")
endif(CPPUNIT_FOUND)

#----------------------------------------------------------------------------#
# Set everything related to Fortran
#----------------------------------------------------------------------------#
if(CMAKE_Fortran_COMPILER)
	include(FortranCInterface)
	FortranCInterface_HEADER(${DAMARIS_SOURCE_DIR}/include/DamarisFCMangle.h
			MACRO_NAMESPACE "FC_FUNC_"
			SYMBOL_NAMESPACE "FC_SYM_")
	add_definitions(-D__ENABLE_FORTRAN)
	message(STATUS "Fortran binding enabled (${CMAKE_Fortran_COMPILER})")
else(CMAKE_Fortran_COMPILER)
	message(STATUS "Fortran binding will not be build")
endif(CMAKE_Fortran_COMPILER)


list(APPEND DEP_LIBRARIES "stdc++" "rt" "dl")


function(add_executable TARGET) 
	_ADD_EXECUTABLE(${TARGET} ${ARGN})
	set_target_properties(${TARGET} 
			PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")
endfunction() 

#----------------------------------------------------------------------------#
# Configure Damaris
#----------------------------------------------------------------------------#
CONFIGURE_FILE(${DAMARIS_SOURCE_DIR}/include/Damaris.h.in 
	       ${DAMARIS_SOURCE_DIR}/include/Damaris.h)

#----------------------------------------------------------------------------#
# Add the proper subdirectories
#----------------------------------------------------------------------------#
add_subdirectory (lib)

if(NOT DEFINED ENABLE_EXAMPLES)
   set(ENABLE_EXAMPLES true)
endif(NOT DEFINED ENABLE_EXAMPLES)
if(ENABLE_EXAMPLES)
        message(STATUS "Examples are enabled, will be built")
	add_subdirectory (examples)
endif(ENABLE_EXAMPLES)

if(CPPUNIT_FOUND AND ENABLE_TESTS)
        message(STATUS "Unit tests are enabled, will be built")
	add_subdirectory (test)
endif(CPPUNIT_FOUND AND ENABLE_TESTS)

#----------------------------------------------------------------------------#
# Installation process
#----------------------------------------------------------------------------#
install(FILES "include/Damaris.h" DESTINATION include)
install(FILES "${CMAKE_BINARY_DIR}/lib/libdamaris.a" DESTINATION lib)
install(DIRECTORY "include/damaris" DESTINATION include)
if(CMAKE_Fortran_COMPILE)
	install(FILES "lib/damaris.mod" DESTINATION include)
endif(CMAKE_Fortran_COMPILE)
