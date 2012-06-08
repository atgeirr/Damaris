set(CLIENT_SIDE_LIBRARIES damaris ${BOOST_LIB} ${PYTHON_LIB} ${XERCESC_LIB} ${VISIT_LIB} rt dl)
set(VISIT_TEST_DIR ${DAMARIS_SOURCE_DIR}/visit/test)

add_definitions(-DPARALLEL)

add_library(gc SHARED gc.cpp)
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
  SET_TARGET_PROPERTIES(gc PROPERTIES COMPILE_FLAGS "-fPIC")
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )

add_executable (amr ${VISIT_TEST_DIR}/amr.c)
target_link_libraries (amr ${CLIENT_SIDE_LIBRARIES})

add_executable (csg ${VISIT_TEST_DIR}/csg.c)
target_link_libraries (csg ${CLIENT_SIDE_LIBRARIES})

add_executable (curve ${VISIT_TEST_DIR}/curve.c)
target_link_libraries (curve ${CLIENT_SIDE_LIBRARIES})

add_executable (interleave ${VISIT_TEST_DIR}/interleave.c)
target_link_libraries (interleave ${CLIENT_SIDE_LIBRARIES})

add_executable (life ${VISIT_TEST_DIR}/life.c)
target_link_libraries (life ${CLIENT_SIDE_LIBRARIES})

add_executable (mandelbrot ${VISIT_TEST_DIR}/mandelbrot.C
						   ${VISIT_TEST_DIR}/patch.C)
target_link_libraries (mandelbrot ${CLIENT_SIDE_LIBRARIES})

add_executable (material ${VISIT_TEST_DIR}/material.c)
target_link_libraries (material ${CLIENT_SIDE_LIBRARIES})

add_executable (mesh ${VISIT_TEST_DIR}/mesh.c)
target_link_libraries (mesh ${CLIENT_SIDE_LIBRARIES})

add_executable (point ${VISIT_TEST_DIR}/point.c)
target_link_libraries (point ${CLIENT_SIDE_LIBRARIES})

add_executable (polyhedral ${VISIT_TEST_DIR}/polyhedral.c)
target_link_libraries (polyhedral ${CLIENT_SIDE_LIBRARIES})

add_executable (scalar ${VISIT_TEST_DIR}/scalar.c)
target_link_libraries (scalar ${CLIENT_SIDE_LIBRARIES})

add_executable (species ${VISIT_TEST_DIR}/species.c)
target_link_libraries (species ${CLIENT_SIDE_LIBRARIES})

add_executable (unstructured ${VISIT_TEST_DIR}/unstructured.c)
target_link_libraries (unstructured ${CLIENT_SIDE_LIBRARIES})

add_executable (updateplots ${VISIT_TEST_DIR}/updateplots.c)
target_link_libraries (updateplots ${CLIENT_SIDE_LIBRARIES})

add_executable (var ${VISIT_TEST_DIR}/var.c)
target_link_libraries (var ${CLIENT_SIDE_LIBRARIES})

add_subdirectory (origin)
