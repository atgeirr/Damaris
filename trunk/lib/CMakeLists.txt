include_directories(${CMAKE_BINARY_DIR})

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
	${DAMARIS_SOURCE_DIR}/src/model/Model.cpp
	${DAMARIS_SOURCE_DIR}/src/model/BcastXML.cpp
	)

if(VisIt_FOUND)
set(DAMARIS_VISIT_SRC
	${DAMARIS_SOURCE_DIR}/src/visit/VisItListener.cpp
	)
endif(VisIt_FOUND)

if(CMAKE_Fortran_COMPILER)
set(DAMARIS_FORTRAN_MODULE 
	${DAMARIS_SOURCE_DIR}/src/main/Damaris.F90
	)
endif(CMAKE_Fortran_COMPILER)

add_library (damaris 	${DAMARIS_SRC} 
			${DAMARIS_VISIT_SRC} 
			${DAMARIS_FORTRAN_MODULE})

if(NOT (MODEL_PRESENT EQUAL 1))

set_source_files_properties(
		${CMAKE_BINARY_DIR}/src/model/Model.cpp
		PROPERTIES GENERATED 1)

add_dependencies(damaris XSDModel)

add_custom_target(XSDModel DEPENDS ${DAMARIS_SOURCE_DIR}/src/model/Model.cpp)

set(XSD_ARGS 	--output-dir ${DAMARIS_SOURCE_DIR}/src/model
		--type-naming knr 
		--hxx-suffix .hpp 
		--cxx-suffix .cpp 
		${DAMARIS_SOURCE_DIR}/src/model/Model.xsd) 

add_custom_command(OUTPUT ${DAMARIS_SOURCE_DIR}/src/model/Model.cpp
		COMMAND ${XSD_EXECUTABLE} ARGS "cxx-tree" ${XSD_ARGS}
		COMMAND cp ARGS ${DAMARIS_SOURCE_DIR}/src/model/Model.hpp ${DAMARIS_SOURCE_DIR}/include/damaris/model/Model.hpp
		DEPENDS ${DAMARIS_SOURCE_DIR}/src/model/Model.xsd
		COMMENT "Generating C++/XML mapping")

endif()
