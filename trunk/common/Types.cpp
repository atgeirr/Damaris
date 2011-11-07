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
 * \file Types.cpp
 * \date July 2011
 * \author Matthieu Dorier
 * \version 0.1
 */
#include <string>

#include "common/Types.hpp"

namespace Damaris {

namespace Types {

	int basicTypeSize(basic_type_e t)
	{
		switch(t) {
			case SHORT :
				return sizeof(short int);
			case INT :
				return sizeof(int);
			case LONG :
				return sizeof(long int);
			case FLOAT :
				return sizeof(float);
			case DOUBLE :
				return sizeof(double);
			case CHAR :
				return sizeof(char);
			default:
				return 0;
		}
		return 0;
	}

	static std::string names[] =
		{"undefined","short","int","long","float","double","char","string"};

	basic_type_e getTypeFromString(const std::string* str)
	{
		if(str->compare("short") == 0)  return SHORT;
		if(str->compare("int") == 0)    return INT;
		if(str->compare("long") == 0)   return LONG;
		if(str->compare("float") == 0)  return FLOAT;
		if(str->compare("double") == 0) return DOUBLE;
		if(str->compare("char") == 0)   return CHAR;
		if(str->compare("string") == 0) return STR;
		return UNDEFINED_TYPE;
	}

	const std::string& getStringFromType(basic_type_e t)
	{	
		if(t <= 7)
			return names[(int)t];
		else
			return names[0];
	}
}
}
