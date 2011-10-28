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
 * \file PythonAction.cpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#include <exception>

#include "common/Debug.hpp"
#include "scripts/PythonInterpreter.hpp"
#include "scripts/PythonAction.hpp"

namespace Damaris {

	PythonAction::PythonAction(std::string file)
	: Action()
	{
		fileName	= file;
		loaded		= true;
	}
	
	PythonAction::PythonAction(std::string n, int i, std::string file)
	: Action(n,i)
	{
		fileName 	= file;
		loaded		= true;
	}

	PythonAction::~PythonAction()
	{
	}
	
	void PythonAction::call(int32_t iteration, int32_t sourceID, MetadataManager* mm)
	{
		if(!PythonInterpreter::isReady())
			load();
		try {
			PythonInterpreter::execFile(fileName);
		} catch(std::exception &e) {
			ERROR("in Python action \"" << name << "\"");
		}
	}

	void PythonAction::load()
	{
		PythonInterpreter::initialize();
	}
}
