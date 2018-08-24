add_definitions(-DPARALLEL)

add_executable (image image.cpp)
target_link_libraries (image damaris ${DEP_LIBRARIES})
set_target_properties (image PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")

add_executable (sector sector.cpp)
target_link_libraries (sector damaris ${DEP_LIBRARIES})
set_target_properties (sector PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")
