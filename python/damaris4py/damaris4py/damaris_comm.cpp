
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
* i.e. If this function is called from a client process, it returns the communicator 
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
* Damaris Environment class wrapper 
* Returns the communicator among the clients (if called on a client processs) otherwise calls exception.
*/
static PyObject* hw_damaris_comm_check_client()
{
  if (damaris::Environment::isClient())
    return hw_damaris_comm() ;
  else {
    cout << "Error: GetClientComm() was not called from a client process!\n";
    bp::throw_error_already_set();
   }
}

/**
* Damaris Environment class wrapper 
* Returns the communicator among the clients (if called on a client processs) otherwise calls exception.
*/
static PyObject* hw_damaris_comm_check_server()
{
  if (damaris::Environment::IsServer())
    return hw_damaris_comm() ;
  else {
    cout << "Error: GetServerComm() was not called from a server process!\n";
    bp::throw_error_already_set();
   }
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

/**
* Damaris Environment class wrapper 
* Tells if the process is a client or not. If true then implies hw_is_server() is false.
*/
static PyObject* hw_is_client()
{
  return bp::object(damaris::Environment::isClient());
}

/**
* Damaris Environment class wrapper
* Tells if the process is a server or not. If true then implies hw_is_client() is false.
*/
static PyObject* hw_is_server()
{
  return bp::object(damaris::Environment::IsServer());
}

/**
* Damaris Environment class wrapper
* Tells if the process is a dedicated core or not. If true then implies hw_is_server() is true.
*/
static PyObject* hw_is_dedicated_core()
{
  return bp::object(damaris::Environment::IsDedicatedCore());
}

/**
* Damaris Environment class wrapper
* Tells if the process is a dedicated node or not. If true then implies hw_is_server() is true.
*/
static PyObject* hw_is_dedicated_node()
{
  return bp::object(damaris::Environment::IsDedicatedNode());
}

/**
* Damaris Environment class wrapper
* Returns the number of Damaris clients (mpi processes) per node. i.e. The number of simulation ranks per node.
*/
static PyObject* hw_clients_per_node()
{
  return bp::object(damaris::Environment::ClientsPerNode());
}

/**
* Damaris Environment class wrapper
* Returns the number of Damaris cores (mpi processes) per node.
*/
static PyObject* hw_cores_per_node()
{
  return bp::object(damaris::Environment::CoresPerNode());
}

/**
* Damaris Environment class wrapper
* Returns the number of Damaris server cores (ranks) being used per node
*/
static PyObject* hw_servers_per_node()
{
  return bp::object(damaris::Environment::ServersPerNode());
}

/**
* Damaris Environment class wrapper
* Returns the number of nodes being used in the simulation
*/
static PyObject* hw_number_of_nodes()
{
  return bp::object(damaris::Environment::NumberOfNodes());
}
/* 
* Expose the functions as a Python module
*/
BOOST_PYTHON_MODULE(pydamaris)
{
  Py_Initialize();
  if (import_mpi4py() < 0) return;

  bp::def("GetClientComm", hw_damaris_comm_check_client);
  bp::def("GetServerComm", hw_damaris_comm_check_server);
  bp::def("GetDamarisComm", hw_damaris_comm);
  bp::def("GetGlobalComm", hw_global_comm);
  bp::def("IsClient", hw_is_client);
  bp::def("IsServer", hw_is_server);
  bp::def("IsDedicatedCore", hw_is_dedicated_core);
  bp::def("IsDedicatedNode", hw_is_dedicated_node);
  bp::def("ClientsPerNode", hw_clients_per_node);
  bp::def("CoresPerNode", hw_cores_per_node);
  bp::def("ServersPerNode", hw_servers_per_node);
  bp::def("NumberOfNodes", hw_number_of_nodes);
}

