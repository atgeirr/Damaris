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
 * \file PyAction.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#include <exception>

#include "core/Debug.hpp"
#include "scripts/python/PyInterpreter.hpp"
#include "scripts/python/PyAction.hpp"

namespace Damaris {

namespace Python {

PyAction::PyAction(const Model::Script& mdl, const std::string &name)
: ScriptAction(mdl,name)
{
}
	
void PyAction::call(int32_t iteration, int32_t sourceID, const char* args)
{
	try {
		PyInterpreter* p = Python::PyInterpreter::getInstance();
		if(p != NULL) {
			p->execFile((std::string)model.file(),sourceID,iteration);
		} else { 
			throw(std::runtime_error("Unable to get a pointer to a Python interpreter."));
		}
	} catch(std::exception &e) {
		ERROR("in Python action \"" << getName() << "\": "<< e.what());
	}
}

PyAction* PyAction::New(const Model::Script& mdl, const std::string& name)
{
	return new PyAction(mdl,name);
}

}
}
