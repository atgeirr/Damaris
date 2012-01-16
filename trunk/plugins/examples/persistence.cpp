#include "hdf5.h"
#include <stdio.h>
#include "server/ServerConfiguration.hpp"
#include "common/MetadataManager.hpp"

extern "C" {

void dump_hdf5(const std::string* event, int32_t step, int32_t src, Damaris::MetadataManager* db)
{
	Damaris::ServerConfiguration* config = Damaris::ServerConfiguration::getInstance();
	int nb_clients = config->getClientsPerNode();
	static int waiting;
	waiting++;

	if(waiting == nb_clients) {
		waiting = 0;
        //TIMER_START(write_time) 
		hid_t dataset_id, dataspace_id, chunk_id;
		hid_t file_id;
		hid_t link_prop_id;
	
		char filename[128];
		char dsetname[128];

		unsigned int gzip_filter_values[1];
		gzip_filter_values[0] = 4;
        
		int serverID = config->getServerID();
        	// create the file
		sprintf(filename,"cm1out.%d.%d.h5",(int)step,serverID);
		file_id = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

		link_prop_id = H5Pcreate(H5P_LINK_CREATE);
		H5Pset_create_intermediate_group(link_prop_id,1);

		hsize_t dims[3], chunkdims[3];
        
		Damaris::Variable *v;
       
		std::list<Damaris::Variable> *lv = db->getAllVariables();
		std::list<Damaris::Variable>::iterator i;

		for(i = lv->begin();i != lv->end();)
		{
        	        v = &(*i);
			Damaris::Layout* ly = v->layout;
                
			//if(v->getIteration() == step)
			if(v != NULL) {
			switch(ly->getDimensions()){
			case 0:
				break;
			case 1:
				break;
			case 2:

				chunkdims[0] = dims[0] = ly->getExtentAlongDimension(0);
				chunkdims[1] = dims[1] = ly->getExtentAlongDimension(1);
				dataspace_id = H5Screate_simple(ly->getDimensions(), dims, NULL);
				
				if(v->name.c_str()[0] == '/')
					sprintf(dsetname,"%d%s",v->source,v->name.c_str());
               	 		else
					sprintf(dsetname,"%d/%s",v->source,v->name.c_str());
				dataset_id = H5Dcreate(file_id, dsetname,H5T_NATIVE_FLOAT, dataspace_id, link_prop_id,H5P_DEFAULT,H5P_DEFAULT);
				H5Dwrite(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT,v->data);
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
				if(v->name.c_str()[0] == '/')
					sprintf(dsetname,"%d%s",v->source,v->name.c_str());
				else
					sprintf(dsetname,"%d/%s",v->source,v->name.c_str());
				dataset_id = H5Dcreate(file_id, dsetname,H5T_NATIVE_FLOAT, dataspace_id, link_prop_id, chunk_id, H5P_DEFAULT);
				H5Dwrite(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT,v->data);
				H5Dclose(dataset_id);
				break;

			}
			i++;
			db->remove(*v);
			// end for each
		} // if
	} // for 

	H5Fclose(file_id);
	H5Pclose(link_prop_id);
	H5close();
	}
}

}
