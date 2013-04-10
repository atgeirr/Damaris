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
 * \file ActionManager.cpp
 * \date May 2012
 * \author Matthieu Dorier
 * \version 0.5
 */
#include "core/Debug.hpp"
#include "core/ActionManager.hpp"
#include "core/GarbageCollector.hpp"


namespace Damaris {

void ActionManager::Init(const Model::Actions& model)
{
	// find the handler's name
	std::string handler_name;
	bool handler_is_event = true;

	Model::Actions::error_const_iterator r(model.error().begin());
	if(r != model.error().end()) {
		// build the action for error handling
		if(r->event().present() && r->script().present()) {
			WARN("Error handler should be attached to either "
			<< "an event or a script.");
		}
		if(r->event().present()) {
			handler_name = r->event().get();
			handler_is_event = true;
		}
		if(r->script().present()) {
			handler_name = r->script().get();
			handler_is_event = false;
		}
		r++;
		if(r != model.error().end()) {
			WARN("Multiple definition of error handlers, "
			<< "only the first one will be registered.");
		}
	} else {
		Add(new GarbageCollector());
	}

	// build events
	Model::Actions::event_const_iterator e(model.event().begin());
	for(; e != model.event().end(); e++) {
		Create<DynamicAction>(*e,(std::string)e->name());
		if(handler_is_event && ((std::string)e->name() == handler_name))
		{
			Create<DynamicAction>(*e,"#error");
		}
	}

	// build scripts
	Model::Actions::script_const_iterator s(model.script().begin());
	for(; s != model.script().end(); s++) {
		Create<ScriptAction>(*s,(std::string)s->name());
		if(!handler_is_event && (std::string)s->name() == handler_name) 
		{
			Create<ScriptAction>(*s,"#error");
		}
	}

}

void ActionManager::ReactToUserSignal(const std::string &sig,
				int32_t iteration, int32_t sourceID)
{
	Action* a = Search(sig);
	if(a != NULL)
	{
		(*a)(iteration,sourceID);
	} else {
		if(sig[0] != '#') {
			ERROR("Unable to process \""<< sig <<"\" signal: unknown event name");
		}
	}
}

void ActionManager::ReactToUserSignal(int id,
				int32_t iteration, int32_t sourceID)
{
    Action* a = Search(id);
    if(a != NULL)
    {
        (*a)(iteration,sourceID);
    } else {
        ERROR("Unable to process signal id = "<< id <<" : unknown event id");
    }
}
}
