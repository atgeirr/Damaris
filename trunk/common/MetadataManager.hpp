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
 * \file MetadataManager.hpp
 * \date July 2011
 * \author Matthieu Dorier
 * \version 0.1
 *
 * MetadataManager holds pointers to all Variables published.
 * These variables can be retrieved by their identifier (name,source,iteration).
 */
#ifndef __DAMARIS_METADATA_H
#define __DAMARIS_METADATA_H

//#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <list>

#include "common/SharedMemorySegment.hpp"
#include "common/Layout.hpp"
#include "common/Variable.hpp"

//using namespace boost::interprocess;
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
		std::list<Variable> vars; /*!< List of all recorded variables. */
//		managed_shared_memory* segment; /*!< A pointer to the shared memory segment. */ 
		SharedMemorySegment* segment; /*!< A pointer to the shared memory segment. */
	public:
		/**
		 * \brief Retrieves a variable given its name, iteration and source.
		 * \param[in] n : name of the variable to retrieve.
		 * \param[in] it : iteration of the variable to retrieve.
		 * \param[in] srcID : source id of the variable to retrieve.
		 * \return A pointer to the first Variable record found, or NULL if none has been found.
		 *         Do not use delete on this pointer, it is managed by the MetadataManager.
		 */
		Variable* get(const std::string *n, int32_t it, int32_t srcID);
		
		/**
		 * \brief Puts a new variable record into the MetadataManager.
		 * \param[in] v : instance of the Variable record to register. A copy of this variable
		 *                is done, so you can safely delete the original one after calling this function.
		 * \return 0 in case of success, -1 if there is already a record identified by the same name, iteration and source.
		 *           In that case, the new Variable is not inserted in the MetadataManager.
		 */
		int put(Variable v);
	//	void put(std::string *name, int32_t iteration, int32_t sourceID, Layout* l, void* data);
	//	void remove(std::string* name, int32_t iteration, int32_t sourceID);	
		
		/**
		 * \brief Removes a variable from the MetadataManager (its content is deleted from shared memory).
		 * \param[in] v : instance of the variable to delete.
		 */
		void remove(Variable v);

		// TODO getVariablesByName(std::string n);

		// TODO getVariablesBySource(int32_t src);

		// TODO getVariablesByIteration(int32_t it);
		/**
		 * \brief Constructor.
		 * Takes a pointer to the shared memory segment that holds the data.
		 * \param[in] s : pointer to the shared memory segment.
		 */	
		MetadataManager(SharedMemorySegment* s);
		//MetadataManager(managed_shared_memory* s);
	
		std::list<Variable>* getAllVariables();

		/**
		 * \brief Destructor.
		 * Will iterate on each variable recorded and delete them (including freeing the memory).
		 */
		~MetadataManager();
	};
	
}

#endif
