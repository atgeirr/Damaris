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
 *
 * ParameterSet.hpp defines the holder of a set of parameters.
 */
#ifndef __DAMARIS_PARAMETER_SET_H
#define __DAMARIS_PARAMETER_SET_H

#include <map>
#include "common/Parameter.hpp"

namespace Damaris {

class ParameterSet {
	private:
		std::map<std::string,Parameter> paramSet;
	
	public:
		ParameterSet();
	
		template<typename T>
		void set(std::string &name, T& value);
		
		template<typename T>
		T* get(std::string &name);

		int operator[](std::string& n);
};

}
#endif

namespace Damaris {

template<typename T>
T* ParameterSet::get(std::string &name)
{
	T* result = NULL;
	std::map<std::string,Parameter>::iterator it = paramSet.find(name);
	for(; it != paramSet.end(); it++)
	{
		if(it->second.value.type() == typeid(T)) {
			*result = boost::any_cast<T>(it->second.value);
			break;
		}
	}
	return result;
}


template<typename T>
void ParameterSet::set(std::string &name, T& value)
{
	paramSet.insert(std::pair<std::string,Parameter>(name,Parameter(name,boost::any(value))));
}

}

