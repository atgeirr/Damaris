set(DAMARIS_CLIENT_SRC 
	${DAMARIS_SOURCE_DIR}/client/Client.cpp
	${DAMARIS_SOURCE_DIR}/common/Configuration.cpp
	${DAMARIS_SOURCE_DIR}/common/ConfigHandler.cpp 
        ${DAMARIS_SOURCE_DIR}/common/LayoutFactory.cpp
	${DAMARIS_SOURCE_DIR}/common/Layout.cpp)

add_library (damaris ${DAMARIS_CLIENT_SRC})
