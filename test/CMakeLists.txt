configure_file(test.xml test.xml COPYONLY)
	
add_executable(TestDataSpace TestDataSpace.cpp)
target_link_libraries(TestDataSpace ${DEP_LIBRARIES} ${CPPUNIT_LIBRARIES})

add_executable(TestVariableManager TestVariableManager.cpp)
target_link_libraries(TestVariableManager damaris ${DEP_LIBRARIES} ${CPPUNIT_LIBRARIES})

add_executable(TestLayoutManager TestLayoutManager.cpp)
target_link_libraries(TestLayoutManager damaris ${DEP_LIBRARIES} ${CPPUNIT_LIBRARIES})

add_executable(TestChannel TestChannel.cpp)
target_link_libraries(TestChannel damaris ${DEP_LIBRARIES} ${CPPUNIT_LIBRARIES})

add_executable(TestReactor TestReactor.cpp)
target_link_libraries(TestReactor damaris ${DEP_LIBRARIES} ${CPPUNIT_LIBRARIES})

add_executable(TestBuffer TestBuffer.cpp)
target_link_libraries(TestBuffer damaris ${DEP_LIBRARIES} ${CPPUNIT_LIBRARIES})

add_executable(TestVariable TestVariable.cpp)
target_link_libraries(TestVariable damaris ${DEP_LIBRARIES} ${CPPUNIT_LIBRARIES})

add_executable(TestActionManager TestActionManager.cpp)
target_link_libraries(TestActionManager damaris ${DEP_LIBRARIES} ${CPPUNIT_LIBRARIES})

add_executable(TestAction TestAction.cpp)
target_link_libraries(TestAction damaris ${DEP_LIBRARIES} ${CPPUNIT_LIBRARIES})

add_executable(TestEventLogger TestEventLogger.cpp)
target_link_libraries(TestEventLogger damaris ${DEP_LIBRARIES} ${CPPUNIT_LIBRARIES})


add_test(NAME "TestDataSpace"       COMMAND ${CMAKE_BINARY_DIR}/test/TestDataSpace)
add_test(NAME "TestVariableManager" COMMAND ${CMAKE_BINARY_DIR}/test/TestVariableManager)
add_test(NAME "TestLayoutManager"   COMMAND ${CMAKE_BINARY_DIR}/test/TestLayoutManager)
add_test(NAME "TestChannel"         COMMAND mpirun -np 2 $MPI_OPTIONS ${CMAKE_BINARY_DIR}/test/TestChannel)
add_test(NAME "TestReactor"         COMMAND mpirun -np 4 $MPI_OPTIONS ${CMAKE_BINARY_DIR}/test/TestReactor)
add_test(NAME "TestBuffer"          COMMAND mpirun -np 2 $MPI_OPTIONS ${CMAKE_BINARY_DIR}/test/TestBuffer)
add_test(NAME "TestVariable"        COMMAND mpirun -np 3 $MPI_OPTIONS ${CMAKE_BINARY_DIR}/test/TestVariable)
add_test(NAME "TestActionManager"   COMMAND mpirun -np 1 $MPI_OPTIONS ${CMAKE_BINARY_DIR}/test/TestActionManager)
add_test(NAME "TestAction"          COMMAND mpirun -np 3 $MPI_OPTIONS ${CMAKE_BINARY_DIR}/test/TestAction)
add_test(NAME "TestEventLogger"     COMMAND ${CMAKE_BINARY_DIR}/test/TestEventLogger)
