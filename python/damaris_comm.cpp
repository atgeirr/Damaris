
#include <mpi.h>
#include <iostream>
#include <boost/python.hpp>
#include <mpi4py/mpi4py.h>

#include "damaris/env/Environment.hpp"

/**
This Python module allows access to the Damaris server MPI communicator (if being called from a Damaris server process)

>export PYTHONPATH=/home/jbowden/mypylib:$PYTHONPATH
>python
>>>from pydamaris import server_comm
>>>comm = server_comm()
>>> print(comm)
<mpi4py.MPI.Comm object at 0x7fe84ccc3210>
*/

namespace bp = boost::python;


static PyObject* hw_server_comm()
{

  PyObject* py_obj = PyMPIComm_New( damaris::Environment::GetEntityComm() );
  if (py_obj == NULL) bp::throw_error_already_set();
  return(py_obj) ;

}

BOOST_PYTHON_MODULE(pydamaris)
{
  Py_Initialize();
  if (import_mpi4py() < 0) return;

  bp::def("server_comm", hw_server_comm);
}

