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
#include "model/Model.hpp"
#include "util/Debug.hpp"
#include "env/Environment.hpp"
#include "action/GroupAction.hpp"
#include "action/DynamicAction.hpp"

namespace damaris {

void DynamicAction::Call(int32_t sourceID, int32_t iteration, const char* args)
{
	if(!loaded_)
		Load();
	if(function_ != NULL)
		(*function_)(GetName().c_str(), sourceID, iteration, args);
}

void DynamicAction::Load()
{
	if(loaded_) return;

	char* error;
	handle_ = NULL;
	if(GetModel().library() == "") {
		handle_ = dlopen(NULL,RTLD_NOW | RTLD_GLOBAL);
	} else {
		handle_ = dlopen(GetModel().library().c_str(), 
					RTLD_NOW | RTLD_GLOBAL);
		if(!handle_)
		{
			ERROR("While loading plugin in \"" 
				<< GetModel().library().c_str() 
				<< "\":" << dlerror());
			return;
		}
	}

	/* loading function */
	function_ = (signal_t)dlsym(handle_,GetModel().action().c_str());

	if ((error = dlerror()) != NULL)  {
		ERROR("While loading function : " << error);
		return;
	}
	loaded_ = true;
}

}
