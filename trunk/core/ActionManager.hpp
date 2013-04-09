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
 * \file ActionsManager.hpp
 * \date May 2012
 * \author Matthieu Dorier
 * \version 0.5
 */
#ifndef __DAMARIS_ACTION_MANAGER_H
#define __DAMARIS_ACTION_MANAGER_H

#include "xml/Model.hpp"
#include "core/Manager.hpp"
#include "event/Action.hpp"
#include "event/DynamicAction.hpp"
#include "event/ScriptAction.hpp"

namespace Damaris {

	/**
	 * ActionManager is a class that offers an access by name
	 * and by id to all actions managed by the program. It
	 * also exposes two functions to execute these actions.
	 * It inherites from Manage<Action>.
	 */
	class ActionManager : public Manager<Action> {

	public:
		/**
		 * Initialize the ActionManager with a model: goes through
		 * all the events and scripts described in the XML file,
		 * creates the appropriate Actions instances, and stores them.
		 */
		static void Init(const Model::Actions& mdl);

		/**
		 * Trigger a particular action identified by its name.
		 */
		static void ReactToUserSignal(const std::string &sig,
				int32_t iteration, int32_t sourceID);

		/**
		 * Trigger a particular action identified by its id.
		 */
		static void ReactToUserSignal(int id,
				int32_t iteration, int32_t sourceID);
	};
}

#endif
