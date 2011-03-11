#include <stdint.h>
#include <string>
#include <list>
#include <iostream>
#include "hdf5.h"

#include "common/Debug.hpp"
#include "common/Configuration.hpp"
#include "common/Util.hpp"
#include "common/MetadataManager.hpp"
#include "common/Variable.hpp"
#include "common/BehaviorManager.hpp"

void hdf5(const std::string* event, int32_t step, int32_t src, Damaris::MetadataManager* db);

namespace Damaris {
	
BehaviorManager::BehaviorManager(MetadataManager *mm)
{
	metadataManager = mm;
	// Tests floowing
	Reaction *r = new Reaction(&hdf5);
	reactions.insert(std::pair<std::string,Reaction*>(std::string("hdf5"),r));
	std::map<std::string,Reaction*>::iterator it = reactions.find(std::string("hdf5"));
}

void BehaviorManager::reactToPoke(std::string *poke, int32_t iteration, int32_t sourceID)
{
	std::map<std::string,Reaction*>::iterator it = reactions.find(*poke);
	if(it != reactions.end())
	{
		Reaction* r = (*it).second;
		(*r)(poke,iteration,sourceID,metadataManager);
	} else {
		LOGF("Unable to process \"%s\" poke.\n",poke->c_str())
	}	
}
	
}

void hdf5(const std::string* event, int32_t step, int32_t src, Damaris::MetadataManager* db)
{
	static int waiting;
	waiting++;

	if(waiting == 1) {
	TIMER_START(write_time)	
        hid_t dataset_id, dataspace_id, chunk_id;
        hid_t file_id, group_id;
        
	char filename[128];
	char dsetname[128];
	char groupname[128];

	unsigned int gzip_filter_values[1];
	gzip_filter_values[0] = 4;
	
	int serverID = Damaris::Configuration::getID();
	// create the file
        sprintf(filename,"cm1out.%d.%d.h5",(int)step,serverID);
	file_id = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

	// create group
	sprintf(groupname,"/proc%05d",0);	
	group_id = H5Gcreate(file_id, groupname, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	hsize_t dims[3], chunkdims[3];
	
        Damaris::Variable* v = NULL;
       
	std::list<Damaris::Variable*> *lv = db->getAllVariables();
        std::list<Damaris::Variable*>::iterator i;
        
	for(i = lv->begin();i != lv->end();)
        {
                v = (*i);
		Damaris::Layout* ly = v->getLayout();
                
	// for each variable v
	if(v != NULL){
	//if(v->getIteration() == step)
	switch(ly->getDimensions()){
	case 0:
		break;
	case 1:
		break;
	case 2:
		chunkdims[0] = dims[0] = ly->getExtentAlongDimension(0);
                chunkdims[1] = dims[1] = ly->getExtentAlongDimension(1);
                dataspace_id = H5Screate_simple(ly->getDimensions(), dims, NULL);
                sprintf(dsetname,"%s",v->getName()->c_str());
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
		sprintf(dsetname,"%s",v->getName()->c_str());
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

	H5Fclose(file_id);
	H5close();
	waiting = 0;
	TIMER_STOP(write_time,"end writing")
	}
}

