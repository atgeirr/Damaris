set(CLIENT_SIDE_LIBRARIES damaris ${BOOST_LIB} ${PYTHON_LIB} ${XERCESC_LIB} ${VISIT_LIB} rt dl)
set(EMB_SERVER_LIBRARIES damaris-server ${CLIENT_SIDE_LIBRARIES})

add_library(hello SHARED hello.cpp)

IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
  SET_TARGET_PROPERTIES(hello PROPERTIES COMPILE_FLAGS "-fPIC")
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )

add_executable (julia_cpp ${DAMARIS_SOURCE_DIR}/test/julia.cpp)
target_link_libraries (julia_cpp ${CLIENT_SIDE_LIBRARIES})

add_executable (julia_alloc_cpp ${DAMARIS_SOURCE_DIR}/test/julia_alloc.cpp)
target_link_libraries (julia_alloc_cpp ${CLIENT_SIDE_LIBRARIES})

add_executable (julia_mpi_cpp ${DAMARIS_SOURCE_DIR}/test/julia_mpi.cpp)
target_link_libraries (julia_mpi_cpp ${EMB_SERVER_LIBRARIES})
set_target_properties (julia_mpi_cpp PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")

add_executable (testRPC ${DAMARIS_SOURCE_DIR}/test/testRPC.cpp)
target_link_libraries (testRPC ${CLIENT_SIDE_LIBRARIES})

#add_executable (test_asio ${DAMARIS_SOURCE_DIR}/test/test_asio.cpp)
#target_link_libraries (test_asio ${EMB_SERVER_LIBRARIES})
#set_target_properties (test_asio PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")
