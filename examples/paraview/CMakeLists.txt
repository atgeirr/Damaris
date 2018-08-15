add_definitions(-DPARALLEL)

add_executable (image image.cpp)
target_link_libraries (image damaris ${DEP_LIBRARIES})
set_target_properties (image PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")
#configure_file(curve.xml.in curve.xml)
