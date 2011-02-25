add_executable (test_fortran ${DAMARIS_SOURCE_DIR}/test/test.f90)

target_link_libraries (test_fortran ${BOOST_LIBRARIES} damaris rt)
