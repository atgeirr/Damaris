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
 * \date February 2012 
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_ACTIONS_MANAGER_H
#define __DAMARIS_ACTIONS_MANAGER_H

#include <string>

#include "xml/Model.hpp"
#include "common/Action.hpp"
#include "common/ActionSet.hpp"
#include "common/Configurable.hpp"
#include "common/Environment.hpp"

namespace Damaris {

/**
 * The ActionsManager keeps the list of defined actions
 * It is in charge of loading functions from shared libraries,
 * from scripts, etc. and call there functions upon reception
 * of events.
 */
class ActionsManager : public Configurable<ActionsManager,Model::Actions> {

	private:
		ActionSet actions; /*!< Set of actions indexed by ID and name. */
		Environment* environment;

	public:
		/**
		 * \brief Constructor.
		 * Takes a model and an environment as parameters.
		 */
		ActionsManager(Model::Actions* mdl, Environment* env);

	private:
		/**
		 * \brief This function is called by the constructor to initialize everything.
		 */
		void init();

		/** 
		 * \brief Adds an action defined through a dynamic library.
		 * \param[in] eventName : Name of the event taken as a key for this action.
		 * \param[in] fileName : Name of the dynamic library (.so, .dylib...)
		 * \param[in] functionName : Name of the function to load in this dynamic library.
		 * \param[in] scope : Scope of the event (core, node, global).
	 	 * 
		 * The action is defined but the dynamic library is not loaded. It will be loaded
		 * by the action the first time the action is called. This way the clients can
		 * have an instance of ActionsManager that just acts as a set of actions from which
		 * they can retrieve action's IDs.
		 */
		void addDynamicAction(const std::string &eventName, 
				const std::string &fileName, const std::string& functionName,
				const Model::Scope &scope);

		/** 
		 * \brief Adds an action defined through a script.
		 * \param[in] eventName : Name of the event taken as a key for this action.
		 * \param[in] fileName : Name of the dynamic library (.so, .dylib...)
		 * \param[in] functionName : Name of the function to load in this dynamic library.
		 * \param[in] scope : Scope of the event (core, node, global).
		 * 
		 * The interpreter for the language in which the script is writen is loaded only
		 * if the action is eventually called.
		 */
		void addScriptAction(const std::string &scriptName,
				const std::string &fileName, const Model::Language &lang,
				const Model::Scope &scope);

		/**
		 * \brief Call a function, in reaction to a fired event.
		 * \param[in] sig : name of the event.
		 * \param[in] iteration : iteration at which the event is sent.
		 * \param[in] sourceID : source that fired the event.
		 * The action to be called is here characterized by its name.
		 */
		public: void reactToUserSignal(const std::string &sig, 
				int32_t iteration, int32_t sourceID);

		/**
		 * \brief Call a function, in reaction to a fired event.
		 * \param[in] sigID : identifier of the event.
		 * \param[in] iteration : iteration at which the event is sent.
		 * \param[in] sourceID : source that fired the event.
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
