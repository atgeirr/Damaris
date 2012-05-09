set(CLIENT_SIDE_LIBRARIES damaris ${BOOST_LIB} ${PYTHON_LIB} ${XERCESC_LIB} ${VISIT_LIB} rt dl)
set(EMB_SERVER_LIBRARIES damaris-server ${CLIENT_SIDE_LIBRARIES})

add_library(hello SHARED hello.cpp)

add_executable (julia_cpp ${DAMARIS_SOURCE_DIR}/test/julia.cpp)
target_link_libraries (julia_cpp ${CLIENT_SIDE_LIBRARIES})

add_executable (julia_alloc_cpp ${DAMARIS_SOURCE_DIR}/test/julia_alloc.cpp)
target_link_libraries (julia_alloc_cpp ${CLIENT_SIDE_LIBRARIES})

add_executable (julia_mpi_cpp ${DAMARIS_SOURCE_DIR}/test/julia_mpi.cpp)
target_link_libraries (julia_mpi_cpp ${EMB_SERVER_LIBRARIES})
