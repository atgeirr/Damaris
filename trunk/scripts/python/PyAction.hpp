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
 * \file PyAction.hpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_PYTHON_ACTION_H
#define __DAMARIS_PYTHON_ACTION_H

#include "core/Action.hpp"

namespace Damaris {

class ActionsManager;

namespace Python {

/**
 * PyAction is a child class of Action used for actions located in
 * a Python script.
 */
class PyAction : public Action {

	friend class Damaris::ActionsManager;

	private:
		std::string fileName; /*!< File name of the script. */

		/**
		 * \brief Condtructor.
		 * This constructor is private and can only be called by the ActionsManager
		 * (friend class), because it does not provide the action's ID and name.
		 * 
		 * \param[in] file : Name of script to load (the file must be
		 * in a directory that Damaris can access from compute nodes).
		 */
		PyAction(std::string file);

		/**
		 * \brief Constructor.
		 * \param[in] name : Name of the action.
		 * \param[in] i : ID of the action in the ActionsSet.
		 * \param[in] file : name of the python file to load.
		 */
		PyAction(std::string name, int i, std::string file);

	public:
		/**
		 * \brief Destructor.
		 */
		~PyAction();
		
		/**
		 * \brief Calls the script.
		 * \see Damaris::Action::operator()
		 */
		void call(int32_t iteration, int32_t sourceID);

		/**
		 * \brief Loads the necessary resources.
		 */
		void load();
};

}
}
#endif
