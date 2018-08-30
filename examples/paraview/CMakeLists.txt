add_definitions(-DPARALLEL)

add_executable (image image.cpp)
target_link_libraries (image damaris ${DEP_LIBRARIES})
set_target_properties (image PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")

add_executable (sector sector.cpp)
target_link_libraries (sector damaris ${DEP_LIBRARIES})
set_target_properties (sector PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")

add_executable (plate plate.cpp)
target_link_libraries (plate damaris ${DEP_LIBRARIES})
set_target_properties (plate PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")

add_executable (lollipop lollipop.cpp)
target_link_libraries (lollipop damaris ${DEP_LIBRARIES})
set_target_properties (lollipop PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")
