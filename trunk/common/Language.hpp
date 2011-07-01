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
 * \file Language.hpp
 * \date July 2011
 * \author Matthieu Dorier
 * \version 0.1
 *
 * Language.hpp contains the enumeration that makes the distinction
 * between C and Fortran. This distinction is important because C and
 * Fortran programs don't have the same array layout.
 */

#ifndef __DAMARIS_LANGUAGE_H
#define __DAMARIS_LANGUAGE_H
/**
 * The language_e enumeration distinguishes between C and Fortran
 * (and a potential unknown language).
 */
enum language_e {
	LG_UNKNOWN,
	LG_FORTRAN,
	LG_C
};

#endif
