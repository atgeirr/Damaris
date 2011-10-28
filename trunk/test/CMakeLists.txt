set(CLIENT_SIDE_LIBRARIES damaris ${BOOST_LIBRARIES} python2.6 xerces-c rt)
set(EMB_SERVER_LIBRARIES damaris-server ${CLIENT_SIDE_LIBRARIES} dl)

if(CMAKE_Fortran_COMPILER)
#  add_executable (test_fortran ${DAMARIS_SOURCE_DIR}/test/test.f90)
#  target_link_libraries (test_fortran ${CLIENT_SIDE_LIBRARIES} rt dl)
endif(CMAKE_Fortran_COMPILER)

add_executable (julia_cpp ${DAMARIS_SOURCE_DIR}/test/julia.cpp)
target_link_libraries (julia_cpp ${CLIENT_SIDE_LIBRARIES} rt dl)

add_executable (julia_alloc_cpp ${DAMARIS_SOURCE_DIR}/test/julia_alloc.cpp)
target_link_libraries (julia_alloc_cpp ${CLIENT_SIDE_LIBRARIES} rt dl)

add_executable (julia_chunk_cpp ${DAMARIS_SOURCE_DIR}/test/julia_chunk.cpp)
target_link_libraries (julia_chunk_cpp ${CLIENT_SIDE_LIBRARIES} rt dl)

if(HAS_MPI)
  add_executable (julia_mpi_cpp ${DAMARIS_SOURCE_DIR}/test/julia_mpi.cpp)
  target_link_libraries (julia_mpi_cpp ${EMB_SERVER_LIBRARIES} rt dl)
endif(HAS_MPI)
