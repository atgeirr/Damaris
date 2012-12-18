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

#include "event/ScriptAction.hpp"

namespace Damaris {

namespace Python {

/**
 * PyAction is a child class of Action used for actions located in
 * a Python script.
 */
class PyAction : public ScriptAction {

	protected:
		/**
		 * \brief Condtructor.
		 * 
		 */
		PyAction(const Model::Script& mdl, const std::string& name);

	public:	
		/**
		 * \brief Calls the script.
		 * \see Damaris::Action::operator()
		 */
		void Call(int32_t iteration, int32_t sourceID, const char* args = NULL);

		static PyAction* New(const Model::Script& mdl, const std::string& name);
};

}
}
#endif
