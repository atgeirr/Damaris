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

#include <VisItDataInterface_V2.h>
#include "core/Manager.hpp"

namespace Damaris {

/**
 * The Action object is an interface to a user-defined action.
 * It is an abstract class, child classes must overwrite the Action::call 
 * and Action::load functions.
 */
class Action  {
	friend class Manager<Action>;

		std::string name; /*!< Name of the action. */
		int id; 	  /*!< ID given to the action when set in
				    the ActionsManager. */

	protected:
		/**
		 * \brief Constructor. 
		 */
		Action(const std::string& n) : name(n) {}
		
	public:
		/**
		 * \brief Gets the ID of the action.
		 */		
		int getID() const { return id; }

		/**
		 * \brief Gets the name of the action.
		 */
		const std::string& getName() const { return name; }	

		/**
		 * \brief Operator overloaded to simplify the call to an action.
		 * This operator simply call the virtual "call" function.
		 * \param[in] iteration : iteration at which the action is called.
		 * \param[in] sourceID : ID of the client that fired the action.
		 * \param[in,out] mm : pointer to the MetadataManager that contains 
		 *                     all recorded variables.
		 */
		void operator()(int32_t iteration, int32_t sourceID, const char *args = NULL);
		
		/**
		 * \brief Call the action. To be overloaded by child classes.
		 * \param[in] iteration : iteration at which the action is called.
		 * \param[in] sourceID : ID of the client that fired the action.
		 * \param[in,out] mm : pointer to the MetadataManager that contains 
		 *                     all recorded variables.
		 * \see Damaris::Actions::operator()
		 */
		virtual void call(int32_t iteration, int32_t sourceID, const char *args = NULL) = 0;

		virtual bool IsExternallyVisible() = 0;

#ifdef __ENABLE_VISIT
		bool exposeVisItMetaData(visit_handle md);
#endif

		class EmptyAction; 
};

/**
 * The EmptyAction class inherites from the Action class and
 * defines an action that does nothing. It is loaded when an error occured
 * and the system is unable to create a regular Action (e.g. the configuration
 * defines a Python action but Python has not been enabled in Damaris at compile time).
 */
class Action::EmptyAction : public Action {

	void call(int32_t iteration, int32_t sourceID, const char* args = NULL) { }

};
}

#endif
