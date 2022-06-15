
#include <mpi.h>
#include <iostream>
#include <boost/python.hpp>
#include <mpi4py/mpi4py.h>

#include "damaris/env/Environment.hpp"

/**
This Python module allows access to the Damaris server MPI communicator (if being called from a Damaris server process)
```
    export PYTHONPATH=/home/jbowden/mypylib:$PYTHONPATH
    python
    >>>from pydamaris import damaris_comm
    >>>comm = damaris_comm()
    >>> print(comm)
    <mpi4py.MPI.Comm object at 0x7fe84ccc3210>
```
*/

namespace bp = boost::python;

/**
* Returns the Damaris MPI_Comm object that is specific to the current process
* i.e. If function is called from a client process, it returns the communicator 
*      among the clients, or, if it is is called from a Damaris server process, 
*      it returns the communicator of the server processes.
*      So, either communicator is returned by Environment::GetEntityComm()
*/
static PyObject* hw_damaris_comm()
{

  PyObject* py_obj = PyMPIComm_New( damaris::Environment::GetEntityComm() );
  if (py_obj == NULL) bp::throw_error_already_set();
  return(py_obj) ;

}

/**
* Damaris possibly re-orders the MPI_COMM_WORLD communicator and names it  _globalComm_
* which is returned by Environment::GetGlobalComm()
* N.B. Using the global communicator is not recommended from a simulation as only Damaris 
*      server processes are used to run Python scripts, so the damaris_comm() communicator 
*      should be used. 
*/
static PyObject* hw_global_comm()
{

  PyObject* py_obj = PyMPIComm_New( damaris::Environment::GetGlobalComm() );
  if (py_obj == NULL) bp::throw_error_already_set();
  return(py_obj) ;

}


/* 
* Expose the functions as a Python module
*/
BOOST_PYTHON_MODULE(pydamaris)
{
  Py_Initialize();
  if (import_mpi4py() < 0) return;

  bp::def("damaris_comm", hw_damaris_comm);
  bp::def("global_comm", hw_global_comm);
}

