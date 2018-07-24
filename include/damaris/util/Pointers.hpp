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

#ifndef __DAMARIS_POINTERS_H
#define __DAMARIS_POINTERS_H




/**
 * This file aims at abstracting the std::shared_ptr, weak_ptr and
 * associated mecanisms behind macros, so that we could eventually
 * replace the use of boost shared pointers with other implementations.
 */

/**
 * For using std::shared_ptr and std::weak_ptr it is necessary to include the <memory> file.
 */

#include <memory>


/**
 * \def ENABLE_SHARED_FROM_THIS(T)
 * Represents a class from which it is possible to get a std::shared_ptr
 * to this (so a member function can create a std::shared_ptr to "this" as soon
 * as a std::shared_ptr to this instance already exists outside the class.
 * This macro is supposed to be called in an inheritance declaration:
 * class A : public ENABLE_SHARED_FROM_THIS(A) { ... };
 */

#define ENABLE_SHARED_FROM_THIS(T) \
    std::enable_shared_from_this<T>


/**
 * \def SHARED_FROM_THIS()
 * Can be called within a member function of a class A that inherites
 * from ENABLE_SHARED_FROM_THIS(A); returns a std::shared_ptr to "this".
 * Works only if a std::shared_ptr to this instance already exists outside
 * the class.
 */
#define SHARED_FROM_THIS() \
    shared_from_this()

/**
 * \def SHARED_FROM_BASE_OF_THIS(BASE,CHILD)
 * In the context of a class B that inherites from a class A which
 * itself inherites from ENABLE_SHARED_FROM_THIS(A), calling
 * SHARED_FROM_THIS directly will return something of type std::shared_ptr<A>
 * instead of std::shared_ptr<B>. In order to get a std::shared_ptr<B>, one must
 * call SHARED_FROM_BASE_OF_THIS(A,B).
 */

#define SHARED_FROM_BASE_OF_THIS(BASE,CHILD) \
	std::static_pointer_cast<CHILD>( \
		BASE::shared_from_this())

/**
 * \def UNLESS_POINTER_EXPIRED(ptr,ret)
 * Helper macro that takes a std::shared_ptr, tests if it has expired and
 * executes a block. Example:
 * UNLESS_POINTER_EXPIRED(x) { do something } else { do other things }
 */
#define UNLESS_POINTER_EXPIRED(ptr) \
		if(not ptr.expired())

#endif
