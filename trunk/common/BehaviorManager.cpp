#include <stdint.h>
#include <string>
#include <list>
#include <iostream>

#include <hdf5.h>

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
	
	herr_t status;
	
        hid_t dataset_id, dataspace_id, chunk_id;
        hid_t file_id, group_id;
        
	char filename[128];
	char dsetname[128];
	char groupname[128];

	unsigned int gzip_filter_values[1];
	gzip_filter_values[0] = 4;
	
	// create the file
        sprintf(filename,"cm1out%d.h5",(int)step);
	file_id = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

	// create group
	sprintf(groupname,"/proc%05d",0);	
	group_id = H5Gcreate(file_id, groupname, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	hsize_t dims[3], chunkdims[3];
	
        Damaris::Variable* v = NULL;
       
	std::list<Damaris::Variable*> *lv = db->getAllVariables();
        std::list<Damaris::Variable*>::iterator i;
        
	for(i = lv->begin();i != lv->end(); i++)
        {
                v = (*i);
		Damaris::Layout* ly = v->getLayout();
                
	// for each variable v
	if(v != NULL){
	if(v->getIteration() == step)
	switch(ly->getDimensions()){
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		chunkdims[0] = dims[0] = ly->getExtentAlongDimension(0);
		chunkdims[1] = dims[1] = ly->getExtentAlongDimension(1);
		chunkdims[2] = dims[2] = ly->getExtentAlongDimension(2);
		chunk_id = H5Pcreate(H5P_DATASET_CREATE);
		H5Pset_chunk(chunk_id,ly->getDimensions(),chunkdims);
		H5Pset_filter(chunk_id,1,0,1,gzip_filter_values);
		dataspace_id = H5Screate_simple(ly->getDimensions(), dims, NULL);
		sprintf(dsetname,"%s",v->getName()->c_str());
		printf(" ---------- writing %s -- dim are %d,%d,%d\n",v->getName()->c_str(),(int)dims[0],(int)dims[1],(int)dims[2]);
		dataset_id = H5Dcreate(group_id, dsetname,H5T_NATIVE_FLOAT, dataspace_id, chunk_id, H5P_DEFAULT, H5P_DEFAULT);
		H5Dwrite(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT,v->getDataAddress());
		H5Dclose(dataset_id);
		break;
	}
	//db->remove(v);
	// end for each
	} // if
	} // for 

	H5Fclose(file_id);
	waiting = 0;
	}
}

