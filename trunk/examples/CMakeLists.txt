add_executable(Example Example.cpp)
target_link_libraries(Example damaris ${DEP_LIBRARIES})

add_subdirectory (visit)
