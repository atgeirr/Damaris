add_definitions(-D__DAMARIS_CLIENT)

set(DAMARIS_CLIENT_SRC 
	${DAMARIS_SOURCE_DIR}/client/Client.cpp
	${DAMARIS_SOURCE_DIR}/client/ClientC.cpp
	${DAMARIS_SOURCE_DIR}/client/ClientFortran.cpp
	${DAMARIS_SOURCE_DIR}/common/Environment.cpp
	${DAMARIS_SOURCE_DIR}/common/Configuration.cpp
	${DAMARIS_SOURCE_DIR}/client/ClientConfiguration.cpp
	${DAMARIS_SOURCE_DIR}/common/ConfigHandler.cpp
	${DAMARIS_SOURCE_DIR}/common/Variable.cpp
	${DAMARIS_SOURCE_DIR}/common/MetadataManager.cpp
	${DAMARIS_SOURCE_DIR}/common/Types.cpp 
        ${DAMARIS_SOURCE_DIR}/common/LayoutFactory.cpp
	${DAMARIS_SOURCE_DIR}/common/Layout.cpp)

add_library (damaris ${DAMARIS_CLIENT_SRC})
