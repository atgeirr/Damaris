set(CLIENT_SIDE_LIBRARIES damaris ${BOOST_LIB} ${PYTHON_LIB} ${XERCESC_LIB} rt dl)
set(EMB_SERVER_LIBRARIES damaris-server ${CLIENT_SIDE_LIBRARIES})

include_directories(${DAMARIS_SOURCE_DIR}/client)
include_directories(${DAMARIS_SOURCE_DIR}/include)

if(CMAKE_Fortran_COMPILER)
  add_executable (test_fortran ${DAMARIS_SOURCE_DIR}/examples/test.f90)
  target_link_libraries (test_fortran ${CLIENT_SIDE_LIBRARIES} gfortran)

#  the following two lines are supposed to compile the fortran alloc test,
#  which requires a module. Yet CMake is not able to find the module itself,
#  so the test must be compiled manualy or the module file should be copied 
#  in the examples directory before uncommenting these lines and running
#  "make" again.

  add_executable (test_alloc_fortran ${DAMARIS_SOURCE_DIR}/examples/test_alloc.f90)
  target_link_libraries (test_alloc_fortran ${CLIENT_SIDE_LIBRARIES} gfortran)

  add_executable (test_chunk_fortran ${DAMARIS_SOURCE_DIR}/examples/test_chunk.f90)
  target_link_libraries (test_chunk_fortran ${CLIENT_SIDE_LIBRARIES} gfortran)

endif(CMAKE_Fortran_COMPILER)

add_executable (test_alloc_c ${DAMARIS_SOURCE_DIR}/examples/test_alloc.c)
target_link_libraries (test_alloc_c ${CLIENT_SIDE_LIBRARIES})

add_executable (test_c ${DAMARIS_SOURCE_DIR}/examples/test.c)
target_link_libraries (test_c ${CLIENT_SIDE_LIBRARIES})

add_executable (test_event_c ${DAMARIS_SOURCE_DIR}/examples/test_event.c)
target_link_libraries (test_event_c ${CLIENT_SIDE_LIBRARIES})

add_executable (test_chunk_c ${DAMARIS_SOURCE_DIR}/examples/test_chunk.c)
target_link_libraries (test_chunk_c ${CLIENT_SIDE_LIBRARIES})

add_executable (test_mpi_c ${DAMARIS_SOURCE_DIR}/examples/test_mpi.c)
target_link_libraries (test_mpi_c ${EMB_SERVER_LIBRARIES})

if(CMAKE_Fortran_COMPILER)
  add_executable (test_mpi_fortran ${DAMARIS_SOURCE_DIR}/examples/test_mpi.f90)
  target_link_libraries (test_mpi_fortran ${EMB_SERVER_LIBRARIES} gfortran)
endif(CMAKE_Fortran_COMPILER)
