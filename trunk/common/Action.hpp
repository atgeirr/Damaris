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
 * \date July 2011
 * \author Matthieu Dorier
 * \version 0.1
 */
#ifndef __DAMARIS_REACTION_H
#define __DAMARIS_REACTION_H

#include <stdint.h>
#include <string>

#include "common/MetadataManager.hpp"

namespace Damaris {

/**
 * The Action object is an interface to a user-defined action,
 * for the moment it only contains a pointer to a dynamically loaded function
 * but we plan to make it an abstract class and have several child classes
 * to handle scripts in lua, ruby, python or perl also.
 */
class Action {

	private:
		void (*function)(const std::string*,int32_t,int32_t,MetadataManager*); /*!< Pointer to the loaded function */
	public:
		/**
		 * \brief Constructor. Takes the pointer over the function to handle.
		 * \param[in] fptr : Pointer to the function.
		 */
		Action(void(*fptr)(const std::string*, int32_t, int32_t, MetadataManager*));
		
		/**
		 * \brief Destructor.
		 */
		~Action();
		
		/**
		 * \brief Operator overloaded to simplify the call to an action.
		 * \param[in] event : name of the event that induced the action.
		 * \param[in] iteration : iteration at which the action is called.
		 * \param[in] sourceID : ID of the client that fired the action.
		 * \param[in,out] mm : pointer to the MetadataManager that contains all recorded variables.
		 */
		void operator()(const std::string* event, 
				int32_t iteration, int32_t sourceID, MetadataManager* mm);
		
		/**
		 * \brief Another way of calling the inner function.
		 * \see Damaris::Actions::operator()
		 */
		void call(const std::string* event,
			  int32_t iteration, int32_t sourceID, MetadataManager* mm);
};

}

#endif
