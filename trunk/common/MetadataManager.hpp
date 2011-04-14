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
