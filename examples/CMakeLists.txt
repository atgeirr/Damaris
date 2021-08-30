add_executable(Example Example.cpp)
target_link_libraries(Example damaris ${DEP_LIBRARIES})

add_subdirectory (paraview)
add_subdirectory (storage)
add_subdirectory (fortran)
add_subdirectory (visit)
add_subdirectory (plugin)
