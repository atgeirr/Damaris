set(CLIENT_SIDE_LIBRARIES damaris ${Boost_LIBRARIES} 
		${PYTHON_LIBRARIES} ${XERCESC_LIBRARIES} ${VISIT_LIB} rt dl)
set(SERVER_SIDE_LIBRARIES damaris-server)
set(VISIT_TEST_DIR ${DAMARIS_SOURCE_DIR}/visit/test)

add_definitions(-DPARALLEL)

add_library(gc SHARED gc.cpp)
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
  SET_TARGET_PROPERTIES(gc PROPERTIES COMPILE_FLAGS "-fPIC")
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )

add_executable (amr ${VISIT_TEST_DIR}/amr.c)
target_link_libraries (amr ${CLIENT_SIDE_LIBRARIES})
set_target_properties (amr PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")

add_executable (csg ${VISIT_TEST_DIR}/csg.c)
target_link_libraries (csg ${CLIENT_SIDE_LIBRARIES})
set_target_properties (csg PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")

add_executable (curve ${VISIT_TEST_DIR}/curve.c)
target_link_libraries (curve ${CLIENT_SIDE_LIBRARIES})
set_target_properties (curve PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")

add_executable (interleave ${VISIT_TEST_DIR}/interleave.c)
target_link_libraries (interleave ${CLIENT_SIDE_LIBRARIES})
set_target_properties (interleave PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")

add_executable (life ${VISIT_TEST_DIR}/life.c)
target_link_libraries (life 
				${SERVER_SIDE_LIBRARIES}
				${CLIENT_SIDE_LIBRARIES})
set_target_properties (life PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")

add_executable (mandelbrot ${VISIT_TEST_DIR}/mandelbrot.C
						   ${VISIT_TEST_DIR}/patch.C)
target_link_libraries (mandelbrot ${CLIENT_SIDE_LIBRARIES})
set_target_properties (mandelbrot PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")

add_executable (mandelbulb ${VISIT_TEST_DIR}/mandelbulb.c)
target_link_libraries (mandelbulb ${SERVER_SIDE_LIBRARIES} ${CLIENT_SIDE_LIBRARIES})
set_target_properties (mandelbulb PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")

add_executable (material ${VISIT_TEST_DIR}/material.c)
target_link_libraries (material ${CLIENT_SIDE_LIBRARIES})
set_target_properties (material PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")

add_executable (mesh ${VISIT_TEST_DIR}/mesh.c)
target_link_libraries (mesh ${CLIENT_SIDE_LIBRARIES})
set_target_properties (mesh PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")

add_executable (blocks ${VISIT_TEST_DIR}/blocks.c)
target_link_libraries (blocks ${CLIENT_SIDE_LIBRARIES})
set_target_properties (blocks PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")

add_executable (point ${VISIT_TEST_DIR}/point.c)
target_link_libraries (point ${CLIENT_SIDE_LIBRARIES})
set_target_properties (point PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")

add_executable (polyhedral ${VISIT_TEST_DIR}/polyhedral.c)
target_link_libraries (polyhedral ${CLIENT_SIDE_LIBRARIES})
set_target_properties (polyhedral PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")

add_executable (scalar ${VISIT_TEST_DIR}/scalar.c)
target_link_libraries (scalar ${CLIENT_SIDE_LIBRARIES})
set_target_properties (scalar PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")

add_executable (species ${VISIT_TEST_DIR}/species.c)
target_link_libraries (species ${CLIENT_SIDE_LIBRARIES})
set_target_properties (species PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")

add_executable (unstructured ${VISIT_TEST_DIR}/unstructured.c)
target_link_libraries (unstructured ${CLIENT_SIDE_LIBRARIES})
set_target_properties (unstructured PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")

add_executable (updateplots ${VISIT_TEST_DIR}/updateplots.c)
target_link_libraries (updateplots ${CLIENT_SIDE_LIBRARIES})
set_target_properties (updateplots PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")

add_executable (var ${VISIT_TEST_DIR}/var.c)
target_link_libraries (var ${CLIENT_SIDE_LIBRARIES})
set_target_properties (var PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")

add_subdirectory (origin)
