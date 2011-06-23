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
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <list>

#include "common/Layout.hpp"
#include "common/Variable.hpp"

using namespace boost::interprocess;
//using boost::multi_index_container;
//using namespace boost::multi_index;

namespace Damaris {
/* TODO : this is for a later use of Boost::MultiIndex
	struct name{};
	struct source{};
	struct iteration{};

	typedef multi_index_container<
		Variable,
		indexed_by<
			ordered_non_unique<
				tag<name>,  BOOST_MULTI_INDEX_MEMBER(Variable,std::string,name)>,
			ordered_non_unique<
				tag<source>,  BOOST_MULTI_INDEX_MEMBER(Variable,int32_t,source)>,
			ordered_non_unique<
				tag<iteration>, BOOST_MULTI_INDEX_MEMBER(Variable,int32_t,iteration)>
			>
		> variable_set;
*/
	class MetadataManager {
	private:
	//	variable_set vars;
		std::list<Variable> vars;
		managed_shared_memory* segment;
	public:
		Variable* get(const std::string *n, int32_t it, int32_t srcID);
		void put(Variable v);
	//	void put(std::string *name, int32_t iteration, int32_t sourceID, Layout* l, void* data);
	//	void remove(std::string* name, int32_t iteration, int32_t sourceID);	
		void remove(Variable v);

		// TODO getVariablesByName(std::string n);

		// TODO getVariablesBySource(int32_t src);

		// TODO getVariablesByIteration(int32_t it);
		
		MetadataManager(managed_shared_memory* s);
	//	std::list<Variable*>* getAllVariables();
		~MetadataManager();
	};
	
}

#endif
