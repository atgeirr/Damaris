# load package helper for generating cmake CONFIG packages
include (CMakePackageConfigHelpers)

# where to install files for "find_package"
set(DAMARIS_PKG "share/cmake/damaris")

set(DAMARIS_SRC
	${DAMARIS_SOURCE_DIR}/src/main/Damaris.cpp
	${DAMARIS_SOURCE_DIR}/src/main/DamarisFortran.cpp
	${DAMARIS_SOURCE_DIR}/src/env/Environment.cpp
	${DAMARIS_SOURCE_DIR}/src/env/Hardware.cpp
	${DAMARIS_SOURCE_DIR}/src/msg/Signals.cpp
	${DAMARIS_SOURCE_DIR}/src/client/Client.cpp
	${DAMARIS_SOURCE_DIR}/src/client/StandaloneClient.cpp
	${DAMARIS_SOURCE_DIR}/src/client/RemoteClient.cpp
	${DAMARIS_SOURCE_DIR}/src/server/Server.cpp
	${DAMARIS_SOURCE_DIR}/src/action/ActionManager.cpp
	${DAMARIS_SOURCE_DIR}/src/action/Action.cpp
	${DAMARIS_SOURCE_DIR}/src/action/DynamicAction.cpp
	${DAMARIS_SOURCE_DIR}/src/data/Variable.cpp
	${DAMARIS_SOURCE_DIR}/src/data/VariableManager.cpp
	${DAMARIS_SOURCE_DIR}/src/data/LayoutManager.cpp
	${DAMARIS_SOURCE_DIR}/src/data/ParameterManager.cpp
	${DAMARIS_SOURCE_DIR}/src/data/Mesh.cpp
	${DAMARIS_SOURCE_DIR}/src/data/MeshManager.cpp
    ${DAMARIS_SOURCE_DIR}/src/data/StructuredMesh.cpp
	${DAMARIS_SOURCE_DIR}/src/data/RectilinearMesh.cpp
	${DAMARIS_SOURCE_DIR}/src/data/CurvilinearMesh.cpp
	${DAMARIS_SOURCE_DIR}/src/data/PointMesh.cpp
	${DAMARIS_SOURCE_DIR}/src/data/UnstructuredMesh.cpp
	${DAMARIS_SOURCE_DIR}/src/data/CurveManager.cpp
	${DAMARIS_SOURCE_DIR}/src/data/Curve.cpp
	${DAMARIS_SOURCE_DIR}/src/data/Block.cpp
	${DAMARIS_SOURCE_DIR}/src/data/Type.cpp
	${DAMARIS_SOURCE_DIR}/src/data/Parameter.cpp
	${DAMARIS_SOURCE_DIR}/src/data/BoxLayout.cpp
	${DAMARIS_SOURCE_DIR}/src/util/Observer.cpp
	${DAMARIS_SOURCE_DIR}/src/comm/Reactor.cpp
	${DAMARIS_SOURCE_DIR}/src/comm/MPI2Reactor.cpp
	${DAMARIS_SOURCE_DIR}/src/buffer/SharedMemorySegment.cpp
	${DAMARIS_SOURCE_DIR}/src/model/BcastXML.cpp
	${DAMARIS_SOURCE_DIR}/src/log/EventLogger.cpp
	${DAMARIS_SOURCE_DIR}/src/storage/StorageManager.cpp
	${DAMARIS_SOURCE_DIR}/src/storage/NullStore.cpp
	${CMAKE_BINARY_DIR}/src/model/Model.cpp
	)

# if Damaris is compiled with VisIt
if(ENABLE_VISIT)
  set(DAMARIS_VISIT_SRC ${DAMARIS_SOURCE_DIR}/src/visit/VisItListener.cpp)
endif(ENABLE_VISIT)

# if Damairs is compiled with HDF5
if(ENABLE_HDF5)
 set(DAMARIS_HDF5_SRC ${DAMARIS_SOURCE_DIR}/src/storage/HDF5Store.cpp)
endif(ENABLE_HDF5)

# if Damaris is compiled with ParaView
if(ENABLE_CATALYST)
  set(DAMARIS_PARAVIEW_SRC ${DAMARIS_SOURCE_DIR}/src/paraview/ParaViewAdaptor.cpp)
endif(ENABLE_CATALYST)

# if Damaris is compiled with Fortran support
if(ENABLE_FORTRAN)
  set(DAMARIS_FORTRAN_MODULE ${DAMARIS_SOURCE_DIR}/src/main/Damaris.F90)
endif(ENABLE_FORTRAN)

# add the Dependency on XSD-generated files
set_source_files_properties(
		${CMAKE_BINARY_DIR}/include/damaris/model/Model.hpp
		${CMAKE_BINARY_DIR}/src/model/Model.cpp
		PROPERTIES GENERATED 1)
# check if Model.cpp and Model.hpp are present in the xsd folder
if(EXISTS ${DAMARIS_SOURCE_DIR}/xsd/Model.hpp AND NOT ${GENERATE_MODEL})

  add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/src/model/Model.cpp
		  ${CMAKE_BINARY_DIR}/include/damaris/model/Model.hpp
		  COMMAND mkdir -p ${CMAKE_BINARY_DIR}/src/model
		  COMMAND mkdir -p ${CMAKE_BINARY_DIR}/include/damaris/model
		  COMMAND cp ${DAMARIS_SOURCE_DIR}/xsd/Model.hpp ${CMAKE_BINARY_DIR}/include/damaris/model
		  COMMAND cp ${DAMARIS_SOURCE_DIR}/xsd/Model.cpp ${CMAKE_BINARY_DIR}/src/model
		  COMMAND cp ${DAMARIS_SOURCE_DIR}/xsd/Model.hpp ${CMAKE_BINARY_DIR}/src/model
		  # COMMAND cp ${DAMARIS_SOURCE_DIR}/xsd/Model.hpp ${DAMARIS_SOURCE_DIR}/include/damaris/model
		  COMMENT "Copying existing C++/XML mapping")
else()

  set(XSD_ARGS 	--output-dir ${CMAKE_BINARY_DIR}/src/model
		--type-naming knr 
		--hxx-suffix .hpp 
		--cxx-suffix .cpp 
                --std       c++11
		${DAMARIS_SOURCE_DIR}/src/model/Model.xsd)
  add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/src/model/Model.cpp
		${CMAKE_BINARY_DIR}/include/damaris/model/Model.hpp
		COMMAND mkdir -p ${CMAKE_BINARY_DIR}/src/model
		COMMAND mkdir -p ${CMAKE_BINARY_DIR}/include/damaris/model
		COMMAND ${XSD_EXECUTABLE} ARGS "cxx-tree" ${XSD_ARGS}
		COMMAND cp ${CMAKE_BINARY_DIR}/src/model/Model.hpp ${CMAKE_BINARY_DIR}/include/damaris/model
		COMMAND cp ${CMAKE_BINARY_DIR}/src/model/Model.hpp ${DAMARIS_SOURCE_DIR}/include/damaris/model
		# Copy the new version of the model to xsd directory 
		# i'm not sure why it needs to be in two places though
		COMMAND cp ${CMAKE_BINARY_DIR}/src/model/Model.cpp ${DAMARIS_SOURCE_DIR}/xsd/Model.cpp
		COMMAND cp ${CMAKE_BINARY_DIR}/src/model/Model.hpp ${DAMARIS_SOURCE_DIR}/xsd/Model.hpp 
		DEPENDS ${DAMARIS_SOURCE_DIR}/src/model/Model.xsd
		COMMENT "Generating C++/XML mapping")
endif()

add_library(damaris 	${DAMARIS_SRC}
			${DAMARIS_VISIT_SRC}
			${DAMARIS_HDF5_SRC} 
			${DAMARIS_PARAVIEW_SRC}
			${DAMARIS_FORTRAN_MODULE})

include_directories(${CMAKE_BINARY_DIR}/include/damaris
		${CMAKE_BINARY_DIR}/include)

target_include_directories(damaris PUBLIC $<INSTALL_INTERFACE:include>)
# local include's BEFORE, in case old incompatable .h files in prefix/include
target_include_directories(damaris BEFORE PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
target_include_directories(damaris PUBLIC ${Boost_INCLUDE_DIRS}
		${XSD_INCLUDE_DIR} ${XercesC_INCLUDE_DIRS})
if(ENABLE_VISIT)
	target_include_directories(damaris PUBLIC ${VisIt_INCLUDE_DIRS})
endif(ENABLE_VISIT)
if(ENABLE_HDF5)
	target_include_directories(damaris PUBLIC ${HDF5_INCLUDE_DIRS})
endif(ENABLE_HDF5)
if(ENABLE_CATALYST)
	target_include_directories(damaris PUBLIC ${PARAVIEW_INCLUDE_DIRS})
	target_include_directories(damaris PUBLIC ${ParaView_INCLUDE_DIRS})
endif(ENABLE_CATALYST)

# for shared libs, establish the lib version
set_target_properties(damaris
		PROPERTIES VERSION ${DAMARIS_VERSION}
		SOVERSION ${DAMARIS_VERSION_MAJOR})

target_compile_options(damaris PRIVATE -fPIC)
target_link_libraries(damaris PUBLIC ${DEP_LIBRARIES})

#----------------------------------------------------------------------------#
# installation stuff 
#----------------------------------------------------------------------------#
write_basic_package_version_file(
		"DamarisConfigVersion.cmake"
		VERSION ${DAMARIS_VERSION}
		COMPATIBILITY AnyNewerVersion)
configure_package_config_file(DamarisConfig.cmake.in DamarisConfig.cmake
		INSTALL_DESTINATION ${DAMARIS_PKG})

#----------------------------------------------------------------------------#
# generate pkg-config file - 
# TODO: Fix This so that Paraview/Catalyst and Boost libbs are included correctly
#----------------------------------------------------------------------------#
set(INCS_LIST_FOR_PKGCONFIG "")
# processing include dirs
# mandatory dependencies (XSD, XercesC, Boost)
# XercesC is not needed because it has its own .pc file
foreach(p ${XSD_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
  list(APPEND INCS_LIST_FOR_PKGCONFIG ${p})
endforeach(p)
# optional: HDF5
if(ENABLE_HDF5)
  foreach(p ${HDF5_INCLUDE_DIRS})
    list(APPEND INCS_LIST_FOR_PKGCONFIG ${p})
  endforeach(p)
endif(ENABLE_HDF5)
# optional: VisIt
if(ENABLE_VISIT)
  foreach(p ${VisIt_INCLUDE_DIRS})
    list(APPEND INCS_LIST_FOR_PKGCONFIG ${p})
  endforeach(p)
endif(ENABLE_VISIT)
# optional: Catalyst
if(ENABLE_CATALYST)
  foreach(p ${ParaView_INCLUDE_DIRS})
    list(APPEND INCS_LIST_FOR_PKGCONFIG ${p})
  endforeach(p)
 foreach(p ${VTK_INCLUDE_DIRS})
    list(APPEND INCS_LIST_FOR_PKGCONFIG ${p})
  endforeach(p)
endif(ENABLE_CATALYST)
# remove duplicates from the list
list(REMOVE_DUPLICATES INCS_LIST_FOR_PKGCONFIG)
# convert the list into a string with "-I" added in front of every path
set(PKG_CONFIG_INC_FLAGS "")
foreach(p ${INCS_LIST_FOR_PKGCONFIG})
  string(CONCAT PKG_CONFIG_INC_FLAGS ${PKG_CONFIG_INC_FLAGS} "-I${p} ")
endforeach(p)
# ---
# processing library dirs and names
set(LIBS_LIST_FOR_PKGCONFIG "")
set(LIBS_DIR_LIST_FOR_PKG_CONFIG "")
# get Boost libraries
foreach(p ${Boost_LIBRARIES})
  get_filename_component(libname ${p} NAME_WE)
  get_filename_component(libpath ${p} DIRECTORY)
  list(APPEND LIBS_LIST_FOR_PKGCONFIG ${libname})
  list(APPEND LIBS_DIR_LIST_FOR_PKGCONFIG ${libpath})
endforeach(p)
# optional: HDF5
if(ENABLE_HDF5)
  foreach(p ${HDF5_LIBRARIES})
    get_filename_component(libname ${p} NAME_WE)
    get_filename_component(libpath ${p} DIRECTORY)
    list(APPEND LIBS_LIST_FOR_PKGCONFIG ${libname})
    list(APPEND LIBS_DIR_LIST_FOR_PKGCONFIG ${libpath})
  endforeach(p)
endif(ENABLE_HDF5)
# optional: VisIt
if(ENABLE_VISIT)
  foreach(p ${VisIt_LIBRARIES})
    get_filename_component(libname ${p} NAME_WE)
    get_filename_component(libpath ${p} DIRECTORY)
    list(APPEND LIBS_LIST_FOR_PKGCONFIG ${libname})
    list(APPEND LIBS_DIR_LIST_FOR_PKGCONFIG ${libpath})
  endforeach(p)
endif(ENABLE_VISIT)

# optional: Catalyst
if(ENABLE_CATALYST)
  # target_link_libraries(damaris ParaView::PythonCatalyst VTK::mpi)  
  if(ParaView_VERSION VERSION_LESS "5.7")
    set(PV_CFG_ARGS --libs vtkPVPythonCatalyst vtkParallelMPI vtkUtilitiesPythonInitializer)
    add_custom_command(OUTPUT pv-config.out COMMAND $<TARGET_FILE:paraview-config> ${PV_CFG_ARGS} > pv-config.out)
    add_custom_target(damaris.pc COMMAND ${DAMARIS_SOURCE_DIR}/lib/make-pc-file.sh DEPENDS pv-config.out)
    message(STATUS "ParaView < 5.7 setting pkg-config libraries") 
  else()
   # set(PV_CFG_ARGS --libs ParaView::PythonCatalyst VTK::mpi)
    # add_custom_command(OUTPUT pv-config.out COMMAND $<TARGET_FILE:paraview-config> ${PV_CFG_ARGS} > pv-config.out)
    # add_custom_target(damaris.pc COMMAND ${DAMARIS_SOURCE_DIR}/lib/make-pc-file.sh DEPENDS pv-config.out)
    set(PV_LIBS ${ParaView_LIBRARIES})
    set(VTK_LIBS ${VTK_LIBRARIES})
    message(STATUS "ParaView >= 5.7 setting pkg-config libraries") 
  endif()

else(ENABLE_CATALYST)
  # add_custom_target(damaris.pc COMMAND sed -e s/PV_LIBS//g damaris.pc.in > damaris.pc)
  set(PV_LIBS ${ParaView_LIBS})
endif(ENABLE_CATALYST)  

# remove dublicates
list(REMOVE_DUPLICATES LIBS_LIST_FOR_PKGCONFIG)
list(REMOVE_DUPLICATES LIBS_DIR_LIST_FOR_PKGCONFIG)
# build a string for the list of directories (-L)
set(PKG_CONFIG_LIBS_FLAGS "")
foreach(p ${LIBS_DIR_LIST_FOR_PKGCONFIG})
  string(CONCAT PKG_CONFIG_LIBS_FLAGS ${PKG_CONFIG_LIBS_FLAGS} "-L${p} ")
endforeach(p)
# build a string for the list of libraries (-l)
foreach(p ${LIBS_LIST_FOR_PKGCONFIG})
  string(REGEX REPLACE "^lib" "" p_without_lib ${p})
  string(CONCAT PKG_CONFIG_LIBS_FLAGS ${PKG_CONFIG_LIBS_FLAGS} "-l${p_without_lib} ")
endforeach(p)
string(CONCAT PKG_CONFIG_LIBS_FLAGS ${PKG_CONFIG_LIBS_FLAGS} " -lstdc++ -lrt -ldl -lpthread")


if(ENABLE_CATALYST)
	####################################################
	# This does not really help as the format of ${VTK_LIBRARIES}  ${BOOST_LIBRARIES} and  ${ParaView_LIBRARIES}
	# is VTK::LIBNAME or BOOST::LIBNAME etc. 
	# build a string for the list of VTK libraries (-l)
#	string(REGEX REPLACE ";" " " VTK_LIBS ${VTK_LIBS})
#	set(PV_CONFIG_LIBS_FLAGS "")
#	foreach(p ${VTK_LIBS})
#	string(REGEX REPLACE "VTK::" " -lVTK::" p_with_l ${p})
	#  string(REGEX REPLACE ";" " " p_with_space ${p_with_l})
#	string(CONCAT PV_CONFIG_LIBS_FLAGS ${PV_CONFIG_LIBS_FLAGS} "${p_with_l} ")
#	endforeach(p)
#	set(VTK_LIBS ${PV_CONFIG_LIBS_FLAGS})

	# build a string for the list of PARAVIEW libraries (-l)
#	string(REGEX REPLACE ";" " " PV_LIBS ${PV_LIBS})
#	set(PV_CONFIG_LIBS_FLAGS "")
#	foreach(p ${PV_LIBS})
#	string(REGEX REPLACE "ParaView::" " -lParaView::" p_with_l ${p})
	#  string(REGEX REPLACE ";" " " p_with_space ${p_with_l})
#	string(CONCAT PV_CONFIG_LIBS_FLAGS ${PV_CONFIG_LIBS_FLAGS} "${p_with_l} ")
#	endforeach(p)
#	set(PV_LIBS ${PV_CONFIG_LIBS_FLAGS})
endif()
####################################################
# ---
# configure the .pc.in file
configure_file(damaris.pc.in damaris.pc @ONLY)
# add_dependencies(damaris damaris.pc)

#----------------------------------------------------------------------------#
# Installation process
#----------------------------------------------------------------------------#
install(TARGETS damaris EXPORT DamarisTargets
		ARCHIVE DESTINATION lib
		LIBRARY DESTINATION lib)
install(EXPORT DamarisTargets
		DESTINATION ${DAMARIS_PKG}
		FILE "DamarisTargets.cmake")
install(FILES   "${CMAKE_CURRENT_BINARY_DIR}/DamarisConfigVersion.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/DamarisConfig.cmake"
	        "${DAMARIS_SOURCE_DIR}/cmake/Modules/FindVisIt.cmake"
		"${DAMARIS_SOURCE_DIR}/cmake/Modules/FindXSD.cmake"
		DESTINATION ${DAMARIS_PKG} )
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/damaris.pc"
	DESTINATION lib/pkgconfig)
install(FILES "${CMAKE_BINARY_DIR}/include/Damaris.h" DESTINATION include)
install(FILES "${CMAKE_BINARY_DIR}/include/damaris/model/Model.hpp" DESTINATION include/damaris/model)
install(DIRECTORY "${DAMARIS_SOURCE_DIR}/include/damaris" DESTINATION include)

if(CMAKE_Fortran_COMPILER)
  install(FILES "${CMAKE_BINARY_DIR}/lib/damaris.mod" DESTINATION include)
endif(CMAKE_Fortran_COMPILER)

install(DIRECTORY "${DAMARIS_SOURCE_DIR}/examples" DESTINATION  ${CMAKE_INSTALL_PREFIX} )
# install(FILES "${CMAKE_BINARY_DIR}/examples/Makefile" DESTINATION examples)

if(ENABLE_EXAMPLES)
	if(ENABLE_CATALYST)
	  install(PROGRAMS "${CMAKE_BINARY_DIR}/examples/paraview/image" DESTINATION examples/paraview)
	  install(PROGRAMS "${CMAKE_BINARY_DIR}/examples/paraview/lollipop" DESTINATION examples/paraview)
	  install(PROGRAMS "${CMAKE_BINARY_DIR}/examples/paraview/plate" DESTINATION examples/paraview)
	  install(PROGRAMS "${CMAKE_BINARY_DIR}/examples/paraview/sector" DESTINATION examples/paraview)
	endif(ENABLE_CATALYST) 
	if(ENABLE_HDF5)
	  install(PROGRAMS "${CMAKE_BINARY_DIR}/examples/storage/2dmesh" DESTINATION examples/storage)
	  install(PROGRAMS "${CMAKE_BINARY_DIR}/examples/storage/3dmesh" DESTINATION examples/storage)
	  install(PROGRAMS "${CMAKE_BINARY_DIR}/examples/storage/blocks-hdf5" DESTINATION examples/storage)
	  install(PROGRAMS "${CMAKE_BINARY_DIR}/examples/storage/mandelbulb-hdf5" DESTINATION examples/storage)
	  install(PROGRAMS "${CMAKE_BINARY_DIR}/examples/storage/vector" DESTINATION examples/storage)
	endif(ENABLE_HDF5)
	if(ENABLE_VISIT)
	  install(PROGRAMS "${CMAKE_BINARY_DIR}/examples/visit/blocks" DESTINATION examples/visit)
	  install(PROGRAMS "${DAMARIS_SOURCE_DIR}/examples/visit/cmd.py" DESTINATION examples/visit)
	  install(PROGRAMS "${CMAKE_BINARY_DIR}/examples/visit/curve" DESTINATION examples/visit)
	  install(PROGRAMS "${CMAKE_BINARY_DIR}/examples/visit/mandelbulb" DESTINATION examples/visit)
	  install(PROGRAMS "${CMAKE_BINARY_DIR}/examples/visit/mesh" DESTINATION examples/visit)
	  install(PROGRAMS "${CMAKE_BINARY_DIR}/examples/visit/point" DESTINATION examples/visit)
	  install(PROGRAMS "${CMAKE_BINARY_DIR}/examples/visit/var" DESTINATION examples/visit)
	endif(ENABLE_VISIT)
	if(ENABLE_FORTRAN)
	  install(PROGRAMS "${CMAKE_BINARY_DIR}/examples/fortran/cube" DESTINATION examples/fortran)
	endif(ENABLE_FORTRAN)
	install(FILES "${CMAKE_BINARY_DIR}/examples/plugin/libgc.so" DESTINATION examples/plugin)
endif(ENABLE_EXAMPLES)

