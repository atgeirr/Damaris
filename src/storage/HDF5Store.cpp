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
            std::cout << "Option \"" << op->key() << "\" has value \"" << (string)(*op) << "\"" << std::endl;


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

        cout << "output file name is: " << fileName.str() << endl;
        return  fileName.str();
    }

    void HDF5Store::OutputPerCore(int32_t iteration) {
        hid_t       file_id, dset_id;                        // file and dataset identifiers
        hid_t       filespace , memspace;     // file and memory dataspace identifiers
        hid_t	    plist_id = H5P_DEFAULT;    // property list identifier
        hid_t       dtype_id = -1;
        string      fileName;


        vector<weak_ptr<Variable> >::const_iterator w = GetVariables().begin();

        // 1. Initialie file
        fileName = GetOutputFileName(iteration);

        file_id = H5Fcreate(fileName.c_str() , H5F_ACC_TRUNC, H5P_DEFAULT, plist_id);

        for (; w != GetVariables().end(); w++) {

            shared_ptr<Variable> v = w->lock();

            // 1.5 Getting the dimention of the variable
            int varDimention = v->GetLayout()->GetDimensions();
            cout << "var dimention is: " << varDimention << endl;

            // 1.6 Create a array for current dimentions
            hsize_t *global_dims;
            hsize_t *local_dims;
            global_dims = new (nothrow) hsize_t[varDimention];
            local_dims = new (nothrow) hsize_t[varDimention];

            if ((global_dims == NULL) || (local_dims == NULL)) {
                ERROR("Failed to allocate memory for dim arrays!");
            }

            // 2. Create DataStace
            for (int i = 0; i < varDimention; i++) {
                global_dims[i] = v->GetLayout()->GetGlobalExtentAlong(i);
                local_dims[i] = v->GetLayout()->GetExtentAlong(i);
            }
            cout << "global_dim is: " << global_dims[0] << "," << global_dims[1] << endl;
            cout << "local_dim is: " << local_dims[0] << "," << local_dims[1] << endl;


            if ((filespace = H5Screate_simple(varDimention, local_dims , global_dims)) < 0)
                ERROR("file space creation failed !");

            cout << "Ready to Get the Type of vairable. FileSpace is: " << filespace << endl;

            //2.1. Get Variable Type
            if (!GetHDF5Type(v->GetLayout()->GetType(), dtype_id))
                ERROR("HDF5:Unknown variable type " << v->GetLayout()->GetType());

            cout << "Ready to Create Dataset with Name: " << v->GetName() << endl;

            // 3. Create Dataset
            dset_id = H5Dcreate(file_id, v->GetName().c_str() , dtype_id , filespace,
                                H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            if (dset_id <0)
                ERROR("Failed to create dataset ... ");

            H5Sclose(filespace);

            BlocksByIteration::iterator begin;
            BlocksByIteration::iterator end;
            v->GetBlocksByIteration(iteration, begin, end);
            int numBlocks = 0;

            for (BlocksByIteration::iterator bid = begin; bid != end; bid++) {

                numBlocks++;

                shared_ptr<Block> b = *bid;
                int blockDimention = b->GetDimensions();
                for (int i = 0; i < blockDimention; i++)
                    b->GetGlobalExtent(i);

                hsize_t *block_offset = new (nothrow) hsize_t[blockDimention];
                hsize_t *block_dim = new (nothrow) hsize_t[blockDimention];
                if (block_offset == NULL || block_dim == NULL)
                    ERROR("HDF5:Failed to allocate memory ");

                // Obtain the starting indices of the hyperslab
                for (int i = 0; i < blockDimention; i++) {
                    block_dim[i] = b->GetEndIndex(i) - b->GetStartIndex(i) + 1;
                    block_offset[i] = b->GetStartIndex(i);
                }
                cout << "Block Dimention is: " << blockDimention << endl;
                cout << "block_dim is: " << block_dim [0] << "," << block_dim[1] << endl;
                cout << "block Offset is: " << block_offset [0] << "," << block_offset[1] << endl;

                // 4. create Memory data space
                memspace = H5Screate_simple(blockDimention, block_dim , NULL);

                // 5. Select hyperslab in the file.
                filespace = H5Dget_space(dset_id);
                H5Sselect_hyperslab(filespace, H5S_SELECT_SET, block_offset , NULL, block_dim , NULL);


                // 6. Getting the data
                void *ptr = b->GetDataSpace().GetData();


                // 7. Writing data
                if (H5Dwrite(dset_id,        //Identifier of the dataset to write to
                         dtype_id,        // Identifier of the memory datatype.
                         memspace,       // Identifier of the memory dataspace.
                         filespace,      // Identifier of the dataset's dataspace in the file.
                         plist_id,       // Identifier of a transfer property list for this I/O operation.
                         ptr) <0)          // Buffer with data to be written to the file.
                    ERROR("Writing Data Failed !");


                // 8 Free evertything
                delete [] block_dim;
                delete [] block_offset;

                // 9. Close handles
                //H5Sclose(filespace);
                H5Sclose(memspace);

            } // for of block iteration
            //H5Dclose(dset_id);
            cout << "Number of blocks is:" << numBlocks << endl;
        } // for of variable iteration

        H5Fclose(file_id);
    }

    void HDF5Store::OutputCollective(int32_t iteration) {
        hid_t file_id;
        hid_t lcpl_id;
        hid_t dset_id;
        hid_t dtype_id = -1;
        hid_t filespace;
        hid_t memspace;
        hid_t plist_id = H5P_DEFAULT; // property list identifier
        string fileName;


        std::vector< weak_ptr<Variable> >::const_iterator w = GetVariables().begin();


        fileName = GetOutputFileName(iteration);

        MPI_Comm comm = Environment::GetEntityComm();
	    MPI_Info info  = MPI_INFO_NULL; // **MSB need to be able to set 'info' parameters

	    plist_id = H5Pcreate(H5P_FILE_ACCESS);
	    H5Pset_fapl_mpio(plist_id, comm, info);


        if((file_id = H5Fcreate(fileName.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, plist_id)) < 0)
            ERROR("HDF5: H5Fcreate Failed ");

        H5Pclose(plist_id);

        lcpl_id = H5Pcreate (H5P_LINK_CREATE);
        H5Pset_create_intermediate_group(lcpl_id, 1);

        for(; w != GetVariables().end(); w++) {
	        shared_ptr<Variable> v = w->lock();

	        int varDimention;
	        varDimention = v->GetLayout()->GetDimensions();

	        hsize_t *global_dim;
	        global_dim = new (nothrow) hsize_t[varDimention];
	        if (global_dim == NULL) {
	            ERROR("HDF5:Failed to allocate memory ");
            }

	        hsize_t *local_dim;
	        local_dim = new (nothrow) hsize_t[varDimention];
	        if (local_dim == NULL) {
	            ERROR("HDF5:Failed to allocate memory ");
	        }


	        for(int i = 0; i < varDimention; i++) {
	            global_dim[i] = v->GetLayout()->GetGlobalExtentAlong(i);
	            local_dim[i] = v->GetLayout()->GetExtentAlong(i);
	        }

	        // Create file dataspace.
	        filespace = H5Screate_simple(varDimention, global_dim , NULL);

            GetHDF5Type(v->GetLayout()->GetType() , dtype_id);


            dset_id = H5Dcreate( file_id, v->GetName().c_str(), dtype_id, filespace,
                     lcpl_id, H5P_DEFAULT, H5P_DEFAULT);

            H5Sclose(filespace);

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

                memspace = H5Screate_simple(varDimention, local_dim , NULL);

                // Select hyperslab in the file.
                filespace = H5Dget_space(dset_id);
                H5Sselect_hyperslab(filespace, H5S_SELECT_SET, offset, NULL, local_dim , NULL);

                // Create property list for collective dataset write.
                plist_id = H5Pcreate(H5P_DATASET_XFER);
                H5Pset_dxpl_mpio(plist_id, H5FD_MPIO_COLLECTIVE);

                void* ptr = b->GetDataSpace().GetData();

                H5Dwrite(dset_id, dtype_id, memspace, filespace, plist_id, ptr);

                H5Sclose(filespace);
                H5Sclose(memspace);
                H5Pclose(plist_id);

                delete [] offset;
            }
            delete [] local_dim;
            delete [] global_dim;
            H5Dclose(dset_id);
        }
    }
}

