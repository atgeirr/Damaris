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
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#ifndef __DAMARIS_DYNAMIC_ACTION_H
#define __DAMARIS_DYNAMIC_ACTION_H

#include "common/Action.hpp"

namespace Damaris {

class ActionsManager;

/**
 * DynamicAction is a child class of Action used for actions located in
 * dynamic libraries (.so on Linux, .dylib on MacOSX). The dynamic library
 * is loaded only the first time the action is called, thus avoiding
 * unnecessary memory usage.
 */
class DynamicAction : public Action {

	friend class ActionsManager;

	private:
		void (*function)(std::string,int32_t,int32_t); /*!< Pointer to the loaded function */
		std::string funName; /*!< Name of the function in the dynamic library. */
		std::string fileName; /*!< Name of the dynamic library. */
		void* handle; /*!< Handle for the opened dynamic library. */

		/**
		 * \brief Condtructor.
		 * This constructor is private and can only be called by the ActionsManager
		 * (friend class), because it does not provide the action's ID and name.
		 * 
		 * \param[in] fun : Name of the function to load.
		 * \param[in] file : Name of the dynamic library to load (the file must be
		 * in a directory set in the LD_LIBRARY_PATH environment variable).
		 */
		DynamicAction(std::string fun, std::string file);

		/**
		 * \brief Constructor.
		 * \param[in] name : Name of the action.
		 * \param[in] i : ID of the action in the ActionsSet.
		 * \param[in] fun : name of the function to load.
		 * \param[in] file : name of the file to load.
		 */
		DynamicAction(std::string name, int i, std::string fun, std::string file);

	public:
		/**
		 * \brief Destructor.
		 */
		~DynamicAction();
		
		/**
		 * \brief Another way of calling the inner function.
		 * \see Damaris::Action::operator()
		 */
		void call(int32_t iteration, int32_t sourceID);

		/**
		 * \brief Loads the shared library.
		 */		
		void load();
};

}

#endif
