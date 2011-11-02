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
 * \file ActionsManager.cpp
 * \date July 2011
 * \author Matthieu Dorier
 * \version 0.1
 */
#include <dlfcn.h>
#include <stdint.h>
#include <string>
#include <list>
#include <iostream>

#include "common/Debug.hpp"
#include "common/Language.hpp"
#include "common/ActionsManager.hpp"
#include "scripts/PyAction.hpp"

namespace Damaris {
	
ActionsManager::ActionsManager()
{
}

/* This function load an action from a dynamic library. 
 * eventName is the name to give to this action in the system.
 * fileName is the name of the dynamic library to load (.so file on Linux, .dylib on MacOSX)
 * functionName is the name of the function within the dynamic library to be loaded.
 * If the dynamic library does not exist, the function prints an error and returns.
 * If the function within the library does not exist, the function prints an error and returns.
 * Finally, if an action has already been loaded with the same event name, the function does
 * not overwrite it, it prints a warning and returns.
 */
void ActionsManager::addDynamicAction(const std::string& eventName, 
		const std::string& fileName, const std::string &functionName)
{

	// check if there is already an action with the same name recorded
	ActionSet::index<by_name>::type::iterator it = actions.get<by_name>().find(eventName);
	if(it != actions.get<by_name>().end()) {
		WARN("Inserting an action with a name identical to a another defined action");
		return;
	}

	// create the action
	DynamicAction* a = new DynamicAction(functionName,fileName);

	// attribute an ID and a name to the action
	a->id = actions.size();
	a->name = eventName;

	// put the action into the ActionsSet
	actions.insert(boost::shared_ptr<Action>((Action*)a));
}

void ActionsManager::addScriptAction(const std::string& name,
		const std::string& fileName, const std::string& language) 
{
	DBG("Adding script " << name);
	// check if there is already an action with the same name recorded
        ActionSet::index<by_name>::type::iterator it = actions.get<by_name>().find(name);
        if(it != actions.get<by_name>().end()) {
                WARN("Inserting an action with a name identical to another defined action");
                return;
        }
	DBG("A");
	Action* a = NULL;
	// create the action
	switch(Language::getLanguageFromString(&(language))) {
		case (Language::LG_PYTHON):
			a = new PyAction(fileName); break;
		default:
			ERROR("Undefined scripting language \"" << language 
					<< "\" for action \"" << name << "\"");
			return;
	}
	DBG("B");
	// attribute an ID and a name to the action
	a->id = actions.size();
	a->name = name;
	DBG("C");
	// put the action into the ActionsSet
	actions.insert(boost::shared_ptr<Action>(a));
}

void ActionsManager::reactToUserSignal(const std::string &sig, 
		int32_t iteration, int32_t sourceID, MetadataManager* mm)
{
	ActionSet::index<by_name>::type::iterator it = actions.get<by_name>().find(sig);
	if(it != actions.get<by_name>().end())
	{
		(*(it->get()))(iteration,sourceID,mm);
	} else {
		ERROR("Unable to process \""<< sig <<"\" signal: unknown event name");
	}	
}

void ActionsManager::reactToUserSignal(int sigID, 
		int32_t iteration, int32_t sourceID, MetadataManager* mm)
{
	ActionSet::index<by_id>::type::iterator it = actions.get<by_id>().find(sigID);
	if(it != actions.get<by_id>().end())
	{
		DBG("Reacting to use signal " << sigID);
		(*(it->get()))(iteration,sourceID,mm);
	} else {
		ERROR("Unknown action ID " << sigID 
			<<", may come from contract inconsistency between clients and servers");
	}
}

Action* ActionsManager::getAction(const std::string &name)
{
	ActionSet::index<by_name>::type::iterator it =
		actions.get<by_name>().find(name);
	if(it == actions.get<by_name>().end()) {
		return NULL;
	}
	return it->get();
}

Action* ActionsManager::getAction(int id)
{
        ActionSet::index<by_id>::type::iterator it =
                actions.get<by_id>().find(id);
        if(it == actions.get<by_id>().end()) {
                return NULL;
        }
        return it->get();
}

}

/* 
// This was to test with a plugin that writes the data in HDF5 
void hdf5(const std::string* event, int32_t step, int32_t src, Damaris::MetadataManager* db)
{

	static int waiting;
	waiting++;

	if(waiting == CCORES) {
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
	sprintf(groupname,"/proc");	
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

	H5Fclose(file_id);
	H5close();
	waiting = 0;
	TIMER_STOP(write_time,"end writing")
	}

}
*/
