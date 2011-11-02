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
 * \file PyInterpreter.cpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */

#include "scripts/PyInterpreter.hpp"
#include "common/Debug.hpp"

namespace Damaris {

bool PyInterpreter::ready = false;
PyObject* PyInterpreter::damaris = NULL;
PyObject* PyInterpreter::metadata = NULL;

void test() {
	std::cout << "hello from python" << std::endl;
}

void PyInterpreter::initialize()
{
	Py_InitializeEx(0);

	PyMethodDef _damarisModuleMethods[] = {{NULL, NULL, 0, NULL}};
	damaris = Py_InitModule("damaris",_damarisModuleMethods);

	PyMethodDef _metadataModuleMethods[] = {{NULL, NULL, 0, NULL}};
	metadata = Py_InitModule("damaris.metadata",_metadataModuleMethods);

	ready = true;
}
		
void PyInterpreter::finalize()
{
	//ready = false;
}

bool PyInterpreter::isReady()
{
	return ready;
}

void PyInterpreter::execFile(const std::string& file) 
{
	boost::python::exec_file(boost::python::str(file));
}

}
