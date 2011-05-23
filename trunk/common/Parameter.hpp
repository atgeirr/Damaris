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

#ifndef __DAMARIS_PARAMETER_H
#define __DAMARIS_PARAMETER_H

#include <string>

namespace Damaris {
	
	enum param_type_e {
		PARAM_INT,
		PARAM_LONG,
		PARAM_FLOAT,
		PARAM_DOUBLE,
		PARAM_CHAR,
		PARAM_STR
	};
/**
 * The Parameter union defines a value read from the configuration file
 * We could have used a void* pointer instead of a union but I find this
 * way more elegant.
 */
	struct Parameter {
		param_type_e type;
		union {
			int* int_ptr;
			long* long_ptr;
			float* float_ptr;
			double* double_ptr;
			char* char_ptr;
			std::string* str_ptr;
		} value;
	};
}

#endif
