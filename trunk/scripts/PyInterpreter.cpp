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
#include <boost/python.hpp>
#include <string>

#include "common/Debug.hpp"
#include "common/MetadataManager.hpp"
#include "scripts/PyVariable.hpp"
#include "scripts/PyInterpreter.hpp"

namespace bp = boost::python;

namespace Damaris {
namespace Python {

static bool ready = false;
static bp::object dict;

static void open(const std::string& varname)
{
	MetadataManager* metadata = MetadataManager::getInstance();
	Variable* v = metadata->getVariable(varname);
	if(v == NULL) {
		ERROR("In damaris.open(): undefined variable \"" << varname << "\"");
	} else {
		INFO("Variable \"" << varname << "\" found");
	}
}

BOOST_PYTHON_MODULE(damaris)
{
	bp::def("open",&open);
	bp::class_<PyVariable>("Variable")
		.def("chunks",&PyVariable::chunks);
}

void initialize()
{
	try {
		PyImport_AppendInittab((char*)"damaris",&initdamaris);
		Py_InitializeEx(0);
		bp::object main = bp::import("__main__");
		dict = main.attr("__dict__");

	} catch(boost::python::error_already_set) {
		PyErr_Print();
	}
	ready = true;
}
		
void finalize()
{
	ready = false;
	Py_Finalize();
}

void execFile(const std::string& file) 
{
	if(!ready)
		initialize();
	if(ready) {
		try {
			bp::exec_file(bp::str(file),dict,dict);
		} catch(...) {
			ERROR("While executing file \"" << file << "\"");
			PyErr_Print();
		}
	} else {
		ERROR("Unable to initialize Python before calling \"" << file << "\"");
	}
}

}
}
