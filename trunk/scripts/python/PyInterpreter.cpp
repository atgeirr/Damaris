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

#define PY_ARRAY_UNIQUE_SYMBOL damaris_ARRAY_API
#include <numpy/arrayobject.h>

#include <string>

#include "common/Debug.hpp"
#include "common/MetadataManager.hpp"
#include "scripts/python/PyChunk.hpp"
#include "scripts/python/PyLayout.hpp"
#include "scripts/python/PyVariable.hpp"
#include "scripts/python/PyInterpreter.hpp"

namespace bp = boost::python;

namespace Damaris {
namespace Python {

static bool ready = false;
static bp::object dict;
static bp::object damaris_dict;

static bp::object open(const std::string& varname)
{
	MetadataManager* metadata = MetadataManager::getInstance();
	Variable* v = metadata->getVariable(varname);
	if(v == NULL) {
		return bp::object();
	} else {
		return bp::object(PyVariable(v));
	}
}

static bp::object clear()
{
	MetadataManager* metadata = MetadataManager::getInstance();
	VariableSet& varset = metadata->getVariableSet();
	VariableSet::index<by_id>::type::iterator it, end;
	it = varset.get<by_id>().begin();
	end = varset.get<by_id>().end();
	while(it != end) {
		it->get()->clear();
		it++;
	}
	return bp::object();
}

BOOST_PYTHON_MODULE(damaris)
{
	bp::def("open",&open);
	bp::def("clear",&clear);
	bp::class_<PyLayout>("Layout")
		.add_property("name",bp::make_function(&PyLayout::name,
			bp::return_value_policy<bp::copy_const_reference>()))
		.add_property("type",bp::make_function(&PyLayout::type,
			bp::return_value_policy<bp::copy_const_reference>()))
		.add_property("extents",&PyLayout::extents)
		;
	bp::class_<PyChunk>("Chunk")
		.add_property("source",&PyChunk::source)
		.add_property("iteration",&PyChunk::iteration)
		.add_property("type",bp::make_function(&PyChunk::type,
			bp::return_value_policy<bp::copy_const_reference>()))
		.add_property("lower_bounds",&PyChunk::lower_bounds)
		.add_property("upper_bounds",&PyChunk::upper_bounds)
		.add_property("data",&PyChunk::data)
		;
	bp::class_<PyVariable>("Variable")
		.def("select",&PyVariable::select)
		.def("remove",&PyVariable::remove)
		.def("clear",&PyVariable::clear)
		.add_property("chunks",&PyVariable::chunks)
		.add_property("layout",&PyVariable::layout)
		.add_property("name",&PyVariable::name)
		.add_property("fullname",bp::make_function(&PyVariable::fullname,
			bp::return_value_policy<bp::copy_const_reference>()))
		;
}

PyInterpreter::PyInterpreter()
{
	initialize();
}

PyInterpreter::~PyInterpreter()
{
	finalize();
}

void PyInterpreter::initialize()
{
	try {
		PyImport_AppendInittab((char*)"damaris",&initdamaris);
		Py_InitializeEx(0);
		bp::object main = bp::import("__main__");
		dict = main.attr("__dict__");
		bp::object damaris =  bp::import("damaris");
		damaris_dict = damaris.attr("__dict__");
		import_array();
	} catch(boost::python::error_already_set) {
		PyErr_Print();
	}
	ready = true;
}
		
void PyInterpreter::finalize()
{
	ready = false;
	Py_Finalize();
}

void PyInterpreter::execFile(const std::string& file, int source, int iteration) 
{
	if(!ready)
		initialize();
	if(ready) {
		try {
			damaris_dict["source"] = source;
			damaris_dict["iteration"] = iteration;
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
