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

#ifndef __DAMARIS_TYPES_H
#define __DAMARIS_TYPES_H
/** 
 * This file defines basic types used by variables and basic type sizes
 * TODO: allow user-defined datatypes
 */
namespace Damaris {
namespace Types {

	enum basic_type_e {
		SHORT  = 1,     // short int, integer*2
		INT    = 2,     // int, integer*4
		LONG   = 3,     // long int, integer*8
		FLOAT  = 4,     // float, real
		DOUBLE = 5,     // double, real*8
		CHAR   = 6,     // char, character
		STR    = 7,	// string

		UNDEFINED_TYPE = 0  // don't know
	}; // enum basic_type_e

	/* this function gives the size (in bytes) of each type */
	int basicTypeSize(basic_type_e t);
	/* this function returns a basic type from a string */
	basic_type_e getTypeFromString(std::string* str);
}
}
#endif
