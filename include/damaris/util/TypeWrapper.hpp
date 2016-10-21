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

#ifndef __DAMARIS_TYPE_WRAPPER_H
#define __DAMARIS_TYPE_WRAPPER_H

namespace damaris {

/**
 * The TypeWrapper structure exists because it is forbidden to write
 * friend class T; within a class template. Instead, one can write
 * friend class TypeWrapper<T>::type; which has the same effect.
 */
template<typename T>
struct TypeWrapper {
	typedef T type; /*!< Enclosed type. */
};

}

#endif
