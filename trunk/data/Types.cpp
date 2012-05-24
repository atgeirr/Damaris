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
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#include <string>

#include "data/Types.hpp"

namespace Damaris {

namespace Types {

	int basicTypeSize(const Model::Type &t)
	{
		switch(t) {
			case Model::Type::short_ :
				return sizeof(short int);
				break;
			case Model::Type::int_ :
				return sizeof(int);
				break;
			case Model::Type::integer :
				return sizeof(int);
				break;
			case Model::Type::long_ :
				return sizeof(long int);
				break;
			case Model::Type::float_ :
				return sizeof(float);
				break;
			case Model::Type::real :
				return sizeof(float);
				break;
			case Model::Type::double_ :
				return sizeof(double);
				break;
			case Model::Type::char_ :
				return sizeof(char);
				break;
			case Model::Type::character :
				return sizeof(char);
				break;
			default:
				return -1;
		}
		return -1;
	}

}
}
