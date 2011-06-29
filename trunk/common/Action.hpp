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
		// the pointer over the function
		void (*function)(const std::string*,int32_t,int32_t,MetadataManager*);
	public:
		// a default constructor for void actions
		// Action();
		// constructor, takes the pointer over the function to handle
		Action(void(*fptr)(const std::string*, int32_t, int32_t, MetadataManager*));
		// destructor
		~Action();
		
		// a () operator to simplify the calling of the action
		void operator()(const std::string* event, 
				int32_t iteration, int32_t sourceID, MetadataManager* mm);
		// call the inner function
		void call(const std::string* event,
			  int32_t iteration, int32_t sourceID, MetadataManager* mm);
};

}

#endif
