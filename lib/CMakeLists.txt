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
		${CMAKE_BINARY_DIR}/src/model/Model.cpp
		PROPERTIES GENERATED 1)
set(XSD_ARGS 	--output-dir ${CMAKE_BINARY_DIR}/src/model
		--type-naming knr 
		--hxx-suffix .hpp 
		--cxx-suffix .cpp 
                --std       c++11
		${DAMARIS_SOURCE_DIR}/src/model/Model.xsd)
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/src/model/Model.cpp
		COMMAND mkdir -p ${CMAKE_BINARY_DIR}/src/model
		COMMAND ${XSD_EXECUTABLE} ARGS "cxx-tree" ${XSD_ARGS}
		DEPENDS ${DAMARIS_SOURCE_DIR}/src/model/Model.xsd
		COMMENT "Generating C++/XML mapping")

add_library(damaris 	${DAMARIS_SRC}
			${DAMARIS_VISIT_SRC}
			${DAMARIS_HDF5_SRC} 
                        ${DAMARIS_PARAVIEW_SRC}
			${DAMARIS_FORTRAN_MODULE})
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
endif(ENABLE_CATALYST)

# for shared libs, establish the lib version
set_target_properties(damaris
		PROPERTIES VERSION ${DAMARIS_VERSION}
		SOVERSION ${DAMARIS_VERSION_MAJOR})

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
# generate pkg-config file
#----------------------------------------------------------------------------#
set(LIBS_LIST_FOR_PKGCONFIG "")
set(LIBS_DIR_LIST_FOR_PKG_CONFIG "")
set(INCS_LIST_FOR_PKGCONFIG "")
# processing include dirs
foreach(p ${XSD_INCLUDE_DIRS} ${XercesC_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
  list(APPEND INCS_LIST_FOR_PKGCONFIG ${p})
endforeach(p)
if(ENABLE_HDF5)
  foreach(p ${HDF5_INCLUDE_DIRS})
    list(APPEND INCS_LIST_FOR_PKGCONFIG ${p})
  endforeach(p)
endif(ENABLE_HDF5)
if(ENABLE_VISIT)
  foreach(p ${VisIt_INCLUDE_DIRS})
    list(APPEND INCS_LIST_FOR_PKGCONFIG ${p})
  endforeach(p)
endif(ENABLE_VISIT)
if(ENABLE_CATALYST)
  foreach(p ${ParaView_INCLUDE_DIRS})
    list(APPEND INCS_LIST_FOR_PKGCONFIG ${p})
  endforeach(p)
endif(ENABLE_CATALYST)
list(REMOVE_DUPLICATES INCS_LIST_FOR_PKGCONFIG)
set(PKG_CONFIG_INC_FLAGS "")
foreach(p ${INCS_LIST_FOR_PKGCONFIG})
  string(CONCAT PKG_CONFIG_INC_FLAGS ${PKG_CONFIG_INC_FLAGS} "-I${p} ")
endforeach(p)
# processing library dirs and names
foreach(p ${DEP_LIBRARIES})
  get_filename_component(libname ${p} NAME_WE)
  get_filename_component(libpath ${p} DIRECTORY)
  list(APPEND LIBS_LIST_FOR_PKGCONFIG ${libname})
  list(APPEND LIBS_DIR_LIST_FOR_PKGCONFIG ${libpath})
endforeach(p)
list(REMOVE_DUPLICATES LIBS_LIST_FOR_PKGCONFIG)
list(REMOVE_DUPLICATES LIBS_DIR_LIST_FOR_PKGCONFIG)
set(PKG_CONFIG_LIBS_FLAGS "")
foreach(p ${LIBS_DIR_LIST_FOR_PKGCONFIG})
  string(CONCAT PKG_CONFIG_LIBS_FLAGS ${PKG_CONFIG_LIBS_FLAGS} "-L${p} ")
endforeach(p)
foreach(p ${LIBS_LIST_FOR_PKGCONFIG})
  string(REGEX REPLACE "^lib" "" p_without_lib ${p})
  string(CONCAT PKG_CONFIG_LIBS_FLAGS ${PKG_CONFIG_LIBS_FLAGS} "-l${p_without_lib} ")
endforeach(p)
configure_file(damaris.pc.in damaris.pc @ONLY)

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
install(DIRECTORY "${DAMARIS_SOURCE_DIR}/include/damaris" DESTINATION include)
if(CMAKE_Fortran_COMPILER)
  install(FILES "${CMAKE_BINARY_DIR}/lib/damaris.mod" DESTINATION include)
endif(CMAKE_Fortran_COMPILER)
