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
 * \file NodeAction.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#include "core/Debug.hpp"
#include "core/NodeAction.hpp"

namespace Damaris {

	NodeAction::NodeAction(Action* a, int clpn)
	: Action()
	{
		base = a;
		clientsPerNode = clpn;
		DBG("Clients per node = " << clpn);
	}
	
	NodeAction::~NodeAction()
	{
		delete base;
	}
	
	void NodeAction::call(int32_t iteration, int32_t sourceID)
	{
		DBG("Node action called iteration is "<<iteration<<" and source is "<<sourceID);
		locks[iteration] = locks[iteration] + 1;
		if(locks[iteration] == clientsPerNode) {
			if(base != NULL) {
				DBG("calling base action");
				base->call(iteration,sourceID);
			}
			locks.erase(iteration);
		}
	}
}
