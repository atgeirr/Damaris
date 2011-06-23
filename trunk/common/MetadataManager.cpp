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

#include "common/Debug.hpp"
#include "common/MetadataManager.hpp"

using namespace boost::interprocess;

namespace Damaris {

	MetadataManager::MetadataManager(managed_shared_memory* s)
	{
		segment = s;
	}
	
	Variable* MetadataManager::get(const std::string* name, int32_t iteration, int32_t sourceID)
	{
		std::list<Variable>::iterator i;
		// TODO : this function bug
		for(i=vars.begin(); i != vars.end(); i++)
		{
			bool c = true;
			c =  i->name.compare(*name) == 0;
			c = c && i->iteration == iteration;
			c = c && i->source == sourceID;
			if(c) return (Variable*)(&(*i));
		}
		return NULL;
	}
	
	void MetadataManager::put(Variable v)
	{
		vars.push_back(v);
		//vars.insert(v);
	}
/*	
	void MetadataManager::put(std::string* name, int32_t iteration, int32_t sourceID, Layout* l, void* data)
	{
		Variable* v = new Variable(name,iteration,sourceID,l,data);
		vars.push_back(v);
	}
	
	void MetadataManager::remove(std::string* name, int32_t iteration, int32_t sourceID)
	{
		Variable* v = get(name,iteration,sourceID);
		vars.remove(v);
	}
*/
	void MetadataManager::remove(Variable v)
	{
		if(v.getDataAddress() != NULL) 
		{
			segment->deallocate(v.getDataAddress());
			v.data = NULL;
		}
		vars.remove(v);
		INFO("removed variable \"" << v.name.c_str() << "\", available memory is now " << segment->get_free_memory());
	}	
/*	
	std::list<Variable*>* MetadataManager::getAllVariables()
	{
		return &vars;
	}
*/	
	MetadataManager::~MetadataManager()
	{
		vars.clear();
	}
	
}
