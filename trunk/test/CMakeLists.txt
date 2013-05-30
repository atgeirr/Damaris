set(CLIENT_SIDE_LIBRARIES damaris ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} 
		${XERCESC_LIBRARIES} ${VISIT_LIB} rt dl)
set(EMB_SERVER_LIBRARIES damaris-server ${CLIENT_SIDE_LIBRARIES})

#add_library(Plugin SHARED pluginHello.cpp)
add_library(gc SHARED gc.cpp)
add_library(Write SHARED writePlugin.cpp)
add_library(Read SHARED readPlugin.cpp)

#set_target_properties(hello PROPERTIES LINK_FLAGS "-Wl,--whole-archive")

IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
  SET_TARGET_PROPERTIES(hello PROPERTIES COMPILE_FLAGS "-fPIC")
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )

add_executable (mandelbulb ${DAMARIS_SOURCE_DIR}/test/mandelbulb.c
                         ${DAMARIS_SOURCE_DIR}/hdf5/HDF5Plugin.cpp)
target_link_libraries (mandelbulb ${EMB_SERVER_LIBRARIES} hdf5)
set_target_properties (mandelbulb PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")

add_executable (julia_cpp ${DAMARIS_SOURCE_DIR}/test/julia.cpp)
target_link_libraries (julia_cpp ${CLIENT_SIDE_LIBRARIES})

add_executable (julia_alloc_cpp ${DAMARIS_SOURCE_DIR}/test/julia_alloc.cpp)
target_link_libraries (julia_alloc_cpp ${CLIENT_SIDE_LIBRARIES})

add_executable (julia_mpi_cpp ${DAMARIS_SOURCE_DIR}/test/julia_mpi.cpp)
target_link_libraries (julia_mpi_cpp ${EMB_SERVER_LIBRARIES})
set_target_properties (julia_mpi_cpp PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")

add_executable (test_mpi ${DAMARIS_SOURCE_DIR}/test/test_mpi.c)
target_link_libraries (test_mpi ${EMB_SERVER_LIBRARIES})
set_target_properties (test_mpi PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")

add_executable (testRPC ${DAMARIS_SOURCE_DIR}/test/testRPC.cpp)
target_link_libraries (testRPC ${CLIENT_SIDE_LIBRARIES})

#add_executable (test_asio ${DAMARIS_SOURCE_DIR}/test/test_asio.cpp)
#target_link_libraries (test_asio ${EMB_SERVER_LIBRARIES})
#set_target_properties (test_asio PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")
