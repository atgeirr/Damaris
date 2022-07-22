
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
  if (damaris::Environment::IsClient())
    return hw_damaris_comm() ;
  else {
    std::cout << "Error: GetClientComm() was not called from a client process!\n";
    bp::throw_error_already_set();
   }
   
   return NULL ;
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
    std::cout << "Error: GetServerComm() was not called from a server process!\n";
    bp::throw_error_already_set();
   }
   return NULL ;
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
static bp::object hw_is_client()
{
  return bp::object(damaris::Environment::IsClient());
}

/**
* Damaris Environment class wrapper
* Tells if the process is a server or not. If true then implies hw_is_client() is false.
*/
static bp::object hw_is_server()
{
  return bp::object(damaris::Environment::IsServer());
}

/**
* Damaris Environment class wrapper
* Tells if the process is a dedicated core or not. If true then implies hw_is_server() is true.
*/
static bp::object hw_is_dedicated_core()
{
  return bp::object(damaris::Environment::IsDedicatedCore());
}

/**
* Damaris Environment class wrapper
* Tells if the process is a dedicated node or not. If true then implies hw_is_server() is true.
*/
static bp::object hw_is_dedicated_node()
{
  return bp::object(damaris::Environment::IsDedicatedNode());
}

/**
* Damaris Environment class wrapper
* Returns the number of Damaris clients (mpi processes) per node. i.e. The number of simulation ranks per node.
*/
static bp::object hw_clients_per_node()
{
  return bp::object(damaris::Environment::ClientsPerNode());
}

/**
* Damaris Environment class wrapper
* Returns the number of Damaris cores (mpi processes) per node.
*/
static bp::object hw_cores_per_node()
{
  return bp::object(damaris::Environment::CoresPerNode());
}

/**
* Damaris Environment class wrapper
* Returns the number of Damaris server cores (ranks) being used per node
*/
static bp::object hw_servers_per_node()
{
  return bp::object(damaris::Environment::ServersPerNode());
}

/**
* Damaris Environment class wrapper
* Returns the number of nodes being used in the simulation
*/
static bp::object hw_number_of_nodes()
{
  return bp::object(damaris::Environment::NumberOfNodes());
}


/**
 * Returns the list of id of clients connected to the
 * dedicated core. In standalone mode, will return a list
 * with only the id of the calling client.
 */
static bp::object hw_list_known_clients()
{
  static const std::list<int> my_cpp_list = damaris::Environment::GetKnownLocalClients();
  static bp::list retlist ;
  for (auto c : my_cpp_list){
    retlist.append(c);
  }
  return bp::object(retlist);
}
    

static bp::object hw_damaris_initialize(boost::python::object xml_path, boost::python::object mpi4py_comm  )
{
  PyObject * mp_py_mpicomm = bp::extract<PyObject *>(mpi4py_comm) ;
  MPI_Comm passed_in_comm =   *PyMPIComm_Get(mp_py_mpicomm) ;
  std::string xml_std_str = bp::extract<std::string>(xml_path) ;
  int res ;
  res = damaris_initialize(xml_std_str.c_str(), passed_in_comm) ;
  bp::object res_bp(res) ;
  return ( res_bp );
}


static bp::object hw_damaris_magic_number( )
{
  return bp::object(damaris::Environment::GetMagicNumber()) ;
}

/* 
* Expose the functions as a Python module
*/
BOOST_PYTHON_MODULE(damaris4py)
{
  Py_Initialize();
  if (import_mpi4py() < 0) return;
  
  bp::def("damaris_initialize", hw_damaris_initialize);
  bp::def("getclientcomm", hw_damaris_comm_check_client);
  bp::def("getservercomm", hw_damaris_comm_check_server);
  bp::def("getdamariscomm", hw_damaris_comm);
  bp::def("getglobalcomm", hw_global_comm);
  bp::def("isclient", hw_is_client);
  bp::def("isserver", hw_is_server);
  bp::def("isdedicatedcore", hw_is_dedicated_core);
  bp::def("isdedicatednode", hw_is_dedicated_node);
  bp::def("clientspernode", hw_clients_per_node);
  bp::def("corespernode", hw_cores_per_node);
  bp::def("serverspernode", hw_servers_per_node);
  bp::def("numberofnodes", hw_number_of_nodes);
  bp::def("listknownclients", hw_list_known_clients);
  bp::def("magicnumber_string", hw_damaris_magic_number);
}

