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
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#ifndef __DAMARIS_TYPES_H
#define __DAMARIS_TYPES_H

#include <string>

namespace Damaris {

/**
 * \namespace Damaris::Types
 * Everything related to data types.
 */
namespace Types {

	/**
	 * The basic_type_e enumeration defines all basic types.
	 */
	enum basic_type_e {
		SHORT  = 1,     /*!< short int, integer*2 */
		INT    = 2,     /*!< int, integer*4 */
		LONG   = 3,     /*!< long int, integer*8 */
		FLOAT  = 4,     /*!< float, real */
		DOUBLE = 5,     /*!< double, real*8 */
		CHAR   = 6,     /*!< char, character */
		STR    = 7,	/*!< string */

		UNDEFINED_TYPE = 0  /*!< don't know the type  (bad...) */
	}; // enum basic_type_e

	/**
	 * This function gives the size (in bytes) of each type. 
	 * \param[in] t : Basic type from which you want the size.
	 * \return The size (in bytes) of the data type.
	 */
	int basicTypeSize(basic_type_e t);
	
	/**
	 * This function returns a basic type from a string.
	 * \param[in] str : string holding the name of a type.
	 * \return The associated basic type or UNDEFINED_TYPE if it does
	 *         not correspond to any known type.
	 */
	basic_type_e getTypeFromString(const std::string* str);
}
}
#endif
