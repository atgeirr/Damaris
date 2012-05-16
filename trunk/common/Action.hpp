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
 * \file Action.hpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_ACTION_H
#define __DAMARIS_ACTION_H

#include <stdint.h>
#include <string>

#include "common/VariableManager.hpp"

namespace Damaris {

class Action;
class ActionsManager;

/**
 * The Action object is an interface to a user-defined action.
 * It is an abstract class, child classes must overwrite the Action::call 
 * and Action::load functions.
 */
class Action {
	friend class ActionsManager;

	protected:
		bool loaded; 	  /*!< Indicates wether associated ressources 
				    are loaded and ready to call the action. */
		std::string name; /*!< Name of the action. */
		int id; 	  /*!< ID given to the action when set in
				    the ActionsManager. */

		/**
		 * \brief Constructor.
		 * Builds an anonymous action without name of ID. 
		 * Accessible only to the ActionsManager class which can set the name and ID itself.
		 */
		Action();
	public:
		/**
		 * \brief Constructor. 
		 * \param[in] n : Name of the action.
		 * \param[in] i : ID of the action.
		 */
		Action(std::string n,int i);
		
		/**
		 * \brief Destructor.
		 */
		~Action();

		/**
		 * \brief Gets the ID of the action.
		 */		
		int getID() const { return id; }

		/**
		 * \brief Gets the name of the action.
		 */
		std::string getName() const { return name; }	

		/**
		 * \brief Operator overloaded to simplify the call to an action.
		 * This operator simply call the virtual "call" function.
		 * \param[in] iteration : iteration at which the action is called.
		 * \param[in] sourceID : ID of the client that fired the action.
		 * \param[in,out] mm : pointer to the MetadataManager that contains 
		 *                     all recorded variables.
		 */
		void operator()(int32_t iteration, int32_t sourceID);
		
		/**
		 * \brief Call the action. To be overloaded by child classes.
		 * \param[in] iteration : iteration at which the action is called.
		 * \param[in] sourceID : ID of the client that fired the action.
		 * \param[in,out] mm : pointer to the MetadataManager that contains 
		 *                     all recorded variables.
		 * \see Damaris::Actions::operator()
		 */
		virtual void call(int32_t iteration, int32_t sourceID) = 0;

		/**
		 * \brief Loads required resources for the action to be called. 
		 * This function is virtual and has to be overloaded by child classes depending on
		 * there needs. Only the server will load the action the first time the action 
		 * is called (lazy loading).
		 */
		virtual void load() = 0;

		
		class EmptyAction; 
};

/**
 * The EmptyAction class inherites from the Action class and
 * defines an action that does nothing. It is loaded when an error occured
 * and the system is unable to create a regular Action (e.g. the configuration
 * defines a Python action but Python has not been enabled in Damaris at compile time).
 */
class Action::EmptyAction : public Action {

	void call(int32_t iteration, int32_t sourceID) { }

	void load() {}
};
}

#endif
