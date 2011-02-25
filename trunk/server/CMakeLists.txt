set(DAMARIS_SERVER_SRC ${DAMARIS_SOURCE_DIR}/server/main.cpp
		       ${DAMARIS_SOURCE_DIR}/server/Server.cpp
		       ${DAMARIS_SOURCE_DIR}/common/Layout.cpp
		       ${DAMARIS_SOURCE_DIR}/common/LayoutFactory.cpp
		       ${DAMARIS_SOURCE_DIR}/common/MetadataManager.cpp
		       ${DAMARIS_SOURCE_DIR}/common/Variable.cpp
		       ${DAMARIS_SOURCE_DIR}/common/Reaction.cpp
		       ${DAMARIS_SOURCE_DIR}/common/BehaviorManager.cpp)

add_executable (server ${DAMARIS_SERVER_SRC})

target_link_libraries (server ${BOOST_LIBRARIES} rt)
