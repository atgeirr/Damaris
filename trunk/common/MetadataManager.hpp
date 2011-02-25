#ifndef __DAMARIS_METADATA_H
#define __DAMARIS_METADATA_H

#include <boost/interprocess/managed_shared_memory.hpp>
#include <list>

#include "common/Layout.hpp"
#include "common/Variable.hpp"

using namespace boost::interprocess;

namespace Damaris {
	
	class MetadataManager {
	private:
		std::list<Variable*> vars;
		managed_shared_memory* segment;
	public:
		Variable* get(std::string *name, int32_t iteration, int32_t sourceID);
		void put(Variable* v);
		void put(std::string *name, int32_t iteration, int32_t sourceID, Layout* l, void* data);
		void remove(std::string* name, int32_t iteration, int32_t sourceID);	
		void remove(Variable* v);
		MetadataManager(managed_shared_memory* s);
		std::list<Variable*>* getAllVariables();
		~MetadataManager();
	};
	
}

#endif
