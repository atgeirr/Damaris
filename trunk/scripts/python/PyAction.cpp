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
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#include <exception>

#include "common/Debug.hpp"
#include "scripts/python/PyInterpreter.hpp"
#include "scripts/python/PyAction.hpp"

namespace Damaris {

namespace Python {

PyAction::PyAction(std::string file)
: Action()
{
	fileName	= file;
	loaded		= true;
}
	
PyAction::PyAction(std::string n, int i, std::string file)
: Action(n,i)
{
	fileName 	= file;
	loaded		= true;
}

PyAction::~PyAction()
{
}
	
void PyAction::call(int32_t iteration, int32_t sourceID)
{
	try {
		PyInterpreter* p = Python::PyInterpreter::getInstance();
		if(p != NULL) {
			p->execFile(fileName,sourceID,iteration);
		} else { 
			throw(std::runtime_error("Unable to get a pointer to a Python interpreter."));
		}
	} catch(std::exception &e) {
		ERROR("in Python action \"" << name << "\": "<< e.what());
	}
}

void PyAction::load()
{
}

}
}
