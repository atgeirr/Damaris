add_definitions(-DPARALLEL)


if(CMAKE_Fortran_COMPILER)
    add_executable (cube cube.f90)
    target_link_libraries (cube damaris ${DEP_LIBRARIES})
    set_target_properties (cube PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")
    configure_file(cube.xml.in cube.xml)
endif(CMAKE_Fortran_COMPILER)
