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
                } else {
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
                path_ = (string)(*op);
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

        switch(fileMode_) {
            case FilePerCore:
                OutputPerCore(iteration);
                break;
            case Collective:
                OutputCollective(iteration);
                break;
            default: // i.e. File per node
                ERROR("FileMode is not supported!!!");
        }

    }

    string HDF5Store::GetOutputFileName(int32_t iteration) {
        stringstream fileName;
        int processId;
        string baseName;

        processId = Environment::GetEntityProcessID();
        baseName = Environment::GetSimulationName();

        if (fileMode_ == FilePerCore){
            fileName << path_ << baseName << "_It" << iteration << "_Pr" << processId << ".h5";
        } else if (fileMode_ == Collective) {
            fileName << path_ << baseName << "_It" << iteration << ".h5";
        }

        return  fileName.str();
    }

    string HDF5Store::GetVariableFullName(shared_ptr<Variable> v , shared_ptr<Block> *b){
        stringstream varName;
        string baseName;
        int numDomains;

        baseName = Environment::GetSimulationName();
        numDomains = Environment::NumDomainsPerClient();

        if ((fileMode_ == Collective) || (b == NULL))
            return v->GetName();

        if (numDomains == 1){
            varName << v->GetName() << "/P" << (*b)->GetSource(); // e.g. varName/P2
        } else {// more than one domain
            varName << v->GetName() << "/P" << (*b)->GetSource() << "/B" << (*b)->GetID(); // e.g. varName/P3/B2
        }

        return  varName.str();
    }

    void HDF5Store::OutputPerCore(int32_t iteration) {
        hid_t       fileId, dsetId;           // file and dataset identifiers
        hid_t       fileSpace , memSpace;     // file and memory dataspace identifiers
        hid_t       dtypeId = -1;
        hid_t       lcplId;
        string      fileName;
        vector<weak_ptr<Variable> >::const_iterator w;

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
            shared_ptr<Variable> v = w->lock();

            // non TimeVarying variables only are written in the first iteration.
            if ((not v->IsTimeVarying()) && (iteration > 0))
                continue;

            // Getting the dimention of the variable
            int varDimention;
            varDimention = v->GetLayout()->GetDimensions();

            // Create a array for dimentions
            hsize_t *globalDims;
            hsize_t *localDims;
            globalDims = new (nothrow) hsize_t[varDimention];
            localDims = new (nothrow) hsize_t[varDimention];

            if ((globalDims == NULL) || (localDims == NULL)) {
                ERROR("Failed to allocate memory for dim arrays!");
            }

            for (int i = 0; i < varDimention; i++) {
                globalDims[i] = v->GetLayout()->GetGlobalExtentAlong(i);
                localDims[i] = v->GetLayout()->GetExtentAlong(i);
            }

            // create the file space
            if ((fileSpace = H5Screate_simple(varDimention, localDims , NULL)) < 0)
                ERROR("HDF5: file space creation failed !");

            // Getting the equivalend hDF5 Variable Type
            if (not GetHDF5Type(v->GetLayout()->GetType(), dtypeId))
                ERROR("HDF5:Unknown variable type " << v->GetLayout()->GetType());

            BlocksByIteration::iterator begin;
            BlocksByIteration::iterator end;
            v->GetBlocksByIteration(iteration, begin, end);
            int numBlocks = 0;
            string varName;

            for (BlocksByIteration::iterator bid = begin; bid != end; bid++) {
                shared_ptr<Block> b = *bid;
                numBlocks++;

                // Create Dataset for each block
                varName = GetVariableFullName(v , &b);
                if ((dsetId = H5Dcreate(fileId, varName.c_str() , dtypeId , fileSpace,
                                         lcplId, H5P_DEFAULT, H5P_DEFAULT)) < 0)
                    ERROR("HDF5: Failed to create dataset ... ");

                // Create block dimentions
                int blockDimention = b->GetDimensions();
                for (int i = 0; i < blockDimention; i++)
                    b->GetGlobalExtent(i);

                hsize_t *blockDim = new (nothrow) hsize_t[blockDimention];
                if (blockDim == NULL)
                    ERROR("HDF5:Failed to allocate memory ");

                // Obtain the block size
                for (int i = 0; i < blockDimention; i++)
                    blockDim[i] = b->GetEndIndex(i) - b->GetStartIndex(i) + 1;

                // Create memory data space
                memSpace = H5Screate_simple(blockDimention, blockDim , NULL);

                // Select hyperslab in the file.
                fileSpace = H5Dget_space(dsetId);
                H5Sselect_all(fileSpace);

                // Getting the data
                void *ptr = b->GetDataSpace().GetData();

                // Writing data
                if (H5Dwrite(dsetId, dtypeId, memSpace, fileSpace, H5P_DEFAULT, ptr) < 0)
                    ERROR("HDF5: Writing Data Failed !");

                // 8 Free evertything
                delete [] blockDim;
                H5Sclose(memSpace);
                H5Dclose(dsetId);
            } // for of block iteration
            H5Sclose(fileSpace);
            delete [] globalDims;
            delete [] localDims;
        } // for of variable iteration

        H5Fclose(fileId);
    }

    void HDF5Store::OutputCollective(int32_t iteration) {
        hid_t fileId;
        hid_t lcplId;
        hid_t dsetId;
        hid_t dtypeId = -1;
        hid_t fileSpace;
        hid_t memSpace;
        hid_t plistId = H5P_DEFAULT;
        string fileName;


        // Initializing variables
        vector<weak_ptr<Variable> >::const_iterator w = GetVariables().begin();
        MPI_Comm comm = Environment::GetEntityComm();
        MPI_Info info  = MPI_INFO_NULL;
        fileName = GetOutputFileName(iteration);

        // Create file access property list
	    plistId = H5Pcreate(H5P_FILE_ACCESS);
	    H5Pset_fapl_mpio(plistId, comm, info);

        // Creating the HDF5 file
        if((fileId = H5Fcreate(fileName.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, plistId)) < 0)
            ERROR("HDF5: H5Fcreate Failed ");

        H5Pclose(plistId);

        // enabling group creation with full qualified names
        lcplId = H5Pcreate (H5P_LINK_CREATE);
        H5Pset_create_intermediate_group(lcplId, 1);

        // for each variable do
        for(; w != GetVariables().end(); w++) {
	        shared_ptr<Variable> v = w->lock();

            // write time varying variables only in first iteration
            if ((not v->IsTimeVarying()) && (iteration > 0))
                    continue;

	        int varDimention;
	        varDimention = v->GetLayout()->GetDimensions();

            hsize_t *globalDim;
            hsize_t *localDim;
            globalDim = new (nothrow) hsize_t[varDimention];
            localDim = new (nothrow) hsize_t[varDimention];

            if ((globalDim == NULL) || (localDim == NULL)) {
                ERROR("Failed to allocate memory for dim arrays!");
            }

	        for(int i = 0; i < varDimention; i++) {
	            globalDim[i] = v->GetLayout()->GetGlobalExtentAlong(i);
	            localDim[i] = v->GetLayout()->GetExtentAlong(i);
	        }

            // Create dataspace.
	        if ((fileSpace = H5Screate_simple(varDimention, globalDim , NULL)) < 0)
                ERROR("HDF5: file space creation failed !");

            // Get equivalent HDF5 type
            if (not GetHDF5Type(v->GetLayout()->GetType() , dtypeId))
                ERROR("HDF5:Unknown variable type " << v->GetLayout()->GetType());

            string varName = GetVariableFullName(v);
            if ((dsetId = H5Dcreate( fileId, varName.c_str(), dtypeId, fileSpace,
                     lcplId, H5P_DEFAULT, H5P_DEFAULT)) < 0)
                ERROR("HDF5: Failed to create dataset ... ");

            H5Sclose(fileSpace);

            BlocksByIteration::iterator begin;
            BlocksByIteration::iterator end;
            v->GetBlocksByIteration(iteration, begin, end);

            for(BlocksByIteration::iterator bid = begin; bid != end; bid ++) {
                shared_ptr<Block> b = *bid;

                hsize_t *offset;

                offset = new (nothrow) hsize_t[varDimention];
                if (offset == NULL)
                    ERROR("HDF5:Failed to allocate memory ");

                // Obtain the starting indices of the hyperslab
                for(int i = 0; i < varDimention; i++) {
                    offset[i] = b->GetStartIndex(i);
                }

                memSpace = H5Screate_simple(varDimention, localDim , NULL);

                // Select hyperslab in the file.
                fileSpace = H5Dget_space(dsetId);
                H5Sselect_hyperslab(fileSpace, H5S_SELECT_SET, offset, NULL, localDim , NULL);

                // Create property list for collective dataset write.
                plistId = H5Pcreate(H5P_DATASET_XFER);
                H5Pset_dxpl_mpio(plistId, H5FD_MPIO_COLLECTIVE);

                void* ptr = b->GetDataSpace().GetData();

                if (H5Dwrite(dsetId, dtypeId, memSpace, fileSpace, plistId, ptr) < 0)
                    ERROR("HDF5: Writing Data Failed !");

                H5Sclose(fileSpace);
                H5Sclose(memSpace);
                H5Pclose(plistId);

                delete [] offset;
            } // for the blocks loop
            delete [] localDim;
            delete [] globalDim;
            H5Dclose(dsetId);
        } // for the variables loop
    }
}

