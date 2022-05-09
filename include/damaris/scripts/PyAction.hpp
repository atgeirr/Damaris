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
    std::string scheduler_file_ ;  //< The Dask scheduler file to read in and start dask-workers with
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
    int dask_scheduler_exists_ ;
    std::string dask_worker_name_ ;
    std::string nthreads_str_ ;  // default = "", openmp == use OMP_NUM_THREADS, "X" where x is an integer
    int nthreads_ ;  // the number of threads to launch per dask worker
    
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
     * Constructor. Initailizes Python and boost::numpy
     */
    PyAction(const model::Script& mdl)
        : Action(mdl.name()), Configurable<model::Script>(mdl)
    { 
        // name_     = mdl.name() ;
        language_       = mdl.language() ;
        file_           = mdl.file() ;
        frequency_      = mdl.frequency() ;
        scheduler_file_ = mdl.scheduler_file();
        nthreads_str_   = mdl.nthreads() ;
        std::transform(nthreads_str_.begin(),nthreads_str_.end(), nthreads_str_.begin(), 
                       [](unsigned char c){ return std::tolower(c); } );
         // 1 if file exists, 0 otherwise
        dask_scheduler_exists_ = DaskSchedulerFileExists(Environment::GetEntityComm(), scheduler_file_) ;
        dask_worker_name_ =  Environment::GetSimulationName() + "_" + Environment::GetMagicNumber() + "_" + std::to_string(Environment::GetEntityProcessID()) ;
        
        
        // Mess around trying to convert a string to a nthread integer
        if (nthreads_str_== std::string("")){
            nthreads_ = 1 ;
        } else if (nthreads_str_ == std::string("openmp")) {
#ifdef _OPENMP
          #pragma omp parallel
          nthreads_ = omp_get_num_threads() ;
#else
          nthreads_ = 1 ;
#endif
        } else {
            try {
              std::string::size_type sz; 
              nthreads_ = std::stoi (nthreads_str_,&sz);
            }
            catch (const std::invalid_argument& ia) {
	           std::cerr << "Invalid argument: " << ia.what() << '\n';
	           nthreads_ = 1 ;
            }       
       }
        
       // The initialisation is being done in Environment::Init
       // Py_Initialize();
       // np::initialize();
  
        /**
        * import the __main__ module and obtain the globals dict
        * assign to the bp::object 
        */
        main_     = bp::import("__main__");
        globals_  = main_.attr("__dict__");
      
        /**
        * String of Python code used to remove datasets from Pyhton environment when the 
        * Damris data they use  is invalidated/deleted
        */
        regex_string_with_python_code_ = "try :               \n"
                                         "  del DamarisData['REPLACE']   \n"
                                         "except KeyError as err:             \n"
                                         "  print('Damaris Server: KeyError could not delete key: ', err) \n" ;
                                         
        e_ = "\\b(REPLACE)([^ ]*)" ;
                                    
        locals_["DamarisData"] = damarisData_ ;
        
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
        
        regex_string_shutdown_dask_ = "from dask.distributed import Client\n"
                                "client = Client(scheduler_file='REPLACE')\n"
                                "client.shutdown()\n" ;
             
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
        regex_string_shutdown_dask_workers_ = "from dask.distributed import Client\n"
                                "client = Client(scheduler_file='REPLACE')\n"
                                "workers = client.scheduler_info()['workers']\n"
                                "shutdown_worker_dict = dict()\n"
                                "for worker in workers :\n"
                                "   if worker[1]['id'] == 'IDNAME' :\n"
                                "       shutdown_worker_dict[worker[0]] = worker[1]\n"                                        
                                "client.retire_workers(shutdown_worker_dict)\n" ;
                                 
        std::string REPLACE_STR =  scheduler_file_ + "$2" ;
        regex_string_shutdown_dask_workers_ = std::regex_replace(regex_string_shutdown_dask_workers_,this->e_,REPLACE_STR.c_str());
        REPLACE_STR =  dask_worker_name_  + "$2" ;
        std::regex id_rgex ; 
        id_rgex = "\\b(IDNAME)([^ ]*)" ;
        regex_string_shutdown_dask_workers_ = std::regex_replace(regex_string_shutdown_dask_workers_,id_rgex,REPLACE_STR.c_str());
        
        // std::cout <<"INFO: regex_string_shutdown_dask_workers_ :\n" << regex_string_shutdown_dask_workers_ << std::endl ;
                
                                              
        // Lunch the dask worker. One per didcated core
        if ((Environment::IsServer() == true) && (dask_scheduler_exists_ == 1)){
            int server_rank ;
            MPI_Comm_rank(Environment::GetEntityComm(), &server_rank) ;
            std::cout <<"INFO: Starting Dask Worker, server rank : " << server_rank << std::endl ;
            this->LaunchDaskWorker() ;
        }

    }
       
        
        
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
        std::cout <<"INFO: ~PyAction : In the destructor \n"  << std::endl ;
        if ((Environment::IsServer() == true) && (dask_scheduler_exists_ == 1))
        {  
            try {
                std::cout <<"INFO: ~PyAction : About to call bp::exec()  \n"  << std::endl ;
                bp::object result = bp::exec(regex_string_shutdown_dask_workers_.c_str(), this->globals_, this->locals_);  
            }  catch( bp::error_already_set &e) {
                std::string logString_del_daskworker("ERORR: PyAction::~PyAction() bp::exec() " ) ;
                logString_del_daskworker += this->extractException() ;
                std::cerr  << logString_del_daskworker << std::endl << std::flush ; 
                Environment::Log(logString_del_daskworker , EventLogger::Debug);
            }
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
