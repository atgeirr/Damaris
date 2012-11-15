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
 * \file DynamicAction.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#include <dlfcn.h>
#include "xml/Model.hpp"
#include "core/Debug.hpp"
#include "core/Environment.hpp"
#include "event/NodeAction.hpp"
#include "event/DynamicAction.hpp"

namespace Damaris {

	DynamicAction::DynamicAction(const Model::Event& mdl, const std::string &name)
	: Action(name), Configurable<Model::Event>(mdl)
	{
		handle 		= NULL;
		function	= NULL;
		loaded		= false;
	}

	DynamicAction::~DynamicAction()
	{
		if(handle != NULL)
			dlclose(handle);
	}

	void DynamicAction::Call(int32_t iteration, int32_t sourceID, const char* args)
	{
		if(!loaded)
			Load();
		if(function != NULL)
			(*function)(GetName(),iteration,sourceID,args);
	}

	void DynamicAction::Load()
	{
		if(loaded) return;

		char* error;
		void* handle = NULL;
		if(model.library() == "") {
			handle = dlopen(NULL,RTLD_NOW | RTLD_GLOBAL);
		} else {
			handle = dlopen(model.library().c_str(), RTLD_NOW | RTLD_GLOBAL);
			if(!handle)
			{
				ERROR("While loading plugin in \"" << model.library().c_str() 
						<< "\":" << dlerror());
				return;
			}
		}

		/* loading function */
		function = (void (*)(const std::string&,int32_t, int32_t, const char*))
			dlsym(handle,model.action().c_str());

		if ((error = dlerror()) != NULL)  {
			ERROR("While loading function : " << error);
			return;
		}
	}

	DynamicAction* DynamicAction::New(const Model::Event& ev, const std::string& name)
	{
		if(ev.scope() == Model::Scope::core)
			return new DynamicAction(ev,name);
		else if(ev.scope() == Model::Scope::node)
			return NodeAction<DynamicAction,Model::Event>::New(ev,name,Environment::ClientsPerNode());
		else {
			CFGERROR("Scope \"" << ev.scope() << "\" is not implemented for event \"" << name << "\".");
		}
		return NULL;
	}
}
