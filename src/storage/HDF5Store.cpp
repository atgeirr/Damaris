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
#include "storage/HDF5Store.hpp"




namespace damaris {

    HDF5Store::HDF5Store(const model::Store &mdl)
            : Store(mdl) {

        fileMode_= FilePerCore;
        xdmfMode_= NoIteration;

        if (!ReadKeyValues(mdl))
            ERROR("Bad KeyValue Parameters for HDF5 Store");
    }

    bool HDF5Store::ReadKeyValues(const model::Store &mdl) {
        model::Store::option_const_iterator op = mdl.option().begin();

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

    bool HDF5Store::GetHDF5Type(model::Type mdlType, hid_t &hdfType) {
        if (mdlType.compare("int") == 0) {
            hdfType = H5T_NATIVE_INT;
        } else if (mdlType.compare("float") == 0) {
            hdfType = H5T_NATIVE_FLOAT;
        } else if (mdlType.compare("real") == 0) {
            hdfType = H5T_NATIVE_FLOAT;
        } else if (mdlType.compare("integer") == 0) {
            hdfType = H5T_NATIVE_INT;
        } else if (mdlType.compare("double") == 0) {
            hdfType = H5T_NATIVE_DOUBLE;
        } else if (mdlType.compare("long") == 0) {
            hdfType = H5T_NATIVE_LONG;
        } else if (mdlType.compare("short") == 0) {
            hdfType = H5T_NATIVE_SHORT;
        } else if (mdlType.compare("char") == 0) {
            hdfType = H5T_NATIVE_CHAR;
        } else if (mdlType.compare("character") == 0) {
            hdfType = H5T_NATIVE_CHAR;
        } else {
            return false;
        }

        return true;
    }

    void HDF5Store::Output(int32_t iteration){

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
    }

    bool HDF5Store::UpdateGhostZones(std::shared_ptr<Variable> v , hid_t &memSpace , hsize_t* localDim){

        int varDimention = v->GetLayout()->GetDimensions();
        hsize_t* offset = new hsize_t[varDimention];
        bool retVal = true;

        for(int i=0; i<varDimention ; i++) {
            int g1 = v->GetLayout()->GetGhostAlong(i).first;
            int g2 = v->GetLayout()->GetGhostAlong(i).second;

            offset[i] = g1;
            localDim[i] = localDim[i] - g1 - g2;
        }

        if ((H5Sselect_hyperslab(memSpace, H5S_SELECT_SET, offset, NULL, localDim , NULL)) < 0){
            ERROR("Hyperslab on memory buffer failed.");
            retVal = false;
        }

        delete [] offset;
        return retVal;
    }

    std::string HDF5Store::GetOutputFileName(int32_t iteration) {
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

    std::string HDF5Store::GetVariableFullName(std::shared_ptr<Variable> v , std::shared_ptr<Block> *b){
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


           /*
            * fileSpace creation is being moved to the inner block loop due
            * to issue with updating layout sizes when paramaters change on the
            * server side. See:
            * https://gitlab.inria.fr/Damaris/damaris-development/-/issues/20
            */

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
                for (int i = 0; i < blockDimention; i++)
                    b->GetGlobalExtent(i);

                hsize_t *blockDim = new (std::nothrow) hsize_t[blockDimention];
                if (blockDim == NULL)
                    ERROR("HDF5:Failed to allocate memory ");

                // Obtain the block size
                for (int i = 0; i < blockDimention; i++)
                    blockDim[i] = b->GetEndIndex(i) - b->GetStartIndex(i) + 1;

                // Obtain the FilesSpace size (has to match the memory space dimensions)
                for (int i = 0; i < varDimention; i++) {
                    localDims[i] = b->GetEndIndex(i) - b->GetStartIndex(i) + 1;
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
    
    

    void HDF5Store::OutputCollective(int32_t iteration) {
        hid_t fileId;
        hid_t lcplId;
        hid_t dsetId;
        hid_t dtypeId = -1;
        hid_t fileSpace;
        hid_t fileSpace2;
        hid_t memSpace;
        hsize_t *memOffset;
        hsize_t *memDim;

        hid_t plistId = H5P_DEFAULT;
        std::string fileName;

        BlocksByIteration::iterator begin;
        BlocksByIteration::iterator end;


        // Initializing variables
        std::vector<std::weak_ptr<Variable> >::const_iterator w = GetVariables().begin();
        MPI_Comm comm = Environment::GetEntityComm();
        MPI_Info info  = MPI_INFO_NULL;
        fileName = GetOutputFileName(iteration);

        // Create file access property list
        plistId = H5Pcreate(H5P_FILE_ACCESS);
        H5Pset_fapl_mpio(plistId, comm, info);

        // Creating the HDF5 file
        if((fileId = H5Fcreate(fileName.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, plistId)) < 0)
            ERROR("HDF5: H5Fcreate Failed. Filename: " << fileName.c_str() );

        H5Pclose(plistId);

        // enabling group creation with full qualified names
        lcplId = H5Pcreate (H5P_LINK_CREATE);
        H5Pset_create_intermediate_group(lcplId, 1);

        // for each variable do
        for(; w != GetVariables().end(); w++) {
                std::shared_ptr<Variable> v = w->lock();

            // write time-varying variables only in first iteration
            if ((not v->IsTimeVarying()) && (iteration > 0))
                    continue;

            int varDimention;
            varDimention = v->GetLayout()->GetDimensions();

            hsize_t *globalDim;
            globalDim = new (std::nothrow) hsize_t[varDimention];

            // Get equivalent HDF5 type
            if (not GetHDF5Type(v->GetLayout()->GetType() , dtypeId))
                ERROR("HDF5:Unknown variable type " << v->GetLayout()->GetType());

            v->GetBlocksByIteration(iteration, begin, end);

            memOffset = new (std::nothrow) hsize_t[varDimention];
            memDim = new (std::nothrow) hsize_t[varDimention];

            if ((memOffset == NULL) || (memDim == NULL)) {
             ERROR("HDF5: Failed to allocate memDim and memOffset memory ");
              ERROR("Writing blocks to the file failed. ");
            }
            int numBlocks = 0;
            for(BlocksByIteration::iterator bid = begin; bid != end; bid ++) {
                 std::shared_ptr<Block> b = *bid;
                 if (numBlocks == 0) {
                     // Obtain the FilesSpace size (has to match the memory space dimensions)
                    for (int i = 0; i < varDimention; i++) {
                        globalDim[i] = b->GetGlobalExtent(i);
                    }
                    // Create dataspace.
                    if ((fileSpace = H5Screate_simple(varDimention, globalDim , NULL)) < 0)
                        ERROR("HDF5: file space creation failed !");

                    std::string varName = GetVariableFullName(v);
                    if ((dsetId = H5Dcreate( fileId, varName.c_str(), dtypeId, fileSpace,
                             lcplId, H5P_DEFAULT, H5P_DEFAULT)) < 0)
                        ERROR("HDF5: Failed to create dataset ... " << varName.c_str());

                    H5Sclose(fileSpace);
                }

                 // Obtain the starting indices and the size of the hyperslab
                 for(int i = 0; i < varDimention; i++) {
                     memOffset[i] = b->GetStartIndex(i);
                     memDim[i]   = b->GetEndIndex(i) - b->GetStartIndex(i) + 1;
                 }

                 // create memory data space
                 memSpace = H5Screate_simple(varDimention, memDim , NULL);

                 // Update ghost zones (N.B. untested)
                 UpdateGhostZones(v , memSpace , memDim);

                 // Select hyperslab in the file.
                 fileSpace2 = H5Dget_space(dsetId);
                 H5Sselect_hyperslab(fileSpace2, H5S_SELECT_SET, memOffset, NULL, memDim , NULL);

                 // Create property list for collective dataset write.
                 plistId = H5Pcreate(H5P_DATASET_XFER);
                 H5Pset_dxpl_mpio(plistId, H5FD_MPIO_COLLECTIVE);

                 void* ptr = b->GetDataSpace().GetData();

                 if (H5Dwrite(dsetId, dtypeId, memSpace, fileSpace2, plistId, ptr) < 0) {
                     ERROR("Writing blocks to the file failed. ");
                 }
                 numBlocks++ ;
                 H5Sclose(fileSpace2);
                 H5Sclose(memSpace);
                 H5Pclose(plistId);
            } // for the blocks loop
            delete [] memOffset;
            delete [] memDim;
            delete [] globalDim;
            H5Dclose(dsetId);
        } // for loop over variables
        
        H5Fclose(fileId);
        H5Pclose(lcplId);
    }

}

