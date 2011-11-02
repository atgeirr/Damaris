#add_definitions(-D__DAMARIS_CLIENT -D__INFO)



set(DAMARIS_CLIENT_SRC 
	${DAMARIS_SOURCE_DIR}/client/Client.cpp
	${DAMARIS_SOURCE_DIR}/client/ClientC.cpp
	${DAMARIS_SOURCE_DIR}/client/ClientFortran.cpp
	${DAMARIS_SOURCE_DIR}/client/Client.f90)

set(DAMARIS_MODEL_SRC
	${DAMARIS_SOURCE_DIR}/xml/Model.cpp)

set(DAMARIS_COMMON_SRC
	${DAMARIS_SOURCE_DIR}/common/Parameter.cpp
	${DAMARIS_SOURCE_DIR}/common/ParameterSet.cpp
	${DAMARIS_SOURCE_DIR}/common/SharedMessageQueue.cpp
	${DAMARIS_SOURCE_DIR}/common/SharedMemorySegment.cpp
	${DAMARIS_SOURCE_DIR}/common/Environment.cpp
	${DAMARIS_SOURCE_DIR}/common/Configuration.cpp
	${DAMARIS_SOURCE_DIR}/common/Variable.cpp
	${DAMARIS_SOURCE_DIR}/common/MetadataManager.cpp
	${DAMARIS_SOURCE_DIR}/common/Action.cpp
	${DAMARIS_SOURCE_DIR}/common/DynamicAction.cpp
	${DAMARIS_SOURCE_DIR}/common/ActionsManager.cpp
	${DAMARIS_SOURCE_DIR}/common/Chunk.cpp
	${DAMARIS_SOURCE_DIR}/common/ShmChunk.cpp
	${DAMARIS_SOURCE_DIR}/common/Language.cpp
	${DAMARIS_SOURCE_DIR}/common/Types.cpp 
	${DAMARIS_SOURCE_DIR}/common/Layout.cpp)

set(DAMARIS_SCRIPTS_SRC
	${DAMARIS_SOURCE_DIR}/scripts/PyInterpreter.cpp
	${DAMARIS_SOURCE_DIR}/scripts/PyAction.cpp)

add_library (damaris ${DAMARIS_CLIENT_SRC}
		     ${DAMARIS_MODEL_SRC}
	             ${DAMARIS_COMMON_SRC}
		     ${DAMARIS_SCRIPTS_SRC})
