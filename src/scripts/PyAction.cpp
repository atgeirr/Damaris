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

    // Constructor (protected)
    PyAction::PyAction(const model::Action &mdl)
            : ScriptAction(mdl) {

        fileMode_= FilePerCore;
        xdmfMode_= NoIteration;

        //if (!ReadKeyValues(mdl))
        //    ERROR("Bad KeyValue Parameters for HDF5 Store");
    }

  /*  bool PyAction::ReadKeyValues(const model::Script &mdl) {
        model::Action::option_const_iterator op = mdl.option().begin();

        for (; op != mdl.option().end(); op++) {
            //std::cout << "Option \"" << op->key() << "\" has value \"" << (string)(*op) << "\"" << std::endl;


            if (op->key()->compare("FileMode") == 0) {
                if (op->compare("FilePerCore") == 0) {
                    fileMode_ = FilePerCore;
                } else if (op->compare("FilePerNode") == 0) {
                    fileMode_ = FilePerNode;
                    ERROR("FilePerNode mode is not supported in this version!");
                    return false;
                } else if (op->compare("Collective") == 0) {
                    fileMode_ = Collective;
                } else if (op->compare("CollectiveTest") == 0) {
                    fileMode_ = CollectiveTest;
                }  else {
                    ERROR("FileMode is undefined. ");
                    return false;
                }
            } else if (op->key()->compare("XDMFMode") == 0) {
                if (op->compare("NoIteration") == 0) {
                    xdmfMode_ = NoIteration;
                } else if (op->compare("FirstIteration") == 0) {
                    xdmfMode_ = FirstIteration;
                    ERROR("Generating the XMF file is not supported in this version.");
                } else if (op->compare("EveryIteration") == 0) {
                    xdmfMode_ = EveryIteration;
                    ERROR("Generating the XMF file is not supported in this version.");
                } else {
                    ERROR("Undefined value for XDMFMode key !");
                    return false;
                }
            } else if (op->key()->compare("FilesPath") == 0) {
                path_ = (std::string)(*op);
            }
        }
        return true;
    }

*/

    void PyAction::Output(int32_t sourceID, int32_t iteration, const char* args){
/*
        if (IterationIsEmpty(iteration))
            return;

        switch(fileMode_) {
            case FilePerCore:
                OutputPerCore(iteration);
                break;
            case Collective:
                OutputCollective(iteration);
                break;
            case CollectiveTest:
                OutputCollective(iteration);
                break;
            default: // e.g. file per dedicated node
                ERROR("FileMode is not supported!!!");
        }
        */
    }

   /*
    bool PyAction::GetNumPyType(model::Type mdlType, np::dtype &dt) {
        if (mdlType.compare("int") == 0) {
            dt = np::dtype::get_builtin<int>();
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
    
    
    std::string PyAction::GetOutputFileName(int32_t iteration) {
        std::stringstream fileName;
        int processId;
        std::string baseName;

        processId = Environment::GetEntityProcessID();
        baseName = Environment::GetSimulationName();

        if (fileMode_ == FilePerCore){
            fileName << path_ << baseName << "_It" << iteration << "_Pr" << processId << ".h5";
        } else if (fileMode_ == Collective) {
            fileName << path_ << baseName << "_It" << iteration << ".h5";
        } else if (fileMode_ == CollectiveTest) {
                    fileName << path_ << baseName << "_It" << iteration << ".h5";
                }

        return  fileName.str();
    }

    std::string PyAction::GetVariableFullName(std::shared_ptr<Variable> v , std::shared_ptr<Block> *b){
        std::stringstream varName;
        std::string baseName;
        int numDomains;

        baseName = Environment::GetSimulationName();
        numDomains = Environment::NumDomainsPerClient();

        // (b == NULL) means that there is no access to block data, i.e. in file-per-core mode or future modes.
        if ((fileMode_ == Collective) || (fileMode_ == CollectiveTest) ||  (b == NULL))
            return v->GetName();

        if (numDomains == 1){
            varName << v->GetName() << "/P" << (*b)->GetSource(); // e.g. varName/P2
        } else {// more than one domain
            varName << v->GetName() << "/P" << (*b)->GetSource() << "/B" << (*b)->GetID(); // e.g. varName/P2/B3
        }

        return  varName.str();
    }
    
    
    void PyAction::CoProcess(int iteration)
    {
        /*
         * vtkNew<vtkCPDataDescription> dataDescription;

        bool lastTimeStep = false ;
        // specify the simulation time and time step for Catalyst
        dataDescription->AddInput("input");

        dataDescription->SetTimeData(iteration*timestep_ , iteration);  // What is the difference? Simulation time and simulation iteration

        if (end_iteration_ != 0)
        {
            if (iteration == end_iteration_)
                lastTimeStep = true ;
        }
        // check: end_iteration_ from the xsd model. if not == 0 then use the value
        if (lastTimeStep == true) { // How to know about it?
            dataDescription->ForceOutputOn();
        }

        if (processor_->RequestDataDescription(dataDescription) != 0) {

            int gridCount = MeshManager::GetNumObjects();

            if (gridCount < 1) {
                ERROR("No mesh has been defined in the Xml file!");
                return;
            }

            vtkNew<vtkMultiBlockDataSet> rootGrid;

            if (not FillMultiBlockGrid(iteration , rootGrid)) {
                ERROR("Error in Filling the root multi-block grid in iteration " << iteration);
                return;
            }

            // Catalyst gets the proper input dataset for the pipeline.
            dataDescription->GetInputDescriptionByName("input")->SetGrid(rootGrid);

            //if ( processor_->GetNumberOfPipelines() == 0)
            //	AddPythonPipeline();

            // Call Catalyst to execute the desired pipelines.
            //  if (iteration > 0)
            if ( processor_->GetNumberOfPipelines() > 0)
                processor_->CoProcess(dataDescription);
        }
        */
    }
    
    bool PyAction::PassDataToPython(int iteration )
    {
       /* int index = 0;
        int serverCount = Environment::CountTotalServers();
        int meshCount = MeshManager::GetNumObjects();
        int serverRank = Environment::GetEntityProcessID();

        rootGrid->SetNumberOfBlocks(meshCount);

        auto meshItr = MeshManager::Begin();
        for(; meshItr != MeshManager::End(); meshItr++) {
            std::shared_ptr<Mesh> mesh = *meshItr;

            // vtkMultiPieceDataSet* vtkMPGrid = vtkMultiPieceDataSet::New();
            vtkNew<vtkMultiPieceDataSet> vtkMPGrid ;
            rootGrid->SetBlock(index , vtkMPGrid);

            index++;

            auto varItr = VariableManager::Begin();
            for(; varItr != VariableManager::End(); varItr++) {

                std::shared_ptr<Variable> var = *varItr;
                const model::Variable& mdl = var->GetModel();

                if (mdl.mesh() == "#") continue;    // a variable with empty mesh
                if (mdl.mesh() != mesh->GetName()) continue;  // a variable with a different mesh
                if (mdl.visualizable() == false) continue;  // non-visualizable variable

                if (not (*varItr)->AddBlocksToVtkGrid(vtkMPGrid , iteration)) {
                    ERROR("Error in adding blocks to variable " << var->GetName());
                    return false;
                }
            }
        }*/

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


           
            //* fileSpace creation is being moved to the inner block loop due
            //* to issue with updating layout sizes when parameters change on the
            //* server side. See:
            //* https://gitlab.inria.fr/Damaris/damaris-development/-/issues/20
            

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

