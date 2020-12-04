add_definitions(-DPARALLEL)

include_directories(${DAMARIS_SOURCE_DIR}/include/damaris)
include_directories(${DAMARIS_SOURCE_DIR}/include/damaris/data)
add_library (gc SHARED gc.cpp)
target_link_libraries (gc damaris ${DEP_LIBRARIES})
target_compile_options(gc PRIVATE -fPIC)


