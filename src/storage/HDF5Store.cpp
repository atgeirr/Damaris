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
                } else if (op->compare("SingleFile") == 0) {
                    fileMode_ = SingleFile;
                    ERROR("SingleFile mode is not supported in this version!");
                    return false;
                } else {
                    ERROR("FileMode is undefined. ");
                    return false;
                }
            } else if (op->key()->compare("XDMFMode") == 0) {
                if (op->compare("NoIteration") == 0) {
                    xdmfMode_ = NoIteration;
                    ERROR("Generating the XMF file is not supported in this version.");
                } else if (op->compare("FirstIteration") == 0) {
                    xdmfMode_ = FirstIteration;
                } else if (op->compare("EveryIteration") == 0) {
                    xdmfMode_ = EveryIteration;
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

    void HDF5Store::Output(int32_t iteration) {

        hid_t file_id;
        hid_t lcpl_id;
        hid_t dset_id;
        hid_t dtype_id = -1;
        hid_t filespace;
        hid_t memspace;
        hid_t plist_id;             // property list identifier
        char filename[128];        // Limit of 128 lenght filenames

        int processId = Environment::GetEntityProcessID();
        std::string basename = Environment::GetSimulationName();
        std::vector<weak_ptr<Variable> >::const_iterator w = GetVariables().begin();

        // Set up file access property list
        plist_id = H5P_DEFAULT;

        // Initialie file name
        sprintf(filename, "%s%s.%d_%d.h5", path_.c_str() , basename.c_str(), (int) iteration, processId);

        if ((file_id = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, plist_id)) < 0)
            ERROR("HDF5: H5Fcreate Failed ");

        lcpl_id = H5Pcreate(H5P_LINK_CREATE);
        H5Pset_create_intermediate_group(lcpl_id, 1);

        for (; w != GetVariables().end(); w++) {
            shared_ptr<Variable> v = w->lock();
            if (v) {
                //Retrieve the server Id
                int source = Environment::GetEntityProcessID();
                std::cout << "Storing variable " << v->GetName() << " for iteration " << iteration << " Server Id: "
                          << source << std::endl;

                //Getting the dimention of the variable
                int varDimention;
                varDimention = v->GetLayout()->GetDimensions();

                // Create a array for current dimentions
                // do not throw an exception if mem allocation fails
                hsize_t *global_dims;
                global_dims = new(nothrow) hsize_t[varDimention];
                if (global_dims == NULL) {
                    ERROR("HDF5: Failed to allocate memory ");
                }

                // Creating an array for maximum dimentions of the array
                hsize_t *block_dims;
                block_dims = new(nothrow) hsize_t[varDimention];
                if (block_dims == NULL) {
                    ERROR("HDF5: Failed to allocate memory ");
                }

                //Fill the global and block dimentions
                for (int i = 0; i < varDimention; i++) {
                    global_dims[i] = v->GetLayout()->GetGlobalExtentAlong(i);
                    block_dims[i] = v->GetLayout()->GetExtentAlong(i);
                }

                // Create file dataspace. Setting maximum size to NULL sets the maximum size to be the current size.
                filespace = H5Screate_simple(varDimention, global_dims, NULL);

                if (!GetHDF5Type(v->GetLayout()->GetType(), dtype_id))
                    ERROR("HDF5:Unknown variable type " << v->GetLayout()->GetType());


                dset_id = H5Dcreate(file_id, v->GetName().c_str(), dtype_id, filespace,
                                    lcpl_id, H5P_DEFAULT, H5P_DEFAULT);

                H5Sclose(filespace);

                BlocksByIteration::iterator begin;
                BlocksByIteration::iterator end;

                v->GetBlocksByIteration(iteration, begin, end);

                for (BlocksByIteration::iterator bid = begin; bid != end; bid++) {

                    shared_ptr<Block> b = *bid;

                    hsize_t *offset;
                    offset = new(nothrow) hsize_t[varDimention];
                    if (offset == NULL)
                        ERROR("HDF5:Failed to allocate memory ");

                    // Obtain the starting indices of the hyperslab
                    for (int i = 0; i < varDimention; i++) {
                        offset[i] = b->GetStartIndex(i);
                    }

                    if ((memspace = H5Screate_simple(varDimention, block_dims, NULL)) < 0)
                        ERROR("HDF5: H5Screate_simple Failed");

                    // Select hyperslab in the file
                    if ((filespace = H5Dget_space(dset_id)) < 0)
                        ERROR("HDF5: H5Dget_space Failed");

                    if ((H5Sselect_hyperslab(filespace, H5S_SELECT_SET, offset, NULL, block_dims /* sure??? */, NULL)) < 0)
                        ERROR("HDF5: H5Sselect_hyperslab Failed");


                    void *ptr = b->GetDataSpace().GetData();

                    if (H5Dwrite(dset_id, dtype_id, memspace, filespace, H5P_DEFAULT, ptr) < 0)
                        ERROR("HDF5: H5Dwrite Failed");

                    H5Sclose(filespace);
                    H5Sclose(memspace);

                    delete offset;
                }

                delete global_dims;
                delete block_dims;

                H5Dclose(dset_id);
            }
        }

        H5Pclose(lcpl_id);
        H5Fclose(file_id);
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
}

