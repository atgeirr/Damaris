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
 * \file Types.hpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_TYPES_H
#define __DAMARIS_TYPES_H

#include <string>
#include "xml/Model.hpp"

namespace Damaris {

/**
 * \namespace Damaris::Types
 * Everything related to data types.
 */
namespace Types {

	/**
	 * This function gives the size (in bytes) of each type. 
	 * \param[in] t : Basic type from which you want the size.
	 * \return The size (in bytes) of the data type.
	 */
	int basicTypeSize(const Model::Type& t);	
}
}
#endif
