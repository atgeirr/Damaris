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
 * \file ParameterSet.hpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#ifndef __DAMARIS_PARAMETER_SET_H
#define __DAMARIS_PARAMETER_SET_H

#include <map>
#include "xml/Model.hpp"
#include "common/Debug.hpp"
#include "common/Configurable.hpp"
#include "common/Parameter.hpp"

namespace Damaris {

class ParameterSet : public Configurable<ParameterSet,Model::DataModel> {
	private:
		std::map<std::string,Parameter> paramSet; /*!< Enclosed map. */
		
		void init();
	public:
		/**
		 * Constructor.
		 */
		ParameterSet(Model::DataModel* mdl);
	
		/**
		 * Sets a parameter. Does not modify an existing parameter.
		 * \return true in case of success, false if the parameter already exists.
		 */
		template<typename T>
		bool set(std::string &name, T& value);
		
		/**
		 * Gets a pointer to an existing parameter, return NULL if
		 * the parameter does not exist.
		 */
		template<typename T>
		T get(std::string &name);

		/**
		 * Access parameters by name, only int parameters are considered.
		 */
		int operator[](std::string& n);
};

template<typename T>
T ParameterSet::get(std::string &name)
{
	std::map<std::string,Parameter>::iterator it = paramSet.find(name);
	for(; it != paramSet.end(); it++)
	{
		if(it->second.value.type() == typeid(T)) {
			return (boost::any_cast<T>(it->second.value));
		}
	}
	WARN("Parameter \""<< name <<"\" not defined, returned value may be inconsistant");
	return T();
}


template<typename T>
bool ParameterSet::set(std::string &name, T& value)
{
	if(paramSet.find(name) != paramSet.end())
		return false;

	paramSet.insert(std::pair<std::string,Parameter>(name,Parameter(name,value)));
	return true;
}

}
#endif
