set(DAMARIS_SERVER_LIB_SRC
		${DAMARIS_SOURCE_DIR}/server/Initiator.cpp
		${DAMARIS_SOURCE_DIR}/server/Server.cpp
		${DAMARIS_SOURCE_DIR}/server/ServerC.cpp
		${DAMARIS_SOURCE_DIR}/server/ServerFortran.cpp
		${DAMARIS_SOURCE_DIR}/server/Options.cpp)

add_library (damaris-server ${DAMARIS_SERVER_LIB_SRC})

add_executable (server ${DAMARIS_SOURCE_DIR}/server/main.cpp)
set_target_properties(server PROPERTIES LINK_FLAGS "-rdynamic")

target_link_libraries (server damaris-server damaris ${XERCESC_LIB} ${BOOST_LIB} ${PYTHON_LIB} ${VISIT_LIB} rt dl)
