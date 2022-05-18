/**************************************************************************
This file is part of Damaris.

Damaris is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Damaris is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Damaris.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/
#ifndef __DAMARIS_PYACTION_H
#define __DAMARIS_PYACTION_H

//#ifdef HAVE_PYTHON_ENABLED


#include "damaris/action/ScriptAction.hpp"
#include "data/VariableManager.hpp"
// #include "damaris/data/Variable.hpp"

#include <iostream>
#include <string>
#include <regex>
#include <iterator>
#include <stdexcept>

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/dict.hpp>

namespace bp = boost::python;
namespace np = boost::python::numpy;



namespace damaris {



 /**
 * PyAction describes an Action that wraps an interpretable Python script. 
 * The XML XSD definition that describes XSD model is as follows
    <xs:complexType name="Script">
        <xs:attribute name="name"      type="xs:string"  use="required"/>
        <xs:attribute name="file"      type="xs:string"  use="required"/>
        <xs:attribute name="execution" type="mdl:Exec"   use="optional" default="remote"/>
        <xs:attribute name="language"  type="mdl:Language"  use="required"/>
        <xs:attribute name="scope"     type="mdl:Scope"  use="optional" default="core"/>
        <xs:attribute name="external"  type="xs:boolean" use="optional" default="false"/>
        <xs:attribute name="frequency" type="xs:unsignedInt" use="optional" default="1" />
    </xs:complexType>
    
  This class heavily makes use of Boost::Python and Boost:Numpy
*/   
class PyAction : public Action, public Configurable<model::Script> {
    
    //void Output(int32_t iteration);
    friend class Deleter<PyAction>;
    
  private:
    // Data obtained from the XML model file
    //std::string name_ ;  We get this from inheriting from Action
    std::string language_ ;
    std::string file_ ;    
    unsigned int frequency_ ;
    
    /**
    * The dictionary that holds the Python environment
    */
    bp::dict locals_;
    /**
    * The dictionary that wil store the created numpy arrays that 
    * wrap the Damaris data. The dictionary will be added to the Python 
    * local dictionary and be named DamarisData['varname_is_key_<type>_<iteration>']
    */
    bp::dict damarisData_;
    
    /**
    * used to hold the __main__ module and obtain the globals dict
    */
    bp::object main_;
    
    /**
    * used to hold the globals dict which is obtained from the __main__ module
    */
    bp::object globals_;
    
    
    /**
    * String of Python code used to remove datasets from Pyhton environment when the 
    * Damris data they use  is invalidated/deleted
    */
    std::string regex_string_with_python_code_; 
    
    /**
    * String for checking all workers are available. 
    * We need to replace:
    *    REPLACE with scheduler_file_
    *  and 
    *    NWORKERS by the expected number of workers (== Damaris server cores)
    */
    std::string regex_string_dask_worker_available_ ;
    std::string regex_check_scheduler_exists_ ;
    std::string scheduler_file_ ;  //< The Dask scheduler file to read in and start dask-workers with
    std::string dask_timeout_ ;
    int         dask_scheduler_exists_ ;
    std::string dask_worker_name_ ;    
    int         dask_nthreads_ ;      // the number of threads to launch per dask worker
    bool        dask_keep_workers_ ;  // if true, then do not destroy the dask workers (simulation will not return until they are destroyed)
     
    /**
    * String of Python code used to shutdown the Dask scheduler and all workers 
    * that corresponds to the scheduler_file_
    * 
    */
    std::string regex_string_shutdown_dask_; 
    
    /**
    * String of Python code used to shutdown the Dask workers *by name*
    * that corresponds to the current scheduler_file_. THe name given means
    * the workers that were launched by the current Damaris simulation are
    * shut down only.
    * 
    */
    std::string regex_string_shutdown_dask_workers_ ;
    
    std::regex e_  ;
    
  protected:
    /**
     * Constructor. Initailizes Python and boost::numpy only in the Server cores/nodes
     */
    PyAction(const model::Script& mdl)
        : Action(mdl.name()), Configurable<model::Script>(mdl)
    {
        std::string dask_nthreads_str ;      // default = "", openmp == use OMP_NUM_THREADS, "X" where x is an integer
        std::string dask_keep_workers_str ;  // default = "no"
        
        language_             = mdl.language() ;
        file_                 = mdl.file() ;
        frequency_            = mdl.frequency() ;
        scheduler_file_       = mdl.scheduler_file();
        dask_timeout_         = mdl.timeout() ;
        dask_nthreads_str     = mdl.nthreads() ;
        dask_keep_workers_str = mdl.keep_workers() ;
        
       /** This initialisation is being done in Environment::Init
       * Py_Initialize();
       * np::initialize();
       */
    
        /**
        * import the __main__ module and obtain the globals dict
        * assign to the bp::object 
        */
        main_     = bp::import("__main__");
        globals_  = main_.attr("__dict__");
      
         /**
        * String of Python code used to test if scheduler is accessible
        * If the Client cannot be created then possibly the scheduler is not available
        */
        regex_check_scheduler_exists_ = "from dask.distributed import Client\n"
                                        "client = Client(scheduler_file='REPLACE', timeout='TIMEOUTs')\n" 
                                        "client.close()\n" ;                                
        /**
         * regex replace the scheduler_file string and then the timeout
        */                              
        e_ = "\\b(REPLACE)([^ ]*)" ;                 
        std::string REPLACE_STR =  scheduler_file_ + "$2" ;
        regex_check_scheduler_exists_ = std::regex_replace(regex_check_scheduler_exists_,this->e_,REPLACE_STR.c_str());
        
        // test that the timeout string is a valid integer (default in Model.xsd is "2")
        try {
            std::string::size_type sz; 
            int tmp_int = std::stoi (dask_timeout_,&sz);
            Environment::Log("PyAction:PyAction() The Dask timeout for client connection was set to "+dask_timeout_+" seconds" , EventLogger::Debug);
        }
        catch (const std::invalid_argument& ia) {
            std::cerr << "PyAction:PyAction() Invalid argument: " << ia.what() << '\n';
            dask_timeout_ = std::string("2");
        }   
        e_ = "\\b(TIMEOUT)([^ ]*)" ;                                
        REPLACE_STR =  dask_timeout_ + "$2" ;
        regex_check_scheduler_exists_ = std::regex_replace(regex_check_scheduler_exists_,this->e_,REPLACE_STR.c_str());
        
        /** 
        * 1 if scheduler file exists, 
        * 0 otherwise. Only tests if the scheduler file exists, 
        * not that the Scheduler is accessible via Python.
        */
        dask_scheduler_exists_ = DaskSchedulerFileExists(Environment::GetEntityComm(), scheduler_file_) ;
        dask_worker_name_      =  Environment::GetSimulationName() + "_" + Environment::GetMagicNumber() + "_" + std::to_string(Environment::GetEntityProcessID()) ;
        
        
        // Mess around trying to convert a string to a nthread integer
        std::transform(dask_nthreads_str.begin(),dask_nthreads_str.end(), dask_nthreads_str.begin(), 
                       [](unsigned char c){ return std::tolower(c); } );        
        if (dask_nthreads_str== std::string("")){
            dask_nthreads_ = 1 ;
        } else if (dask_nthreads_str == std::string("openmp")) {
#ifdef _OPENMP
          #pragma omp parallel
          dask_nthreads_ = omp_get_num_threads() ;
#else
          dask_nthreads_ = 1 ;
#endif
        } else {
            // Convert string to integer
            try {
              std::string::size_type sz; 
              dask_nthreads_ = std::stoi (dask_nthreads_str,&sz);
            }
            catch (const std::invalid_argument& ia) {
	           std::cerr << "PyAction:PyAction() Invalid argument: " << ia.what() << '\n';
	           dask_nthreads_ = 1 ;
            }       
       }
       
       // set string to lower case
       std::transform(dask_keep_workers_str.begin(),dask_keep_workers_str.end(), dask_keep_workers_str.begin(), 
                       [](unsigned char c){ return std::tolower(c); } );
       
       /** dask_keep_workers_ is tested for in ~PyAction destructor. If true then workers are not destroyed
       *  and the simulation will not be able to shutdown. This allows a user to continuie to use the Dask
       *  setup to query and process data interactively, if need be.
       */
       if (dask_keep_workers_str == std::string("no"))
       {
            dask_keep_workers_ = false ;
       } else {
            dask_keep_workers_ = true ;
       }
                                          
       
        // This dictionary is placed in the Python interpreter and contains data that
        // can be accessed through the script
        locals_["DamarisData"] = damarisData_ ;
        
        
        // Define Python lists for some usefull metadata to be pushed to Python
        bp::dict dask_dict ;
        bp::dict damaris_dict ;    
        
        // Add some usefull Dask information
        if (this->dask_scheduler_exists_ == 1 ) {
            dask_dict["dask_scheduler_file"]     = this->scheduler_file_ ;
        } else {
            dask_dict["dask_scheduler_file"]     = std::string("") ;
        }
        
        dask_dict["dask_workers_name"]           = this->dask_worker_name_ ;
        // N.B. this will **not be correct** for Dedicated Node mode
        dask_dict["dask_nworkers"]               = Environment::ServersPerNode() ; // _serversPerNode_  ;
        dask_dict["dask_threads_per_worker"]     = this->dask_nthreads_ ; 
        
        // Add usefull Damaris properties. These are the Damaris Environment properties
        damaris_dict["is_dedicated_node"]        = Environment::IsDedicatedNode() ; // _isDedicatedNode_
        damaris_dict["is_dedicated_core"]        = Environment::IsDedicatedCore() ; // _isDedicatedCore_
        damaris_dict["servers_per_node"]         = Environment::ServersPerNode() ; 
        damaris_dict["clients_per_node"]         = Environment::ClientsPerNode() ;
        damaris_dict["ranks_per_node"]           = Environment::CoresPerNode() ;
        damaris_dict["cores_per_node"]           = Environment::CoresPerNode() ;
        damaris_dict["number_of_nodes"]          = Environment::NumberOfNodes() ;
        damaris_dict["simulation_name"]          = Environment::GetSimulationName() ;
        damaris_dict["simulation_magic_number"]  = Environment::GetMagicNumber() ;  // this is a reference to a string. I hope that is fine.
        
        // These are the two 'static' dictionaries
        // There will be another dictionary containing iteration data
        damarisData_["dask_env"]              =  dask_dict ;
        damarisData_["damaris_env"]           =  damaris_dict ;
        /**
         * String for waiting until all workers are available. 
         * We need to replace:
         *    REPLACE with scheduler_file_
         *  and 
         *    NWORKERS by the expected number of workers (== number of Damaris server cores)
         */
        regex_string_dask_worker_available_ = "from dask.distributed import Client\n"
                                              "client = Client(scheduler_file='REPLACE')\n"
                                              "while ((client.status == 'running'') and (len(client.scheduler_info()['workers']) < NWORKERS)):\n"
                                              "  sleep(1.0)\n" ;
        
        /*regex_string_shutdown_dask_ = "from dask.distributed import Client\n"
                                      "client =  Client(scheduler_file='REPLACE')\n"
                                      "client.shutdown()\n" ;*/
         
        /**
        * String of Python code used to remove datasets from Python environment when the 
        * Damaris data they use  is invalidated/deleted
        */
       /* regex_string_with_python_code_ = "try :               \n"
                                         "  del DamarisData['REPLACE']   \n"
                                         "except KeyError as err:             \n"
                                         "  print('Damaris Server: KeyError could not delete key: ', err) \n" ;
        */
       /**
        * String of Python code used to remove datasets from Python environment when the 
        * Damaris data they use  is invalidated/deleted at the end of an iteration
        */
        regex_string_with_python_code_ = "import gc\n"
                                         "try :\n"
                                         "  del DamarisData['iteration_data']\n"
                                         "  gc.collect()\n"
                                         "except KeyError as err:\n"
                                         "  print('Damaris Server: KeyError could not delete key: ', err)\n" ;
        
         
        /**
         * String for removing workers from the worker pool. Workers is a dict of dicts, 
         * the first index of which is the protocol + ip address:port of the worker.
         * 
         * We need to replace:
         *    REPLACE with scheduler_file_
         *  and 
         *    IDNAME by the expected name of the worker (dask_worker_name_)
         * 
         * N.B. The simulation will not end if there are workers that were launched 
         *      by it still running. This will not shut down the scheduler.
         */                         
        regex_string_shutdown_dask_workers_="from dask.distributed import Client\n"
                                            "client = Client(scheduler_file='REPLACE')\n"
                                            "workers = client.scheduler_info()['workers']\n"
                                            "shutdown_worker_dict = dict()\n"
                                            "id_name_list = ['IDNAME']\n"
                                            "for worker in workers.items() :\n"
                                            "  if worker[1]['id'] in id_name_list:\n"
                                            "    shutdown_worker_dict[worker[0]] = worker[1]\n" 
                                            "client.retire_workers(shutdown_worker_dict)\n" ; 

        e_ = "\\b(REPLACE)([^ ]*)" ;                          
        REPLACE_STR =  scheduler_file_ + "$2" ;
        regex_string_shutdown_dask_workers_ = std::regex_replace(regex_string_shutdown_dask_workers_,this->e_,REPLACE_STR.c_str());
        REPLACE_STR =  dask_worker_name_  + "$2" ;
        std::regex id_rgex ; 
        id_rgex = "\\b(IDNAME)([^ ]*)" ;
        regex_string_shutdown_dask_workers_ = std::regex_replace(regex_string_shutdown_dask_workers_,id_rgex,REPLACE_STR.c_str());
        
        // std::cout <<"INFO: regex_string_shutdown_dask_workers_ :\n" << regex_string_shutdown_dask_workers_ << std::endl ;
                
                                              
        // Lunch the dask worker. One per didcated core
        // The test for IsServer() may not be required as this class is only creaed by Server cores/nodes
        if ((Environment::IsServer() == true) && (dask_scheduler_exists_ == 1)){
            int server_rank ;
            MPI_Comm_rank(Environment::GetEntityComm(), &server_rank) ;
            std::cout <<"INFO: PyAction::PyAction() Starting Dask Worker, server rank : " << server_rank << std::endl ;
            this->LaunchDaskWorker() ;
        }

    }
       
    /**
     *  Tidy up exception catching code in catch {} blocks. Prints to std::cerr and Environment::Log()
     */
    void CatchPrintAndLogPyException(std::string MessageStr ) ;    
    
    /**
     *  Very special Python API code to get an excption from Python and print it in C++
     */   
    std::string extractException() ;
    
    
    /**
     * 
     * Tests for file on rank 0 of the given communicator and broacasts result to other processes.
     * 
     * Returns 1 if the dask-scheduler file exists, 0 otherwise. 
     */
    int  DaskSchedulerFileExists(const MPI_Comm& comm, std::string filename) ;
     
     
    /**
    * Uses boost::Numpy data types to convert a Damaris variable 
    * (as given in the XML model) to a numpy data type.
    */
    np::dtype GetNumPyType(model::Type mdlType);
    // bool GetNumPyType(model::Type mdlType, np::dtype &dt);
     
     
    /**
    * Retruns a string indicating the type of the variable data
    */
     std::string GetTypeString(model::Type mdlType);


    /**
    * Creates a name to be used as key in a Python dictionary to reference the data 
    *  within a Python script.
    *  The string is a concatenation of the variable name (defined in XML file) and adds:
    *  - A block string "_Px", where x is the Damaris client global rank of the 
    *    source of the data
    *  - And, if the data is multi-domain (as set in XML file) then adds:
    *    A domain string "_By" where y is the domain ID
    * 
    */
    std::string GetVariableFullName(std::shared_ptr<Variable> v , std::shared_ptr<Block> *b);


    /**
    * Uses boost::Numpy data types to convert a Damaris variable 
    * (as given in the XML model) to a numpy data type.
    */
    bool PassDataToPython(int iteration ); 
    

    /**
    * Executes a dask-worker in a sub-shell, specifying the dask-scheduler configuration file that is 
    * set in the Dmaaris XML <pyscript ... scheduler-file=""> tag and 
    * 
    * Returns the return value from the std::system command
    */ 
    int  LaunchDaskWorker() ;

    /**
     * Destructor.
     */
    ~PyAction() 
    {
        if ((dask_keep_workers_ == false) && (dask_scheduler_exists_ == 1)) 
        {
            // This test for IsServer() may not be required as this class is only creaed by Server cores/nodes
            if (Environment::IsServer() == true)
            {  
                try 
                {                    
                    Environment::Log("Calling: PyAction::~PyAction() bp::exec() to shut down Dask workers" , EventLogger::Debug);
                    bp::object result = bp::exec(regex_string_shutdown_dask_workers_.c_str(), this->globals_, this->locals_);  
                }  catch( bp::error_already_set &e) {
                    CatchPrintAndLogPyException("ERORR: PyAction::~PyAction() bp::exec() " ) ;                
                }
            }       
        } else {
            Environment::Log("Calling: PyAction::~PyAction() Not removing Dask workers" , EventLogger::Debug);
        }
    }

    public:    
    
    /**
     * \see damaris::Action::operator()
     */
    virtual  void Call(int32_t sourceID, int32_t iteration,
                const char* args = NULL) {
        
        if (iteration % frequency_ == 0){
            PassDataToPython( iteration ); 
        }
        
    }


    /**
     * Tells if the action can be called from outside the simulation.
     */
     virtual  bool IsExternallyVisible() const final  { 
        return GetModel().external(); 
    }

    /**
     * \see Action::GetExecLocation
     */
   virtual   model::Exec GetExecLocation() const final   {
        return GetModel().execution();
    }

    /**
     * \see Action::GetScope
     */
   virtual   model::Scope GetScope() const final {
        return GetModel().scope();
    }

    /**
     * Creates a new instance of an inherited class of PyAction 
     * according to the "language" field in the description. 
     * This will be called from the ScriptManager object
     */
    template<typename SUPER>
    static std::shared_ptr<SUPER> New(const model::Script& mdl, const std::string name) {
        
         return std::shared_ptr<SUPER>(new PyAction(mdl), Deleter<PyAction>());
    
    }

    

   np::ndarray   ReturnNpNdarray(int blockDimension, int * localDims, void *np_ptr, model::Type mdlType) {
         //;
         
        if (mdlType.compare("int") == 0) {
            int mytype= 0 ;
           return ( ReturnNpNdarrayTyped<int>( blockDimension, localDims, np_ptr, mdlType, mytype));
        } else if (mdlType.compare("float") == 0) {
            float mytype= 0 ;
            return ( ReturnNpNdarrayTyped<float>(blockDimension, localDims, np_ptr, mdlType, mytype));
        } else if (mdlType.compare("real") == 0) {
            float mytype= 0 ;
            return ( ReturnNpNdarrayTyped<float>(blockDimension, localDims, np_ptr, mdlType, mytype));
        } else if (mdlType.compare("integer") == 0) {
            int mytype= 0 ;
            return ( ReturnNpNdarrayTyped<int>(blockDimension, localDims, np_ptr, mdlType,  mytype));
        } else if (mdlType.compare("double") == 0) {
            double mytype= 0 ;
            return ( ReturnNpNdarrayTyped<double>(blockDimension, localDims, np_ptr, mdlType,mytype));
        } else if (mdlType.compare("long") == 0) {
            long mytype= 0 ;
           return (  ReturnNpNdarrayTyped<long int>(blockDimension, localDims, np_ptr, mdlType,  mytype));
        } else if (mdlType.compare("short") == 0) {
            short mytype= 0 ;
            return ( ReturnNpNdarrayTyped<short>(blockDimension, localDims, np_ptr, mdlType,  mytype));
        } else if (mdlType.compare("char") == 0) {
            char mytype= 0 ;
            return ( ReturnNpNdarrayTyped<char>(blockDimension, localDims, np_ptr, mdlType, mytype));
        } else if (mdlType.compare("character") == 0) {
            char mytype= 0 ;
             return (ReturnNpNdarrayTyped<char>(blockDimension, localDims, np_ptr, mdlType,  mytype));
        } else {
             std::cerr << "ERROR: PyAction::ReturnNpNdarray(): no matching type supported found" ;
        }
        //return ( np::ndarray mul_data_ex ) ;
    }

    
   template <typename T>
     np::ndarray   ReturnNpNdarrayTyped(int blockDimension, int * localDims, void *np_ptr, model::Type mdlType,   T mytype) {
        bp::object own_local = bp::object() ;
        // np::ndarray   retarray ;
        if (blockDimension == 1) {
            return( np::from_data( static_cast<const T *>( np_ptr ), GetNumPyType(mdlType) ,
                                bp::make_tuple(localDims[0]),
                                bp::make_tuple(sizeof(T)),
                                own_local));
        } else if (blockDimension == 2 ) {
            return(  np::from_data( static_cast<const T *>( np_ptr ), GetNumPyType(mdlType) ,
                                bp::make_tuple(localDims[0],localDims[1]),
                                bp::make_tuple(sizeof(T)*localDims[2],sizeof(T)),
                                own_local));
        } else if (blockDimension == 3 ) {
            return(  np::from_data( static_cast<const T *>( np_ptr ), GetNumPyType(mdlType) ,
                                bp::make_tuple(localDims[0],localDims[1],localDims[2]),
                                bp::make_tuple(sizeof(T)*localDims[2]*localDims[1],sizeof(T)*localDims[2],sizeof(T)),
                                own_local));
        } else if (blockDimension == 4 ) {
           return(  np::from_data( static_cast<const T *>( np_ptr ), GetNumPyType(mdlType) ,
                                bp::make_tuple(localDims[0],localDims[1],localDims[2],localDims[3]),
                                bp::make_tuple(sizeof(T)*localDims[3]*localDims[2]*localDims[1],sizeof(T)*localDims[2]*localDims[1],sizeof(T)*localDims[2],sizeof(T)),
                                own_local));
        } else if (blockDimension == 5 ) {
            return(  np::from_data( static_cast<const T *>( np_ptr ), GetNumPyType(mdlType) ,
                                bp::make_tuple(localDims[0],localDims[1],localDims[2],localDims[3],localDims[4]),
                                bp::make_tuple(sizeof(T)*localDims[4]*localDims[3]*localDims[2]*localDims[1],sizeof(T)*localDims[3]*localDims[2]*localDims[1],sizeof(T)*localDims[2]*localDims[1],sizeof(T)*localDims[2],sizeof(T)),
                                own_local));
        } else if (blockDimension >= 6 ) {
            std::cerr << "ERROR: PyAction::ReturnNpNdarrayTyped(): Damaris Python integration currently can parse a maximum of 5 dimensional NumPy array.\n"<< "The array given has dimensions of : " << blockDimension << std::endl << std::flush ; 
        }
        //return ( np::ndarray mul_data_ex ) ;
    }


};

     
}

//#endif  // HAVE_PYTHON_ENABLED
#endif
