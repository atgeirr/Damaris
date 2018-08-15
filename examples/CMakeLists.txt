add_executable(Example Example.cpp)
target_link_libraries(Example damaris ${DEP_LIBRARIES})

add_subdirectory (visit)
add_subdirectory (storage)
add_subdirectory (plugin)
add_subdirectory (fortran)
add_subdirectory (paraview)
