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

namespace Damaris {

void ActionManager::Init(const Model::Actions& model)
{
	// build events
	Model::Actions::event_const_iterator e(model.event().begin());
	for(; e != model.event().end(); e++) {
		Create<DynamicAction>(*e,(std::string)e->name());
	}

	// build scripts
	Model::Actions::script_const_iterator s(model.script().begin());
	for(; s != model.script().end(); s++) {
		Create<ScriptAction>(*s,(std::string)s->name());
	}
}

void ActionManager::reactToUserSignal(const std::string &sig,
				int32_t iteration, int32_t sourceID)
{
	Action* a = Search(sig);
	if(a != NULL)
	{
		(*a)(iteration,sourceID);
	} else {
		ERROR("Unable to process \""<< sig <<"\" signal: unknown event name");
	}
}

void ActionManager::reactToUserSignal(int id,
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
