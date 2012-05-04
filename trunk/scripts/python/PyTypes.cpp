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
 * \file PyTypes.cpp
 * \date February 2012 
 * \author Matthieu Dorier
 * \version 0.4
 */
#include "scripts/python/PyTypes.hpp"

namespace Damaris {
namespace Python {

namespace PyTypes {

int getPyTypeFromDamarisType(const Model::TypeModel &t)
{

	switch(t) {
		case Model::TypeModel::short_:
			return NPY_SHORT;
		case Model::TypeModel::int_:
			return NPY_INT;
		case Model::TypeModel::integer:
			return NPY_INT;
		case Model::TypeModel::long_:
			return NPY_LONGLONG;
		case Model::TypeModel::float_:
			return NPY_FLOAT;
		case Model::TypeModel::real :
			return NPY_FLOAT;
		case Model::TypeModel::double_:
			return NPY_DOUBLE;
		case Model::TypeModel::char_:
			return NPY_UINT8;
		case Model::TypeModel::character:
			return NPY_UINT8;
		case Model::TypeModel::string:
			return NPY_STRING;
		default:
			return -1;
	}
	return -1;
}

}
}
}
