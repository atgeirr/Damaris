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
 * \file DynamicAction.hpp
 * \date July 2011
 * \author Matthieu Dorier
 * \version 0.1
 */
#ifndef __DAMARIS_DYNAMIC_ACTION_H
#define __DAMARIS_DYNAMIC_ACTION_H

#include "common/Action.hpp"

namespace Damaris {

class ActionsManager;

/**
 * The Action object is an interface to a user-defined action,
 * for the moment it only contains a pointer to a dynamically loaded function
 * but we plan to make it an abstract class and have several child classes
 * to handle scripts in lua, ruby, python or perl also.
 */
class DynamicAction : public Action {

	friend class ActionsManager;

	private:
		void (*function)(std::string,int32_t,int32_t,MetadataManager*); /*!< Pointer to the loaded function */
		std::string funname;
		std::string filename;
		void* handle;

		DynamicAction(std::string fun, std::string file);
	public:
		/**
		 * \brief Constructor. Takes the pointer over the function to handle.
		 * \param[in] fptr : Pointer to the function.
		 */
		DynamicAction(std::string name, int i, std::string fun, std::string file);
		/**
		 * \brief Destructor.
		 */
		~DynamicAction();
		
		/**
		 * \brief Another way of calling the inner function.
		 * \see Damaris::Actions::operator()
		 */
		void call(int32_t iteration, int32_t sourceID, MetadataManager* mm);
		
		void load();
};

}

#endif
