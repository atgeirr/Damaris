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
 * \version 0.4
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
	 */
	class ActionManager : public Manager<Action> {

	public:
		static void Init(const Model::Actions& mdl);

		static void reactToUserSignal(const std::string &sig,
				int32_t iteration, int32_t sourceID);

		static void reactToUserSignal(int id,
				int32_t iteration, int32_t sourceID);
	};
}

#endif
