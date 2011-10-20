set(CLIENT_SIDE_LIBRARIES damaris ${BOOST_LIBRARIES} xerces-c rt)
set(EMB_SERVER_LIBRARIES damaris-server ${CLIENT_SIDE_LIBRARIES} dl)

if(CMAKE_Fortran_COMPILER)
  add_executable (test_fortran ${DAMARIS_SOURCE_DIR}/test/test.f90)
  target_link_libraries (test_fortran ${CLIENT_SIDE_LIBRARIES} rt dl)

  add_executable (test_fortran_alloc ${DAMARIS_SOURCE_DIR}/test/test_alloc.f90)
  target_link_libraries (test_fortran_alloc ${CLIENT_SIDE_LIBRARIES} rt dl)
endif(CMAKE_Fortran_COMPILER)

add_executable (test_cpp ${DAMARIS_SOURCE_DIR}/test/test.cpp)
target_link_libraries (test_cpp ${CLIENT_SIDE_LIBRARIES} rt dl)

add_executable (test_cpp_alloc ${DAMARIS_SOURCE_DIR}/test/test_alloc.cpp)
target_link_libraries (test_cpp_alloc ${CLIENT_SIDE_LIBRARIES} rt dl)

add_executable (test_c ${DAMARIS_SOURCE_DIR}/test/test.c)
target_link_libraries (test_c ${CLIENT_SIDE_LIBRARIES} rt dl)

#add_executable (test_mpi ${DAMARIS_SOURCE_DIR}/test/test_mpi.c)
#target_link_libraries (test_mpi ${EMB_SERVER_LIBRARIES} rt dl)

