/**************************************************************************
This file is part of Damaris.

Damaris is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Damaris is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Damaris.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

#ifndef __DAMARIS_TYPES_H
#define __DAMARIS_TYPES_H

#include "damaris/model/Model.hpp"

namespace damaris {

	/**
	 * This function gives the size (in bytes) of a type. 
	 * 
	 * \param[in] t : Basic type from which we want the size.
	 */
	size_t TypeSize(const model::Type& t);
	
	/**
	 * This function is used as a compile-time predicate to return a 
	 * Damaris type from a C++ type.
	 */
	template <typename T>
	model::Type TypeFromCpp();
	
}
#endif
