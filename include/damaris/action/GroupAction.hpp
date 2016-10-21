/**************************************************************************
This file is part of Damaris.

Damaris is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Damaris is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Damaris.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/
#ifndef __DAMARIS_GROUP_ACTION_H
#define __DAMARIS_GROUP_ACTION_H

#include <stdint.h>
#include <string>
#include <map>

#include "damaris/util/Debug.hpp"
#include "damaris/env/Environment.hpp"

namespace damaris {

/**
 * The GroupAction object is a wrapper for an action that has to wait for
 * each clients connected to the same server to call it before actually 
 * triggering the action.
 */
template <class BASE, typename MODEL>
class GroupAction : public BASE {
	
	friend class Deleter<GroupAction<BASE,MODEL> >;

	private:
	std::map<int,int> locks_; /*!< This map associates an iteration 
				with the number of clients that 
				have fired the event for this iteration. */

	/**
	 * Constructor. 
	 */
	GroupAction(const MODEL& mdl)
	: BASE(mdl)
	{}
		
	/**
	 * Destructor.
	 */
	virtual ~GroupAction() {}
		
	public:
		
	/**
	 * \see damaris::Action::operator()
	 */
	virtual void Call(int32_t source, int32_t iteration,
				const char* args = NULL)
	{
		if(source == -1) { // case of an external call
			BASE::Call(source,iteration,args);
			return;
		}

		if((not Environment::HasServer()) 
		|| (BASE::GetExecLocation() == model::Exec::local)) { 
			// if the environment doesn't have a server
			// or the execution is local
			// we don't wait for other to send the event
			BASE::Call(source,iteration,args);
			return;
		}

		locks_[iteration] += 1;

		int num_clients = Environment::GetKnownLocalClients().size();

		if(locks_[iteration] >= num_clients) {
			//(Environment::ClientsPerNode()
			// /Environment::ServersPerNode())) {
			DBG("calling base action");
			BASE::Call(source,iteration,args);
			locks_.erase(iteration);
		}
	}

	static shared_ptr<BASE> New(const MODEL& mdl)
	{
		return shared_ptr<BASE>(
				new GroupAction<BASE,MODEL>(mdl),
				Deleter<GroupAction<BASE,MODEL> >());
	}

	static shared_ptr<BASE> New(const MODEL& mdl, const std::string& name)
	{
		return New(mdl);
	}
};

}

#endif
