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

#include <sstream>
#include "util/Debug.hpp"
#include "damaris/scripts/PyAction.hpp"


namespace damaris {
    template <typename T>
    struct StaticCast
    {
        template <typename U>
        T operator()(const U& rhs)
        {
            return static_cast<const T>(rhs);
        }
    };
}
    
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

    /**
     * Constructor.
     */
    PyAction::PyAction(const model::Script& mdl)
                            : ScriptAction(mdl)
    { 
        name_     = mdl.name() ;
        language_ = mdl.language() ;
        file_     = mdl.file() ;
        frequency_= mdl.frequency() ;
        
        Py_Initialize();
        np::initialize();
  
        /**
        * import the __main__ module and obtain the globals dict
        * assign to the bp::object 
        */
        main     = bp::import("__main__");
        globals  = main.attr("__dict__");
      
        /**
        * String of Python code used to remove datasets from Pyhton environment when the 
        * Damris data they use  is invalidated/deleted
        */
        regex_string_with_python_code = "try :               \n"
                                        "  del DamarisData['REPLACE']   \n"
                                        "except KeyError as err:             \n"
                                        "  print('Damaris Server: KeyError could not delete key: ', err) \n" ;; 

    }
    
    
    void PyAction::Call(int32_t sourceID, int32_t iteration, const char* args){
        if (iteration % frequency_ == 0){
            PassDataToPython( iteration );
        }
    }

    
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
    
    /*
    bool PyAction::StaticConstCast(void * serv_ptr) {
        if (mdlType.compare("int") == 0) {
            return StaticCast<int>();
        } else if (mdlType.compare("float") == 0) {
            dt = np::dtype::get_builtin<float>();
        } else if (mdlType.compare("real") == 0) {
            dt = np::dtype::get_builtin<float>();
        } else if (mdlType.compare("integer") == 0) {
            dt = np::dtype::get_builtin<int>();
        } else if (mdlType.compare("double") == 0) {
            dt = np::dtype::get_builtin<double>();
        } else if (mdlType.compare("long") == 0) {
            dt = np::dtype::get_builtin<long int>();
        } else if (mdlType.compare("short") == 0) {
            dt = np::dtype::get_builtin<short>();;
        } else if (mdlType.compare("char") == 0) {
            dt = np::dtype::get_builtin<char>();
        } else if (mdlType.compare("character") == 0) {
            dt = np::dtype::get_builtin<char>();
        } else {
            return false;
        }
        return true;
    }
    */
    
    
    
    std::string PyAction::GetVariableFullName(std::shared_ptr<Variable> v , std::shared_ptr<Block> *b){
        std::stringstream varName;
        std::string baseName;
        int numDomains;

        baseName = Environment::GetSimulationName();
        numDomains = Environment::NumDomainsPerClient();
        std::string typestr ;
        // (b == NULL) means that there is no access to block data, i.e. in file-per-core mode or future modes.
        
        
        if (numDomains == 1){
            varName << v->GetName() << typestr << "_P" << (*b)->GetSource(); // e.g. varName_P2
        } else {// more than one domain
            varName << v->GetName() << typestr << "_P" << (*b)->GetSource() << "_B" << (*b)->GetID(); // e.g. varName_P2_B3
        }

        return  varName.str();
    }
    
    

    
    
    bool PyAction::PassDataToPython(int iteration )
    {
        
        bp::object own_local = bp::object() ;
        // np::dtype dt ;  //= np::dtype::get_builtin<int>()
        
        // std::vector<std::weak_ptr<Variable> >::const_iterator w;
        // w = GetVariables().begin();
        
        // for each variable ... (unlike HDF5 storage, which can have a <variable ... store="" /> attribute 
        VariableManager::iterator v = VariableManager::Begin();
        for(; v != VariableManager::End(); v++) {
        
        // for (; w != GetVariables().end(); w++) {
            // std::shared_ptr<Variable> v = w->lock();

            // non TimeVarying variables only are written in the first iteration.
            if ((not v->get()->IsTimeVarying()) && (iteration > 0))
                continue;

            // Getting the dimensions of the variable
            int varDimention;
            varDimention = v->get()->GetLayout()->GetDimensions();

            // Create a array for dimensions
            int *globalDims;
            int *localDims;
            globalDims = new (std::nothrow) int[varDimention];
            localDims = new (std::nothrow)  int[varDimention];

            if ((globalDims == NULL) || (localDims == NULL)) {
                ERROR("in PyAction::PassDataToPython(): Failed to allocate memory for dim arrays!");
            }

            // Getting the equivalent boost  Variable Type
            // if (not GetNumPyType(v->GetLayout()->GetType(), dt)) 
            //    ERROR("in PyAction::PassDataToPython(): Unknown variable type " << v->GetLayout()->GetType());

            BlocksByIteration::iterator begin;
            BlocksByIteration::iterator end;
            v->get()->GetBlocksByIteration(iteration, begin, end);
            std::string varName;

            for (BlocksByIteration::iterator bid = begin; bid != end; bid++) {
                std::shared_ptr<Block> b = *bid;

                // Obtain block array dimension
                int blockDimension = b->GetDimensions();

                int *blockDim = new (std::nothrow) int[blockDimension];
                if (blockDim == NULL)
                    ERROR("in PyAction::PassDataToPython(): Failed to allocate blockDim memory ");

                // Obtain the block size
                // Numpy uses C storage conventions, assuming that the last listed
                // dimension is the fastest-changing dimension and the first-listed
                // dimension is the slowest changing.
                // So here we are assuming that Damaris has stored the fastest moving dimension
                // in the 1st ([0]) position of the lower_bounds_ and upper_bounds_ arrays
                int i_backwards = blockDimension - 1 ;
                for (int i = 0 ; i < blockDimension ; i++)
                {
                     blockDim[i_backwards] = b->GetEndIndex(i) - b->GetStartIndex(i) + 1;
                     i_backwards-- ;
                }

                // Obtain the FilesSpace size (has to match the memory space dimensions)
                i_backwards =  blockDimension - 1;
                for (int i = 0 ; i < blockDimension ; i++) {
                    localDims[i_backwards] = b->GetEndIndex(i) - b->GetStartIndex(i) + 1;
                    i_backwards-- ;
                }

                // globalDims are not currently used (may be needed for VDS support?)
                for (int i = 0; i < varDimention; i++) {
                    globalDims[i] = b->GetGlobalExtent(i) ;
                }

                // Create Dataset name for this block- includes the data type string: _<type>_P<X>[_B<Y>]
                // <type> as C named data type
                // <X> is the variable source rank
                // <Y> is the block number for the rank
                varName = GetVariableFullName((*v) , &b);

                // Update ghost zones
                // UpdateGhostZones(v , memSpace , blockDim);

                // Getting the data
                void *np_ptr = b->GetDataSpace().GetData();

                // Writing data
                damarisData["iteration"] = iteration ;  // This is the current iteration
                
                 np::ndarray mul_data_ex = np::from_data( static_cast<const int *>( np_ptr ), GetNumPyType(v->get()->GetLayout()->GetType()) ,
                                            bp::make_tuple(localDims[0],localDims[1],localDims[2]),
                                            bp::make_tuple(sizeof(int)*localDims[2]*localDims[1],sizeof(int)*localDims[2],sizeof(int)),
                                             own_local);
                std::string numpy_name =  varName + "_" + std::to_string(iteration) ;
                damarisData[numpy_name]  = mul_data_ex ;
 

                delete [] blockDim;
            } // for of block iteration
 
            delete [] globalDims;
            delete [] localDims;
        } // for of variable iteration

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

