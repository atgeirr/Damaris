/*******************************************************************
This file is part of Damaris.

Damaris is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Damaris is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Damaris.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
/**
 * \file HDF5Writer.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#include <stdint.h>
#include <string>
#include <list>
#include <iostream>
#include <hdf5.h>
#include "common/Debug.hpp"
#include "common/Process.hpp"
#include "common/Variable.hpp"

extern "C" {

void hdf5_dump(const std::string& event, int32_t step, int32_t src)
{

	TIMER_START(write_time)	
        hid_t dataset_id, dataspace_id, chunk_id;
        hid_t file_id, group_id;
        
	char filename[128];
	char dsetname[128];
	char groupname[128];

	Damaris::Process* p = Damaris::Process::get();
	int serverID = p->getEnvironment()->getID();

	// create the file
	std::string basename = "";
        sprintf(filename,"%s.%d.%d.h5",basename.c_str(),(int)step,serverID);
//	file_id = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

	// create group
	sprintf(groupname,"/proc");
//	group_id = H5Gcreate(file_id, groupname, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

printf("Coucou from inside an inner function!");
//	Damaris::VariableSet &vset = p->getMetadataManager()->getVariableSet();
//	Damaris::VariableIndex it = vset;
        
//	for(; it != vset.end(); it++)
  //      {
    //            Damaris::Variable* v = (*it);
/**		write_variable_hdf5(
		Damaris::Layout* ly = v->getLayout();
		
		int d = ly->getDimensions();
		hsize_t *dims = new hsize_t[ly->getDimensions()];
               	for(int j = 0; j < ly
		chunkdims[0] = dims[0] = ly->getExtentAlongDimension(0);
                chunkdims[1] = dims[1] = ly->getExtentAlongDimension(1);
                dataspace_id = H5Screate_simple(ly->getDimensions(), dims, NULL);
                sprintf(dsetname,"%d-%s",v->getSource(),v->getName()->c_str());
                dataset_id = H5Dcreate(group_id, dsetname,H5T_NATIVE_FLOAT, dataspace_id, H5P_DEFAULT,H5P_DEFAULT,H5P_DEFAULT);
                H5Dwrite(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT,v->getDataAddress());
                H5Dclose(dataset_id);
		break;
	case 3:
		chunkdims[0] = dims[0] = ly->getExtentAlongDimension(0);
		chunkdims[1] = dims[1] = ly->getExtentAlongDimension(1);
		chunkdims[2] = dims[2] = ly->getExtentAlongDimension(2);
		chunk_id = H5Pcreate(H5P_DATASET_CREATE);
		H5Pset_chunk(chunk_id,ly->getDimensions(),chunkdims);
#ifdef __ENABLE_COMPRESSION
		H5Pset_filter(chunk_id,1,0,1,gzip_filter_values);
#endif
		dataspace_id = H5Screate_simple(ly->getDimensions(), dims, NULL);
		sprintf(dsetname,"%d-%s",v->getSource(),v->getName()->c_str());
		dataset_id = H5Dcreate1(group_id, dsetname,H5T_NATIVE_FLOAT, dataspace_id, chunk_id);
		H5Dwrite(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT,v->getDataAddress());
		H5Dclose(dataset_id);
		break;
	}
	i++;
	db->remove(v);
	// end for each
	} // if
	} // for 
*/
	//H5Fclose(file_id);
	//H5close();
	TIMER_STOP(write_time,"end writing")

}

}
