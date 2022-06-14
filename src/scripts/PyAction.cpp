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
// #ifdef HAVE_PYTHON_ENABLED


#include <sstream>
#include <tuple>
#include <cstdlib>  // for std::system()
#include <fstream> 
#include "util/Debug.hpp"
#include "scripts/PyAction.hpp"


// namespace damaris {
//     template <typename T>
//     struct StaticCast
//     {
//         template <typename U>
//         T operator()(const U& rhs)
//         {
//             return static_cast<const T>(rhs);
//         }
//     };
// }
//     
// #include "damaris_data.hpp"


/*
  * The XSD model is something like this:
  * <!-- Script node -->
    <xs:complexType name="Script">
        <xs:attribute name="name"      type="xs:string"  use="required"/>
        <xs:attribute name="file"      type="xs:string"  use="required"/>
        <xs:attribute name="execution" type="mdl:Exec"   use="optional" default="remote"/>
        <xs:attribute name="language"  type="mdl:Language"  use="required"/>
        <xs:attribute name="scope"     type="mdl:Scope"  use="optional" default="core"/>
        <xs:attribute name="external"  type="xs:boolean" use="optional" default="false"/>
        <xs:attribute name="frequency" type="xs:unsignedInt" use="optional" default="1" />
    </xs:complexType>
  * 
  * */

namespace bp = boost::python;
namespace np = boost::python::numpy;


namespace damaris {

    
    
    
//     void PyAction::Call(int32_t sourceID, int32_t iteration, const char* args) {
//         
//         if (iteration % frequency_ == 0){
//             PassDataToPython( iteration ); 
//         }
//         
//     }

    
    np::dtype PyAction::GetNumPyType(model::Type mdlType) {
        // np::dtype &dt
        if (mdlType.compare("int") == 0) {
            return(np::dtype::get_builtin<int>());
        } else if (mdlType.compare("float") == 0) {
            return(np::dtype::get_builtin<float>());
        } else if (mdlType.compare("real") == 0) {
            return(np::dtype::get_builtin<float>());
        } else if (mdlType.compare("integer") == 0) {
            return(np::dtype::get_builtin<int>());
        } else if (mdlType.compare("double") == 0) {
            return(np::dtype::get_builtin<double>());
        } else if (mdlType.compare("long") == 0) {
            return(np::dtype::get_builtin<long int>());
        } else if (mdlType.compare("short") == 0) {
            return(np::dtype::get_builtin<short>());
        } else if (mdlType.compare("char") == 0) {
            return(np::dtype::get_builtin<char>());
        } else if (mdlType.compare("character") == 0) {
            return(np::dtype::get_builtin<char>());
        } else {
             std::cerr << "PyAction::GetNumPyType() no matching type supported found" ;
        }

    }
    
    
    std::string PyAction::GetTypeString(model::Type mdlType) {
        std::string retstr ;
        if (mdlType.compare("int") == 0) {
            retstr = "_int";
        } else if (mdlType.compare("float") == 0) {
            retstr = "_float";
        } else if (mdlType.compare("real") == 0) {
            retstr = "_float";
        } else if (mdlType.compare("integer") == 0) {
            retstr = "_int";
        } else if (mdlType.compare("double") == 0) {
            retstr = "_double";
        } else if (mdlType.compare("long") == 0) {
            retstr = "_longint";
        } else if (mdlType.compare("short") == 0) {
            retstr = "_short";
        } else if (mdlType.compare("char") == 0) {
            retstr = "_char";
        } else if (mdlType.compare("character") == 0) {
            retstr = "_char";
        } else {
            return "" ;
        }
        return retstr;
    }
    
    

           
    
    std::string PyAction::GetVariableFullName(std::shared_ptr<Variable> v , std::shared_ptr<Block> *b){
        std::stringstream varName;
        std::string baseName;
        std::string typestr ;
        int numDomains;

        baseName = Environment::GetSimulationName();
        numDomains = Environment::NumDomainsPerClient();
    
        
        typestr = GetTypeString(v->GetLayout()->GetType()) ; // return type string is prefixed by "_"
        
        if (numDomains == 1){
            varName << v->GetName() << typestr << "_P" << (*b)->GetSource(); // e.g. varName_P2
        } else {// more than one domain
            varName << v->GetName() << typestr << "_P" << (*b)->GetSource() << "_B" << (*b)->GetID(); // e.g. varName_P2_B3
        }

        return  varName.str();
    }
    

    void PyAction::CatchPrintAndLogPyException(std::string MessageStr ) {
                    MessageStr += this->extractException() ;
                    std::cerr  << MessageStr << std::endl << std::flush ; 
                    Environment::Log(MessageStr , EventLogger::Debug);
    }
    
    
    // from wiki.python.org/moin/boost.python/EmbeddingPython
    std::string PyAction::extractException() 
    {
        // using namespace boost::python;

        PyObject *exc,*val,*tb;
        PyErr_Fetch(&exc,&val,&tb);
        PyErr_NormalizeException(&exc,&val,&tb);
        bp::handle<> hexc(exc),hval(bp::allow_null(val)),htb(bp::allow_null(tb));
        if(!hval)
        {
            return bp::extract<std::string>(bp::str(hexc));
        }
        else
        { 
            bp::object traceback(bp::import("traceback"));
            bp::object format_exception(traceback.attr("format_exception"));
            bp::object formatted_list(format_exception(hexc,hval,htb));
            bp::object formatted(bp::str("").join(formatted_list));
            return bp::extract<std::string>(formatted);
        }
    }
    
    
    
    int  PyAction::LaunchDaskWorker() 
    {
        std::string launch_worker("dask-worker --scheduler-file ") ;
        std::cout << std::flush ;
        // launch_worker += scheduler_file_ + " --name " + dask_worker_name_ +  std::string(" &") ;
        launch_worker += scheduler_file_ + " --name " + dask_worker_name_ +    std::string(" --nthreads ") + std::to_string(dask_nthreads_) + std::string("  &") ;
        std::cout <<"INFO: Starting Dask Worker, calling : " << launch_worker << std::endl ;
        Environment::Log(launch_worker , EventLogger::Debug);
        int ret = std::system(launch_worker.c_str()) ;
        return (ret) ;
    }
    
    

    
    int  PyAction::DaskSchedulerFileExists(const MPI_Comm& comm, std::string filename) 
    {
        int rank;
        MPI_Comm_rank(comm,&rank);
        std::string dask_file_exists("INFO: Scheduler file ") ;
        dask_file_exists += filename ;
        int retint = 0 ;
        if (rank == 0) {
            std::ifstream daskschedfile;
            daskschedfile.open(filename.c_str()); 
            if(! daskschedfile.fail()) {
                std::cout  << "INFO: scheduler file found : " << filename << std::endl << std::flush ;
                daskschedfile.close();
                retint = 1 ;
            }
            // Now test to see if it is a valid Dask scheduler
            if (retint == 1) {
                try {
                   std::string test_scheduler_str("INFO: PyAction::DaskSchedulerFileExists(): Calling bp::exec() on the following code to test that scheduler works : ") ;
                   // std::cout  << test_scheduler_str  << filename << std::endl << std::flush ;
                   Environment::Log(test_scheduler_str , EventLogger::Debug);
                   Environment::Log(regex_check_scheduler_exists_ , EventLogger::Debug);
                   bp::object result = bp::exec(regex_check_scheduler_exists_.c_str(), this->globals_, this->locals_);  
                }  
                catch( bp::error_already_set &e) 
                {
                    
                    CatchPrintAndLogPyException("ERROR: PyAction::DaskSchedulerFileExists() bp::exec() Test attaching to Dask Scheduler. " ) ;                    
                    retint = 0 ;
                }
                
            }
            
            MPI_Bcast(&retint,1,MPI_INT,0,comm);
        } else {
            
            MPI_Bcast(&retint,1,MPI_INT,0,comm);  // recieve the value from rank 0
        }
        
        if (retint == 1) {
            dask_file_exists += " Exists and is a valid scheduler file. Damaris server cores will launch dask-workers." ;
        } else {
            dask_file_exists += " Does not exist or is not a valid scheduler file. Check Damaris XML file <pyscript> tag for scheduler-file value, or when you launch a dask-scheduler use: --schedule-file " + filename ; 
        }
        
        Environment::Log(dask_file_exists , EventLogger::Debug);
        
        return (retint ) ;
    }

  
    
    bool PyAction::PassDataToPython(int iteration )
    {
        std::stringstream varName ;
        BlocksByIteration::iterator begin;
        BlocksByIteration::iterator end;
        
        // Each variable will be added to this dictionary (by variable name)
        // and will contain meta-data about multiple blocks
        bp::dict iteration_dict ;
        iteration_dict["iteration"] = iteration ;
        // for each variable ... (unlike HDF5 storage, which can have a <variable ... store="" /> attribute 
        // VariableManager::iterator w = VariableManager::Begin();
        
        std::vector<std::weak_ptr<Variable> >::const_iterator w = GetVariables().begin();
        //for(; w != VariableManager::End(); w++) {
        //  std::shared_ptr<Variable> v = (*w) ; // ->lock()) ; // ->lock();
        
        // for selected variables ... (like HDF5 storage, we can can have a <variable ... script="MyScript" /> attribute 
        
        for (; w != GetVariables().end(); w++) {
            // A (sub) dictionary for each variable

            // damaris_variable_dict will be accessible by the Damaris variable name from the iteration_dict 
            // which is equivalent to the DD['iteration_data'] dictionary if access via Python
            bp::dict damaris_variable_dict ;  
            std::shared_ptr<Variable> v = w->lock();
            // non TimeVarying variables only are written in the first iteration. 
            //if ((not v->get()->IsTimeVarying()) && (iteration > 0))
            if ((not v->IsTimeVarying()) && (iteration > 0))
                continue;

            // Getting the dimensions of the variable
            int varDimention;
            // varDimention = v->get()->GetLayout()->GetDimensions();
            varDimention = v->GetLayout()->GetDimensions();

            // Create a array for dimensions
            // int *globalDims;
            int *localDims;
            int *localOffset; // equivalent to memOffset aray in HDF5Store
            // globalDims = new (std::nothrow) int[varDimention];
            localDims = new (std::nothrow)  int[varDimention];
            localOffset = new (std::nothrow) int[varDimention]; 
            //memDim = new (std::nothrow) hsize_t[varDimention];

            // (globalDims == NULL) || 
            if (localDims == NULL) {
                ERROR("in PyAction::PassDataToPython(): Failed to allocate memory for localDims arrays!");
            }
            if (localOffset == NULL) {
                ERROR("in PyAction::PassDataToPython(): Failed to allocate memory for localOffset arrays!");
            }
 
            v->GetBlocksByIteration(iteration, begin, end);

            // Define Python lists for some usefull metadata to be pushed to Python
            bp::list block_list ;   // each client produces a seperate 'block' of a variable
            bp::list blockid_list ; // Variables can also have multiple 'domains', which translates to multiple blocks and 
                                    // is data written in multiple chunks on a single damaris interation
            bp::dict variable_data ;  // the numpy arrays - one for each block + domain
            for (BlocksByIteration::iterator bid = begin; bid != end; bid++) {
                std::shared_ptr<Block> b = *bid;

                // Obtain block array number of dimension
                int blockDimension = b->GetDimensions();
                varName << "P" << b->GetSource() << "_B" << b->GetID();
            
                // Obtain the block size
                // Numpy uses C storage conventions, assuming that the last listed
                // dimension is the fastest-changing dimension and the first-listed
                // dimension is the slowest changing.
                std::string logString_localDims   ("PyAction::PassDataToPython() numpy array dims    ['iteration_data']['") ;
                std::string logString_localOffsets("PyAction::PassDataToPython() numpy array offsets ['iteration_data']['") ; 
                logString_localDims    += v->GetName() + "']['numpy_data']" + "[ " + varName.str() + "]" ;
                logString_localOffsets += v->GetName() + "']['numpy_data']" + "[ " + varName.str() + "]" ;
                for (int i = 0 ; i < blockDimension ; i++) {
                    localOffset[i] = b->GetStartIndex(i);
                    localDims[i]   = b->GetEndIndex(i) - b->GetStartIndex(i) + 1;
                    logString_localDims    += "[" + std::to_string(localDims[i]) + "]" ;
                    logString_localOffsets += "[" + std::to_string(localOffset[i]) + "]" ;
                }
                
                block_list.append(b->GetSource() ) ;
                //if (Environment::NumDomainsPerClient() > 1 ) {
                blockid_list.append(b->GetID());  // always add the blockid
                // }
                //// globalDims are not currently used
                //for (int i = 0; i < varDimention; i++) {
                //    globalDims[i] = b->GetGlobalExtent(i) ;
                //}

                // Get pointer to the block data
                void *np_ptr = b->GetDataSpace().GetData();
                
                try {
                   // Wrapping data as NumPy array
                   // ************** Push the NumPy data through to Python 
                   np::ndarray mul_data_ex = ReturnNpNdarray(blockDimension, localDims, np_ptr, v->GetLayout()->GetType()) ;
                
                   // Store reference to NumPy array in Python dictionary
                   variable_data[varName.str()]  = mul_data_ex ;
                }
                catch( bp::error_already_set &e) {
                   CatchPrintAndLogPyException("ERROR: PyAction::PassDataToPython() np::from_data() Error wrapping Damaris data as NumPy array /n ") ;                      
                }
                varName.str("") ; // reset the name string
                Environment::Log(logString_localDims , EventLogger::Debug);
                Environment::Log(logString_localOffsets , EventLogger::Debug);
            } // for each block of the variable
            damaris_variable_dict["numpy_data"]    =  variable_data ; 
            damaris_variable_dict["block_source"]  =  block_list    ;
            damaris_variable_dict["block_domains"] =  blockid_list  ;
            // std::sring typestr = 
            damaris_variable_dict["type_string"]   =  GetTypeString(v->GetLayout()->GetType()) ; // return type string is prefixed by "_"
            
            // For this variable, add all the meta-data and numpy array blocks to the iteration dictionary
            iteration_dict[v->GetName()]           = damaris_variable_dict ;
           
            delete [] localOffset;
            delete [] localDims;
        } // for each variable of the iteration (that is specified with script="..." ))
         
        damarisData_["iteration_data"]           = iteration_dict ;
        std::string logString_Script ;
        logString_Script = std::to_string(iteration) +" PyAction::PassDataToPython() Running Script: " +  this->file_  ;
        Environment::Log(logString_Script , EventLogger::Debug);
        // **************  Now run the external Python script that has access to the exposed data
        try {
            bp::object res = bp::exec_file(this->file_.c_str(), this->globals_, this->locals_) ;
        } 
        catch( bp::error_already_set &e) {
            CatchPrintAndLogPyException("ERROR: PyAction::PassDataToPython() bp::exec_file() Error Running Python Script /n") ;           
        }
        
        // ************** Now remove the iteration data, as the block data will be deleted from shared memory (TBC)
        try {
              bp::object result = bp::exec(regex_string_with_python_code_.c_str(), this->globals_, this->locals_);  
        }  catch( bp::error_already_set &e) {
              CatchPrintAndLogPyException("ERROR: PyAction::PassDataToPython() bp::exec() Deleting Iteration Data " ) ;
        }
        
        
        // ************** Now remove the data, as the block data will be deleted from shared memory (well, maybe only the references to it)
        /*
         * e_ = "\\b(REPLACE)([^ ]*)" ;  // Set the regex as we use it in the constructor for various substitutions
        w = GetVariables().begin();
        // for selected variables ... (like HDF5 storage, we can can have a <variable ... script="MyScript" /> attribute 
        for (; w != GetVariables().end(); w++) {
            std::shared_ptr<Variable> v = w->lock();
            v->GetBlocksByIteration(iteration, begin, end);
            std::string varName;
            
            for (BlocksByIteration::iterator bid = begin; bid != end; bid++) {
                std::shared_ptr<Block> b = *bid;
                varName = GetVariableFullName(v , &b);
                std::string numpy_name =  varName + "_" + std::to_string(iteration) ;
                
                // std::cout <<"INFO: " << iteration << " PyAction::PassDataToPython() deleting: " << numpy_name << std::endl << std::flush ;
                numpy_name +=  "$2" ;
               
                std::string string_with_python_code = std::regex_replace (regex_string_with_python_code_,this->e_,numpy_name.c_str());
                
                try {
                    bp::object result = bp::exec(string_with_python_code.c_str(), this->globals_, this->locals_);  
                }  catch( bp::error_already_set &e) {
                    CatchPrintAndLogPyException("ERROR: PyAction::PassDataToPython() bp::exec() " ) ;
                }
            }                        
        }
        */
            
          
        
        
        // if all good then return true
        return true;
    }


    /*
     void HDF5Store::OutputPerCore(int32_t iteration) {
        hid_t       fileId, dsetId;           // file and dataset identifiers
        hid_t       fileSpace , memSpace;     // file and memory dataspace identifiers
        hid_t       dtypeId = -1;
        hid_t       lcplId;
        std::string      fileName;
        std::vector<std::weak_ptr<Variable> >::const_iterator w;

        // Initialise variables
        fileName = GetOutputFileName(iteration);
        w = GetVariables().begin();

        // Create the HDF5 file
        if ((fileId = H5Fcreate(fileName.c_str() , H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT)) < 0)
            ERROR("HDF5: H5Fcreate Failed ");

        // Enable group creation with fully qualified dataset names
        lcplId = H5Pcreate (H5P_LINK_CREATE);
        H5Pset_create_intermediate_group(lcplId, 1);

        // for each variable ...
        for (; w != GetVariables().end(); w++) {
            std::shared_ptr<Variable> v = w->lock();

            // non TimeVarying variables only are written in the first iteration.
            if ((not v->IsTimeVarying()) && (iteration > 0))
                continue;

            // Getting the dimention of the variable
            int varDimention;
            varDimention = v->GetLayout()->GetDimensions();

            // Create a array for dimentions
            hsize_t *globalDims;
            hsize_t *localDims;
            globalDims = new (std::nothrow) hsize_t[varDimention];
            localDims = new (std::nothrow) hsize_t[varDimention];

            if ((globalDims == NULL) || (localDims == NULL)) {
                ERROR("Failed to allocate memory for dim arrays!");
            }


           
            // fileSpace creation is being moved to the inner block loop due
            // to issue with updating layout sizes when parameters change on the
            // server side. See:
            // https://gitlab.inria.fr/Damaris/damaris-development/-/issues/20
            

            // Getting the equivalent hDF5 Variable Type
            if (not GetHDF5Type(v->GetLayout()->GetType(), dtypeId))
                ERROR("HDF5:Unknown variable type " << v->GetLayout()->GetType());

            BlocksByIteration::iterator begin;
            BlocksByIteration::iterator end;
            v->GetBlocksByIteration(iteration, begin, end);
            int numBlocks = 0;
            std::string varName;

            for (BlocksByIteration::iterator bid = begin; bid != end; bid++) {
                std::shared_ptr<Block> b = *bid;
                numBlocks++;

                // Create block dimentions
                int blockDimention = b->GetDimensions();
                //for (int i = 0; i < blockDimention; i++)
                //    b->GetGlobalExtent(i);              // TODO: What does this do? Nothing!

                hsize_t *blockDim = new (std::nothrow) hsize_t[blockDimention];
                if (blockDim == NULL)
                    ERROR("HDF5:Failed to allocate memory ");

                // Obtain the block size
                // HDF5 uses C storage conventions, assuming that the last listed
                // dimension is the fastest-changing dimension and the first-listed
                // dimension is the slowest changing.
                // So here we are assuming that Damaris has stored the fastest moving dimension
                // in the 1st ([0]) position of the lower_bounds_ and upper_bounds_ arrays
                int i_backwards = blockDimention - 1 ;
                for (int i = 0 ; i < blockDimention ; i++)
                {
                     blockDim[i_backwards] = b->GetEndIndex(i) - b->GetStartIndex(i) + 1;
                     i_backwards-- ;
                }

                // Obtain the FilesSpace size (has to match the memory space dimensions)
                i_backwards =  blockDimention - 1;
                for (int i = 0 ; i < blockDimention ; i++) {
                    localDims[i_backwards] = b->GetEndIndex(i) - b->GetStartIndex(i) + 1;
                    i_backwards-- ;
                }

                // globalDims are not currently used (may be needed for VDS support?)
                for (int i = 0; i < varDimention; i++) {
                    globalDims[i] = b->GetGlobalExtent(i) ;
                }

                // create the file space
                if ((fileSpace = H5Screate_simple(varDimention, localDims , NULL)) < 0)
                    ERROR("HDF5: file space creation failed !");

                // Create Dataset for each block
                varName = GetVariableFullName(v , &b);
                if ((dsetId = H5Dcreate(fileId, varName.c_str() , dtypeId , fileSpace,
                                         lcplId, H5P_DEFAULT, H5P_DEFAULT)) < 0)
                    ERROR("HDF5: Failed to create dataset ... ");
               
                // Create memory data space
                memSpace = H5Screate_simple(blockDimention, blockDim , NULL);

                // Update ghost zones
                UpdateGhostZones(v , memSpace , blockDim);

                // Select hyperslab in the file.
                fileSpace = H5Dget_space(dsetId);
                H5Sselect_all(fileSpace);
                //H5Sselect_hyperslab(fileSpace, H5S_SELECT_SET, memOffset, NULL, blockDim , NULL);

                // Getting the data
                void *ptr = b->GetDataSpace().GetData();

                // Writing data
                if (H5Dwrite(dsetId, dtypeId, memSpace, fileSpace, H5P_DEFAULT, ptr) < 0)
                    ERROR("HDF5: Writing Data Failed !");

                // 8 Free evertything
                delete [] blockDim;
                H5Sclose(memSpace);
                H5Sclose(fileSpace);
                H5Dclose(dsetId);
            } // for of block iteration
            // H5Sclose(fileSpace);
            delete [] globalDims;
            delete [] localDims;
        } // for of variable iteration

        H5Fclose(fileId);
        H5Pclose(lcplId);
    }
    
    
    */
}

// #endif // HAVE_PYTHON_ENABLED

