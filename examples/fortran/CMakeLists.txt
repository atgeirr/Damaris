add_definitions(-DPARALLEL)


if(ENABLE_FORTRAN)
    add_executable (cube cube.f90)
    target_link_libraries (cube damaris ${DEP_LIBRARIES})
    set_target_properties (cube PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")
    set_target_properties (cube PROPERTIES LINKER_LANGUAGE Fortran)
    configure_file(cube.xml.in cube.xml)
endif(ENABLE_FORTRAN)
