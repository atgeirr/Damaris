add_definitions(-D__DEBUG -D__INFO -D__DAMARIS_SERVER)

set(DAMARIS_SERVER_LIB_SRC
		${DAMARIS_SOURCE_DIR}/server/Server.cpp
		${DAMARIS_SOURCE_DIR}/server/ServerC.cpp
		${DAMARIS_SOURCE_DIR}/server/ServerFortran.cpp
		${DAMARIS_SOURCE_DIR}/server/Options.cpp
		${DAMARIS_SOURCE_DIR}/server/ServerConfiguration.cpp
		${DAMARIS_SOURCE_DIR}/server/Action.cpp
		${DAMARIS_SOURCE_DIR}/server/ActionsManager.cpp)

set(DAMARIS_SERVER_PRG_SRC
		${DAMARIS_SOURCE_DIR}/server/main.cpp
		${DAMARIS_SOURCE_DIR}/common/ConfigHandler.cpp
		${DAMARIS_SOURCE_DIR}/common/Environment.cpp
		${DAMARIS_SOURCE_DIR}/common/Configuration.cpp
		${DAMARIS_SOURCE_DIR}/common/Types.cpp
		${DAMARIS_SOURCE_DIR}/common/Layout.cpp
		${DAMARIS_SOURCE_DIR}/common/LayoutFactory.cpp
		${DAMARIS_SOURCE_DIR}/common/MetadataManager.cpp
		${DAMARIS_SOURCE_DIR}/common/Variable.cpp)

add_library (damaris-server ${DAMARIS_SERVER_LIB_SRC})

add_executable (server ${DAMARIS_SERVER_PRG_SRC} ${DAMARIS_SERVER_LIB_SRC})

target_link_libraries (server ${BOOST_LIBRARIES} xerces-c boost_program_options rt dl) #hdf5 z)
