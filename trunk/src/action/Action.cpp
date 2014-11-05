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
#include "util/Debug.hpp"
#include "action/Action.hpp"

namespace damaris {

void Action::operator()( int32_t sourceID, int32_t iteration, const char* args)
{
	Call(sourceID,iteration,args);
}

#ifdef HAVE_VISIT_ENABLED
bool Action::ExposeVisItMetaData(visit_handle h)
{
	visit_handle cmd = VISIT_INVALID_HANDLE;
	if(not IsExternallyVisible()) return false;
	if(VisIt_CommandMetaData_alloc(&cmd) == VISIT_OKAY) {
		VisIt_CommandMetaData_setName(cmd, GetName().c_str());
		VisIt_SimulationMetaData_addGenericCommand(h, cmd);
		return true;
	}
	return false;
}
#endif

}
