cmake_minimum_required (VERSION 2.8)
project (DAMARIS)

enable_language (Fortran)

set(EXTERNAL_ROOT ~/deploy)

add_definitions (-O2 -Wall -D__INFO)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${DAMARIS_SOURCE_DIR}/cmake/Modules/")

set(BOOST_ROOT ${EXTERNAL_ROOT})

set(HDF5_INCLUDE_DIR "${EXTERNAL_ROOT}/include")
set(HDF5_LIBRARY_DIR "${EXTERNAL_ROOT}/lib")

set(ZLIB_LIBRARY_DIR "${EXTERNAL_ROOT}/lib")
set(ZLIB_INCLUDE_DIR "${EXTERNAL_ROOT}/include")

find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIR})
include_directories(${HDF5_INCLUDE_DIR})
include_directories(${ZLIB_INCLUDE_DIR})
include_directories(${DAMARIS_SOURCE_DIR})

link_directories(${HDF5_LIBRARY_DIR})
link_directories(${ZLIB_LIBRARY_DIR})

add_subdirectory (client)
add_subdirectory (server)
add_subdirectory (test)
