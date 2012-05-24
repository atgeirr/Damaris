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
 * \file Action.cpp
 * \date February 2012 
 * \author Matthieu Dorier
 * \version 0.4
 */
#include "core/Debug.hpp"
#include "event/Action.hpp"

namespace Damaris {

	void Action::operator()(int32_t iteration, int32_t sourceID, const char* args)
	{
		call(iteration,sourceID,args);
	}

	bool Action::exposeVisItMetaData(visit_handle h)
	{
		visit_handle cmd = VISIT_INVALID_HANDLE;
		if(not IsExternallyVisible()) return false;
		DBG("AAAAA");
		if(VisIt_CommandMetaData_alloc(&cmd) == VISIT_OKAY) {
			VisIt_CommandMetaData_setName(cmd, getName().c_str());
			DBG("BBBBB");
			VisIt_SimulationMetaData_addGenericCommand(h, cmd);
			return true;
		}
		return false;
	}
}
