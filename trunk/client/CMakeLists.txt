set(DAMARIS_CLIENT_SRC 
	${DAMARIS_SOURCE_DIR}/client/Client.cpp
	${DAMARIS_SOURCE_DIR}/client/ClientC.cpp
	${DAMARIS_SOURCE_DIR}/client/ClientFortran.cpp
	${DAMARIS_SOURCE_DIR}/client/Client.f90
	${DAMARIS_SOURCE_DIR}/client/StdAloneClient.cpp)

set(DAMARIS_MODEL_SRC
	${DAMARIS_SOURCE_DIR}/xml/Model.cpp)

set(DAMARIS_COMMON_SRC
	${DAMARIS_SOURCE_DIR}/core/Process.cpp
	${DAMARIS_SOURCE_DIR}/core/ParameterManager.cpp
	${DAMARIS_SOURCE_DIR}/core/SharedMessageQueue.cpp
	${DAMARIS_SOURCE_DIR}/core/SharedMemorySegment.cpp
	${DAMARIS_SOURCE_DIR}/core/Environment.cpp
	${DAMARIS_SOURCE_DIR}/core/VariableManager.cpp
	${DAMARIS_SOURCE_DIR}/core/LayoutManager.cpp
	${DAMARIS_SOURCE_DIR}/core/Action.cpp
	${DAMARIS_SOURCE_DIR}/core/DynamicAction.cpp
	${DAMARIS_SOURCE_DIR}/core/NodeAction.cpp
	${DAMARIS_SOURCE_DIR}/core/ActionsManager.cpp
	${DAMARIS_SOURCE_DIR}/data/Variable.cpp
	${DAMARIS_SOURCE_DIR}/data/ChunkDescriptor.cpp
	${DAMARIS_SOURCE_DIR}/data/Chunk.cpp
	${DAMARIS_SOURCE_DIR}/data/ShmChunk.cpp
	${DAMARIS_SOURCE_DIR}/data/Types.cpp 
	${DAMARIS_SOURCE_DIR}/data/Layout.cpp
	${DAMARIS_SOURCE_DIR}/data/Parameter.cpp)


if(PYTHON_LIB)
  set(DAMARIS_SCRIPTS_SRC
		${DAMARIS_SCRIPTS_SRC}
		${DAMARIS_SOURCE_DIR}/scripts/python/PyInterpreter.cpp
		${DAMARIS_SOURCE_DIR}/scripts/python/PyVariable.cpp
		${DAMARIS_SOURCE_DIR}/scripts/python/PyAction.cpp
		${DAMARIS_SOURCE_DIR}/scripts/python/PyChunk.cpp
		${DAMARIS_SOURCE_DIR}/scripts/python/PyLayout.cpp
		${DAMARIS_SOURCE_DIR}/scripts/python/PyTypes.cpp)
endif(PYTHON_LIB)

if(VISIT_ROOT)
	set(DAMARIS_VISIT_SRC
		${DAMARIS_SOURCE_DIR}/visit/VisItListener.cpp
		${DAMARIS_SOURCE_DIR}/data/Mesh.cpp
		${DAMARIS_SOURCE_DIR}/data/RectilinearMesh.cpp)
endif(VISIT_ROOT)

if(HDF5_ROOT)
  set(DAMARIS_PERSISTENCY_SRC
	${DAMARIS_PERSISTENCY_SRC}
	${DAMARIS_SOURCE_DIR}/hdf5/HDF5Writer.cpp)
endif(HDF5_ROOT)

add_library (damaris ${DAMARIS_COMMON_SRC}
			 ${DAMARIS_CLIENT_SRC}
		     ${DAMARIS_MODEL_SRC}
		     ${DAMARIS_SCRIPTS_SRC}
		     ${DAMARIS_PERSISTENCY_SRC}
			 ${DAMARIS_VISIT_SRC})
