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
 * \file Parameter.hpp
 * \date July 2011
 * \author Matthieu Dorier
 * \version 0.1
 *
 * Parameter.hpp defines the parameter structure: a structure that can hold
 * a pointer to most basic types.
 */
#ifndef __DAMARIS_PARAMETER_H
#define __DAMARIS_PARAMETER_H

#include "common/Types.hpp"
#include <string>

namespace Damaris {
/**
 * The Parameter union defines a value read from the configuration file
 * We could have used a void* pointer instead of a union but I find this
 * way more elegant.
 */
	struct Parameter {
		Types::basic_type_e type; /*!< Type of the parameter. */
		union {
			short* short_ptr; 	/*!< Pointer to a short value. */
			int* int_ptr;		/*!< Pointer to an int value. */
			long* long_ptr; 	/*!< Pointer to a long value. */
			float* float_ptr; 	/*!< Pointer to a float value. */
			double* double_ptr;	/*!< Pointer to a double value. */
			char* char_ptr; 	/*!< Pointer to a char value. */
			std::string* str_ptr;	/*!< Pointer to a string value. */
		} value; /*!< Pointer to the parameter's value */
	};
}

#endif
