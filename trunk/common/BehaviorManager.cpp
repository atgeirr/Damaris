#include <stdint.h>
#include <string>
#include <list>
#include <iostream>

//#include <hdf5.h>

#include "common/Configuration.hpp"
#include "common/Util.hpp"
#include "common/MetadataManager.hpp"
#include "common/Variable.hpp"
#include "common/BehaviorManager.hpp"

void hdf5(const std::string* event, int32_t step, int32_t src, Damaris::MetadataManager* db)
{
	std::list<Damaris::Variable*> *lv = db->getAllVariables();
	std::list<Damaris::Variable*>::iterator i;
	for(i = lv->begin();i != lv->end(); i++)
	{
		Damaris::Variable* v = (*i);
		v->print();
	}
}

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
/*
#define WRITE(vname) \
        std::cerr << " -- searching ni for step " << step << " source " << src << std::endl; \
        v = db->get(std::string(vname),step,src); \
        if(v != NULL) \
        std::cerr << " ------ YEAH ------- \n";
void hdf5(const std::string* event, int64_t step, int32_t src, Damaris::metadata* db)
{
	static int waiting;
	waiting++;

	if(waiting == CORE_PER_NODE) {
	
	herr_t status;
	
        hid_t dataset_id, dataspace_id, chunk_id;
        hid_t file_id;
        
	char filename[128];
        sprintf(filename,"cm1out%d.h5",(int)step);

	unsigned int gzip_filter_values[1];
	gzip_filter_values[0] = 4;

        file_id = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

	hsize_t dims[3], chunkdims[3];
	
        Damaris::variable* v = NULL;
        
	// for each variable v
	if(v != NULL)
	switch(v->getDimensions()){
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		dims[0] = v->getExtentAlongDimension(0);
		dims[1] = v->getExtentAlongDimension(1);
		dims[2] = v->getExtentAlongDimension(2);
		// TODO
		break;
	}
	// end for each

	H5Fclose(file_id);
	waiting = 0;
	}
}
*/
