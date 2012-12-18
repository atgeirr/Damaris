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
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_NODE_ACTION_H
#define __DAMARIS_NODE_ACTION_H

#include <stdint.h>
#include <string>
#include <map>

#include "core/Debug.hpp"
#include "core/Environment.hpp"

namespace Damaris {

/**
 * The NodeAction object is a wrapper for an action that has to wait for
 * each core of a node to call it before actually firing the action.
 */
template <class BASE, typename MODEL>
class NodeAction : public BASE {

	private:
		std::map<int,int> locks; /*!< This map associates an iteration with the number of
								   clients that have fired the event for this iteration. */
		int clientsPerNode; /*!< Number of clients in each node (to know who to wait for) */

		/**
		 * \brief Constructor. 
		 * \param[in] b : Base action to wrap.
		 * \param[in] n : Number of clients to wait for (clients per node).
		 */
		NodeAction(const MODEL& mdl, const std::string& name, int n)
		: BASE(mdl,name) {
			clientsPerNode = n;
		}
		
	public:
		/**
		 * \brief Call the action.
		 * \param[in] iteration : iteration at which the action is called.
		 * \param[in] sourceID : ID of the client that fired the action.
		 * \see Damaris::Action::operator()
		 */
		virtual void Call(int32_t iteration, int32_t sourceID, const char* args = NULL)
		{
			if(sourceID == -1) { // case of an external call
				BASE::Call(iteration,sourceID,args);
				return;
			}

			if(not Environment::HasServer()) { // if the environment doesn't have a server, we don't wait for other
				BASE::Call(iteration,sourceID,args);
				return;
			}

			DBG("Node action called iteration is "<<iteration<<" and source is "<<sourceID);
			locks[iteration] = locks[iteration] + 1;
			DBG("lock is now " << locks[iteration] << "/" << clientsPerNode);
			if(locks[iteration] == clientsPerNode) {
				DBG("calling base action");
				BASE::Call(iteration,sourceID,args);
				locks.erase(iteration);
			}
		}

		static BASE* New(const MODEL& mdl, const std::string& name, int n)
		{
			return new NodeAction<BASE,MODEL>(mdl,name,n);
		}

		virtual ~NodeAction() {}
};

}

#endif
