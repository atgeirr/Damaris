add_executable(Example Example.cpp)
add_library(example SHARED Example_lib.cpp)
target_link_libraries(Example damaris ${DEP_LIBRARIES})

include_directories(${DAMARIS_SOURCE_DIR}/include)
include_directories(${DAMARIS_SOURCE_DIR}/include/damaris)
target_compile_options(example PRIVATE -fPIC)
target_link_libraries(example damaris ${DEP_LIBRARIES})


add_subdirectory (paraview)
add_subdirectory (storage)
add_subdirectory (fortran)
add_subdirectory (visit)
add_subdirectory (plugin)
add_subdirectory (python)
