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
 * \file NodeAction.hpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#ifndef __DAMARIS_NODE_ACTION_H
#define __DAMARIS_NODE_ACTION_H

#include <dlfcn.h>
#include <stdint.h>
#include <string>
#include <map>

#include "common/Action.hpp"
#include "common/MetadataManager.hpp"

namespace Damaris {

class ActionsManager;
/**
 * The NodeAction object is a wrapper for an action that has to wait for
 * each core of a node to call it before actually firing the action.
 */

class NodeAction : public Action {
	friend class ActionsManager;

	private:
		Action* base; /*!< base action to call */
		std::map<int,int> locks; /*!< This map associates an iteration with the number of
					      clients that have fired the event for this iteration. */
		int clientsPerNode; /*!< Number of clients in each node (to know who to wait for) */

	public:
		/**
		 * \brief Constructor. 
		 * \param[in] b : Base action to wrap.
		 * \param[in] n : Number of clients to wait for (clients per node).
		 */
		NodeAction(Action* b, int n);
		
		/**
		 * \brief Destructor.
		 */
		~NodeAction();

		/**
		 * \brief Call the action.
		 * \param[in] iteration : iteration at which the action is called.
		 * \param[in] sourceID : ID of the client that fired the action.
		 * \param[in,out] mm : pointer to the MetadataManager that contains 
		 *                     all recorded variables.
		 * \see Damaris::Actions::operator()
		 */
		void call(int32_t iteration, int32_t sourceID);

		/**
		 * \brief Loads required resources for the action to be called. 
		 */
		void load() { if(base != NULL) base->load();}
};

}

#endif
