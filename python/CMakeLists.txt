# Damaris CMakeLists.txt for the Python module
# 
# This module enables access to the Damaris sub-communicator used to communicate between 
# the Damris server rancks (per-node dedicated-cores or dedicated nodes) as a MPI4Py comm object.
# -DENABLE_PYTHONMODULE=1 needs to be added to the cmake caommand line.
cmake_minimum_required(VERSION 3.12)
 
 if(ENABLE_PYTHONMOD)
 
    if ( NOT ENABLE_PYTHON )
      find_package(Boost COMPONENTS python REQUIRED)
      find_package(Boost COMPONENTS numpy REQUIRED)
      find_package(PythonInterp 3)
      find_package(PythonLibs 3 REQUIRED)
    endif()
    
    # if(PythonInterp_FOUND)
        execute_process(
            COMMAND python -c "import mpi4py"
            RESULT_VARIABLE EXIT_CODE
            ERROR_QUIET
        ) 
        if (NOT ${EXIT_CODE} EQUAL 0)
            message(
                WARNING
                "The \"mpi4py\" Python3 package is not installed. Cannot build Damaris Python module. Please install mpi4py using the following command: \"MPICC=$(which mpicc) python -m pip install --no-cache-dir mpi4py\"."
            )
        endif()

         execute_process(
            COMMAND python -c "import mpi4py; print( mpi4py.get_include() )"
            OUTPUT_VARIABLE MPI4PY_INCLUDE_PATH
            ERROR_QUIET
        ) 

        string(REPLACE "\n" "" MPI4PY_INCLUDE_PATH ${MPI4PY_INCLUDE_PATH})
    # endif(PythonInterp_FOUND)
        message(
                STATUS
                "Found mpi4py: INCLUDE_PATH: ${MPI4PY_INCLUDE_PATH}" 
            )  


    add_library(pydamaris MODULE damaris_comm.cpp)
    set_target_properties(pydamaris PROPERTIES PREFIX "")


    # Set up the libraries and header search paths for this target
    target_include_directories(pydamaris PRIVATE damaris ${MPI4PY_INCLUDE_PATH} ${PYTHON_INCLUDE_DIRS})
    target_link_libraries(pydamaris damaris ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
    



    # configure_file(__init__.py ${CMAKE_CURRENT_BINARY_DIR}/python/__init__.py COPYONLY)
    # install(TARGETS pydamaris __init__.py DESTINATION "${PYTHON_INSTALL_PATH}")
     install(TARGETS pydamaris DESTINATION "${PYTHON_INSTALL_PATH}")
endif(ENABLE_PYTHONMOD)