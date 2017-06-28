add_definitions(-DPARALLEL)

add_executable (mandelbulb-hdf5 mandelbulb-hdf5.c)
target_link_libraries (mandelbulb-hdf5 damaris ${DEP_LIBRARIES})
set_target_properties (mandelbulb-hdf5 PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")


