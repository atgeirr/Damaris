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
 * \file Language.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#include "core/Language.hpp"

namespace Damaris {

namespace Language {

language_e getLanguageFromString(const std::string* s)
{
	if(*s == "Fortran" || *s == "fortran" || *s == "FORTRAN" || *s == "F90" || *s == "F77"
		|| *s == "f90" || *s == "f77") return LG_FORTRAN;
	else 
	if(*s == "Python" || *s == "python" || *s == "PYTHON") return LG_PYTHON;
	else if(*s == "?") return LG_UNKNOWN;
	else return LG_C;
}

}
}
