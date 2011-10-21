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
 * \file MetadataManager.cpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#include "common/Debug.hpp"
#include "common/MetadataManager.hpp"

namespace Damaris {

MetadataManager::MetadataManager()
{
}	

bool MetadataManager::addVariable(std::string varname, std::string layoutname)
{
	VariableSet::index<by_name>::type::iterator it = 
		variables.get<by_name>().find(varname);
	
	if(it != variables.get<by_name>().end()) {
		WARN("Inserting a variable with a name identical to a previously defined variable");
		return false;
	}

	// check that the layout exists
	Layout* l = getLayout(layoutname);
	if(l == NULL) {
		ERROR("Undefined layout \"" << layoutname << "\" for variable \""
			<< varname << "\"");
		return false;
	}

	// allocate the variable
	int id = variables.size();
	Variable* v = new Variable(id,varname,l);

	variables.insert(boost::shared_ptr<Variable>(v));
	return true;
}

Variable* MetadataManager::getVariable(std::string &name)
{
	VariableSet::index<by_name>::type::iterator it = 
		variables.get<by_name>().find(name);
	if(it == variables.get<by_name>().end()) {
		return NULL;
	}
	return it->get();
}

Variable* MetadataManager::getVariable(int id)
{
	VariableSet::index<by_id>::type::iterator it =
		variables.get<by_id>().find(id);
	if(it == variables.get<by_id>().end()) {
		return NULL;
	}
	return it->get();
}

bool MetadataManager::addLayout(std::string& lname, Layout &l)
{
	if(layouts.find(lname) != layouts.end())
	{
		ERROR("Trying to define two layouts with the same name \"" << lname << "\"");
		return false;
	}
	layouts.insert(std::pair<std::string,Layout>(lname,l));
	return true;
}

Layout* MetadataManager::getLayout(std::string& lname)
{
	std::map<std::string,Layout>::iterator it = layouts.find(lname);
	if(it == layouts.end()) return NULL;
	return &(it->second);
}

MetadataManager::~MetadataManager()
{
}

}
