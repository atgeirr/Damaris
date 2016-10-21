add_definitions(-DPARALLEL)

add_executable (curve curve.c)
target_link_libraries (curve damaris ${DEP_LIBRARIES})
set_target_properties (curve PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")
configure_file(curve.xml.in curve.xml)

add_executable (life life.c)
target_link_libraries (life damaris ${DEP_LIBRARIES})
set_target_properties (life PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")
configure_file(life.xml.in life.xml)

add_executable (mandelbulb mandelbulb.c)
target_link_libraries (mandelbulb damaris ${DEP_LIBRARIES})
set_target_properties (mandelbulb PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")
configure_file(mandelbulb.xml.in mandelbulb.xml)

add_executable (mesh mesh.c)
target_link_libraries (mesh damaris ${DEP_LIBRARIES})
set_target_properties (mesh PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")
configure_file(mesh.xml.in mesh.xml)

add_executable (blocks blocks.c)
target_link_libraries (blocks damaris ${DEP_LIBRARIES})
set_target_properties (blocks PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")
configure_file(blocks.xml.in blocks.xml)

add_executable (point point.c)
target_link_libraries (point damaris ${DEP_LIBRARIES})
set_target_properties (point PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")
configure_file(point.xml.in point.xml)

add_executable (var var.c)
target_link_libraries (var damaris ${DEP_LIBRARIES})
set_target_properties (var PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")
configure_file(var.xml.in var.xml)
