set(DAMARIS_CLIENT_SRC 
	${DAMARIS_SOURCE_DIR}/client/Client.cpp
	${DAMARIS_SOURCE_DIR}/client/ClientC.cpp
	${DAMARIS_SOURCE_DIR}/client/ClientFortran.cpp
	${DAMARIS_SOURCE_DIR}/client/Client.F90
	${DAMARIS_SOURCE_DIR}/client/StdAloneClient.cpp
	${DAMARIS_SOURCE_DIR}/client/Definition.cpp)

set(DAMARIS_MODEL_SRC
	${DAMARIS_SOURCE_DIR}/xml/Model.cpp
	${DAMARIS_SOURCE_DIR}/xml/BcastXML.cpp)

set(DAMARIS_COMMON_SRC
	${DAMARIS_SOURCE_DIR}/core/Process.cpp
	${DAMARIS_SOURCE_DIR}/core/ParameterManager.cpp
	${DAMARIS_SOURCE_DIR}/core/Environment.cpp
	${DAMARIS_SOURCE_DIR}/core/VariableManager.cpp
	${DAMARIS_SOURCE_DIR}/core/MeshManager.cpp
	${DAMARIS_SOURCE_DIR}/core/CurveManager.cpp
	${DAMARIS_SOURCE_DIR}/core/LayoutManager.cpp
	${DAMARIS_SOURCE_DIR}/core/ActionManager.cpp
	${DAMARIS_SOURCE_DIR}/core/Observable.cpp
	${DAMARIS_SOURCE_DIR}/core/Observer.cpp
	${DAMARIS_SOURCE_DIR}/core/ProcInfo.cpp
	${DAMARIS_SOURCE_DIR}/comm/SharedMessageQueue.cpp
	${DAMARIS_SOURCE_DIR}/comm/MPISendRecvQueue.cpp
	${DAMARIS_SOURCE_DIR}/memory/SharedMemorySegment.cpp
	${DAMARIS_SOURCE_DIR}/storage/StorageManager.cpp
        ${DAMARIS_SOURCE_DIR}/storage/SimpleWriter.cpp
        ${DAMARIS_SOURCE_DIR}/storage/SimpleReader.cpp    
	${DAMARIS_SOURCE_DIR}/event/Action.cpp
	${DAMARIS_SOURCE_DIR}/event/BoundAction.cpp
	${DAMARIS_SOURCE_DIR}/event/DynamicAction.cpp
	${DAMARIS_SOURCE_DIR}/event/ScriptAction.cpp
	${DAMARIS_SOURCE_DIR}/data/Variable.cpp
	${DAMARIS_SOURCE_DIR}/data/Mesh.cpp
	${DAMARIS_SOURCE_DIR}/data/Curve.cpp
	${DAMARIS_SOURCE_DIR}/data/RectilinearMesh.cpp
	${DAMARIS_SOURCE_DIR}/data/CurvilinearMesh.cpp
	${DAMARIS_SOURCE_DIR}/data/PointMesh.cpp
	${DAMARIS_SOURCE_DIR}/data/ChunkDescriptor.cpp
	${DAMARIS_SOURCE_DIR}/data/Chunk.cpp
	${DAMARIS_SOURCE_DIR}/data/Types.cpp
	${DAMARIS_SOURCE_DIR}/data/Layout.cpp
	${DAMARIS_SOURCE_DIR}/data/BoxLayout.cpp
	${DAMARIS_SOURCE_DIR}/data/TypeLayout.cpp
	${DAMARIS_SOURCE_DIR}/data/Parameter.cpp)


if(PYTHONLIBS_FOUND)
  set(DAMARIS_SCRIPTS_SRC
		${DAMARIS_SCRIPTS_SRC}
		${DAMARIS_SOURCE_DIR}/scripts/python/PyInterpreter.cpp
		${DAMARIS_SOURCE_DIR}/scripts/python/PyVariable.cpp
		${DAMARIS_SOURCE_DIR}/scripts/python/PyAction.cpp
		${DAMARIS_SOURCE_DIR}/scripts/python/PyChunk.cpp
		${DAMARIS_SOURCE_DIR}/scripts/python/PyLayout.cpp
		${DAMARIS_SOURCE_DIR}/scripts/python/PyTypes.cpp)
endif(PYTHONLIBS_FOUND)

if(VISIT_ROOT)
	set(DAMARIS_VISIT_SRC
		${DAMARIS_SOURCE_DIR}/visit/VisItListener.cpp)
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
