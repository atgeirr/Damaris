set(CLIENT_SIDE_LIBRARIES damaris ${BOOST_LIBRARIES} xerces-c rt)
set(EMB_SERVER_LIBRARIES damaris-server ${CLIENT_SIDE_LIBRARIES} dl)

if(CMAKE_Fortran_COMPILER)
  add_executable (test_fortran ${DAMARIS_SOURCE_DIR}/examples/test.f90)
  target_link_libraries (test_fortran ${CLIENT_SIDE_LIBRARIES} rt dl)

  add_executable (test_alloc_fortran ${DAMARIS_SOURCE_DIR}/examples/test_alloc.f90
		  ${DAMARIS_SOURCE_DIR}/include/damaris.mod)
  target_link_libraries (test_alloc_fortran ${CLIENT_SIDE_LIBRARIES} rt dl)
endif(CMAKE_Fortran_COMPILER)

add_executable (test_alloc_c ${DAMARIS_SOURCE_DIR}/examples/test_alloc.c)
target_link_libraries (test_alloc_c ${CLIENT_SIDE_LIBRARIES} rt dl)

add_executable (test_c ${DAMARIS_SOURCE_DIR}/examples/test.c)
target_link_libraries (test_c ${CLIENT_SIDE_LIBRARIES} rt dl)

if(HAS_MPI)
  add_executable (test_mpi_c ${DAMARIS_SOURCE_DIR}/examples/test_mpi.c)
  target_link_libraries (test_mpi_c ${EMB_SERVER_LIBRARIES} rt dl)

  if(CMAKE_Fortran_COMPILER)
    add_executable (test_mpi_fortran ${DAMARIS_SOURCE_DIR}/examples/test_mpi.f90)
    target_link_libraries (test_mpi_fortran ${EMB_SERVER_LIBRARIES} rt dl)
  endif(CMAKE_Fortran_COMPILER)
endif(HAS_MPI)
