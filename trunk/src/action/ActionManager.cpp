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
#include "action/ActionManager.hpp"
#include "action/DynamicAction.hpp"
#include "action/ScriptAction.hpp"
#include "action/BoundAction.hpp"
#include "gc/GarbageCollector.hpp"

namespace damaris {

void ActionManager::Init(const model::Actions& mdl)
{
	// find the handler's name
	std::string handler_name;
	bool handler_is_event = true;

	model::Actions::error_const_iterator r(mdl.error().begin());
	if(r != mdl.error().end()) {
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
		if(r != mdl.error().end()) {
			WARN("Multiple definition of error handlers, "
			<< "only the first one will be registered.");
		}
	} else {
		Add(GarbageCollector::New());
	}

	// build events
	model::Actions::event_const_iterator e(mdl.event().begin());
	for(; e != mdl.event().end(); e++) {
		Create<DynamicAction>(*e,(std::string)e->name());
		if(handler_is_event && ((std::string)e->name() == handler_name))
		{
			Create<DynamicAction>(*e,"#error");
		}
	}

	// build scripts
	model::Actions::script_const_iterator s(mdl.script().begin());
	for(; s != mdl.script().end(); s++) {
		Create<ScriptAction>(*s,(std::string)s->name());
		if(!handler_is_event && (std::string)s->name() == handler_name) 
		{
			Create<ScriptAction>(*s,"#error");
		}
	}
	
	if(not Search("#error")) {
		WARN("Error handler not found, switching back to default"
		<< " garbage collector.");
		Add(GarbageCollector::New());
	}

}
}
