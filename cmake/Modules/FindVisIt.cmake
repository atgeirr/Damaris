# - Tries to find VisIt
# You may define VisIt_ROOT to hint at the install location. 
# Once done this will define
#  VisIt_FOUND - VisIt is installed
#  VisIt_INCLUDE_DIRS - Directories where VisIt headers can be found
#  VisIt_LIBRARIES - Libraries required by VisIt headers
include(FindPackageHandleStandardArgs)

set(VisIt_VERSIONS 2.13.2 2.13.1 2.13.0 2.12.3 2.12.2 2.12.1 2.12.0 2.11.0 2.10.3 2.10.2 2.10.1 2.10.0 2.9.2 2.9.1 2.9.0 2.8.2 2.8.1 2.8.0)
set(VisIt_ARCHITECTURES linux-x86_64)
set(VisIt_CANDIDATES /opt/visit /opt/visit/visit ${VisIt_ROOT})
set(VisIt_SUFFIXES)
foreach(version ${VisIt_VERSIONS})
  foreach(arch ${VisIt_ARCHITECTURES})
    list(APPEND VisIt_SUFFIXES ${version}/${arch}/libsim/V2)
  endforeach()
endforeach()

set(VisIt_INC_SUFFIXES)
set(VisIt_LIB_SUFFIXES)
foreach(suffix ${VisIt_SUFFIXES})
  list(APPEND VisIt_INC_SUFFIXES ${suffix}/include)
  list(APPEND VisIt_LIB_SUFFIXES ${suffix}/lib)
endforeach()
list(APPEND VisIt_INC_SUFFIXES sim/V2/lib)
list(APPEND VisIt_LIB_SUFFIXES lib)

find_path(VisIt_INCLUDE_DIRS VisItControlInterface_V2.h HINTS 
		${VisIt_CANDIDATES} PATH_SUFFIXES ${VisIt_INC_SUFFIXES} 
		ENV CPLUS_INCLUDE_PATH)
find_library(VisIt_LIBRARIES simV2  HINTS 
		${VisIt_CANDIDATES} PATH_SUFFIXES ${VisIt_LIB_SUFFIXES} 
		ENV LIBRARY_PATH LD_LIBRARY_PATH)

find_package_handle_standard_args(VisIt DEFAULT_MSG VisIt_INCLUDE_DIRS 
		VisIt_LIBRARIES)
mark_as_advanced(VisIt_INCLUDE_DIRS VisIt_LIBRARIES)
if(VisIt_INCLUDE_DIRS)
	set(VisIt_FOUND TRUE)
endif(VisIt_INCLUDE_DIRS)
