set(VISIT_LIBRARIES ${VISIT_LIB} rt dl m)
set(VISIT_TEST_DIR ${DAMARIS_SOURCE_DIR}/visit/test/origin)

add_definitions(-DPARALLEL)

add_executable (amr_or ${VISIT_TEST_DIR}/amr.c)
target_link_libraries (amr_or ${VISIT_LIBRARIES})

add_executable (csg_or ${VISIT_TEST_DIR}/csg.c)
target_link_libraries (csg_or ${VISIT_LIBRARIES})

add_executable (curve_or ${VISIT_TEST_DIR}/curve.c)
target_link_libraries (curve_or ${VISIT_LIBRARIES})

add_executable (interleave_or ${VISIT_TEST_DIR}/interleave.c)
target_link_libraries (interleave_or ${VISIT_LIBRARIES})

add_executable (life_or ${VISIT_TEST_DIR}/life.c)
target_link_libraries (life_or ${VISIT_LIBRARIES})

add_executable (mandelbrot_or ${VISIT_TEST_DIR}/mandelbrot.C
						   ${VISIT_TEST_DIR}/patch.C)
target_link_libraries (mandelbrot_or ${VISIT_LIBRARIES})

add_executable (material_or ${VISIT_TEST_DIR}/material.c)
target_link_libraries (material_or ${VISIT_LIBRARIES})

add_executable (mesh_or ${VISIT_TEST_DIR}/mesh.c)
target_link_libraries (mesh_or ${VISIT_LIBRARIES})

add_executable (blocks_or ${VISIT_TEST_DIR}/blocks.c)
target_link_libraries (blocks_or ${VISIT_LIBRARIES})

add_executable (point_or ${VISIT_TEST_DIR}/point.c)
target_link_libraries (point_or ${VISIT_LIBRARIES})

add_executable (polyhedral_or ${VISIT_TEST_DIR}/polyhedral.c)
target_link_libraries (polyhedral_or ${VISIT_LIBRARIES})

add_executable (scalar_or ${VISIT_TEST_DIR}/scalar.c)
target_link_libraries (scalar_or ${VISIT_LIBRARIES})

add_executable (species_or ${VISIT_TEST_DIR}/species.c)
target_link_libraries (species_or ${VISIT_LIBRARIES})

add_executable (unstructured_or ${VISIT_TEST_DIR}/unstructured.c)
target_link_libraries (unstructured_or ${VISIT_LIBRARIES})

add_executable (updateplots_or ${VISIT_TEST_DIR}/updateplots.c)
target_link_libraries (updateplots_or ${VISIT_LIBRARIES})

add_executable (var_or ${VISIT_TEST_DIR}/var.c)
target_link_libraries (var_or ${VISIT_LIBRARIES})
