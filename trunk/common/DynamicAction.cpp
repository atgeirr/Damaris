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
 * \date July 2011
 * \author Matthieu Dorier
 * \version 0.1
 */
#include "common/Debug.hpp"
#include "common/DynamicAction.hpp"

namespace Damaris {

	DynamicAction::DynamicAction(std::string fun, std::string file)
	: Action()
	{
		funname = fun;
		filename = file;
		handle = NULL;
		function = NULL;
	}
	
	DynamicAction::DynamicAction(std::string n, int i, std::string fun, std::string file)
	: Action(n,i)
	{
		funname = fun;
		filename = file;
		handle = NULL;
		function = NULL;
	}

	DynamicAction::~DynamicAction()
	{
		dlclose(handle);
	}
	
	void DynamicAction::call(int32_t iteration, int32_t sourceID, MetadataManager* mm)
	{
		if(!loaded)
			load();
		if(function != NULL)
			(*function)(name,iteration,sourceID,mm);
	}

	void DynamicAction::load()
	{
		if(loaded) return;

		char* error;
		void* handle = dlopen(filename.c_str(),RTLD_NOW | RTLD_GLOBAL);
		
		if(!handle)
		{
			ERROR("While loading plugin in \"" << filename.c_str() << "\":" << dlerror());
			return;
		}

		/* loading function */
		function = (void (*)(std::string,int32_t, int32_t, Damaris::MetadataManager*))dlsym(handle,funname.c_str());

		if ((error = dlerror()) != NULL)  {
			ERROR("While loading function in dynamic library: " << error);
			return;
		}
	}
}
