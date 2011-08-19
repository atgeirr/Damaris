if(CMAKE_Fortran_COMPILER)
  add_executable (test_fortran ${DAMARIS_SOURCE_DIR}/test/test.f90)
  target_link_libraries (test_fortran ${BOOST_LIBRARIES} xerces-c damaris rt)
endif(CMAKE_Fortran_COMPILER)

add_executable (test_cpp ${DAMARIS_SOURCE_DIR}/test/test.cpp)
target_link_libraries (test_cpp ${BOOST_LIBRARIES} xerces-c damaris rt)

add_executable (test_cpp_alloc ${DAMARIS_SOURCE_DIR}/test/test_alloc.cpp)
target_link_libraries (test_cpp_alloc ${BOOST_LIBRARIES} xerces-c damaris rt)

add_executable (test_c ${DAMARIS_SOURCE_DIR}/test/test.c)
target_link_libraries (test_c ${BOOST_LIBRARIES} xerces-c damaris rt)
