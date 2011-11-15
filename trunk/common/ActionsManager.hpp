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
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#ifndef __DAMARIS_ACTIONS_MANAGER_H
#define __DAMARIS_ACTIONS_MANAGER_H

#include <string>

#include "common/MetadataManager.hpp"
#include "common/Action.hpp"
#include "common/DynamicAction.hpp"
#include "common/ActionSet.hpp"
#include "common/Singleton.hpp"

namespace Damaris {

/**
 * The ActionsManager keeps the list of defined actions
 * It is in charge of loading functions from shared libraries,
 * from scripts, etc. and call there functions upon reception
 * of events.
 */
class ActionsManager : public Singleton<ActionsManager> {
	friend class Singleton<ActionsManager>;

	private:
		ActionSet actions; /*!< Set of actions indexed by ID and name. */

		/**
		 * \brief Constructor.
		 */
		ActionsManager();				
	public:
		/** 
		 * \brief Adds an action defined through a dynamic library.
		 * \param[in] eventName : Name of the event taken as a key for this action.
		 * \param[in] fileName : Name of the dynamic library (.so, .dylib...)
		 * \param[in] functionName : Name of the function to load in this dynamic library.
	 	 * 
		 * The action is defined but the dynamic library is not loaded. It will be loaded
		 * by the action the first time the action is called. This way the clients can
		 * have an instance of ActionsManager that just acts as a set of actions from which
		 * they can retrieve action's IDs.
		 */
		void addDynamicAction(const std::string &eventName, 
				const std::string &fileName, const std::string& functionName);

		void addScriptAction(const std::string &scriptName,
				const std::string &fileName, const std::string &lang);

		/**
		 * \brief Call a function, in reaction to a fired event.
		 * \param[in] sig : name of the event.
		 * \param[in] iteration : iteration at which the event is sent.
		 * \param[in] sourceID : source that fired the event.
		 * \param[in,out] mm : Pointer to the MetadataManager.
		 * The action to be called is here characterized by its name.
		 */
		void reactToUserSignal(const std::string &sig, 
				int32_t iteration, int32_t sourceID);

		/**
		 * \brief Call a function, in reaction to a fired event.
		 * \param[in] sigID : identifier of the event.
		 * \param[in] iteration : iteration at which the event is sent.
		 * \param[in] sourceID : source that fired the event.
		 * \param[in,out] mm : pointer to the MetadataManager.
		 * The action to be called is here characterized by its ID.
		 */
		void reactToUserSignal(int sigID, 
				int32_t iteration, int32_t sourceID);

		/**
		 * \brief Gets an action by name.
		 * \param[in] name : name of the action.
		 */ 
		Action* getAction(const std::string &name);

		/**
		 * \brief Gets an action by ID.
		 * \param[in] id : id of the action.
		 */
		Action* getAction(int id);
};

}

#endif
