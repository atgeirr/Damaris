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

#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

/**
 * This file aims at abstracting the shared_ptr, weak_ptr and
 * associated mecanisms behind macros, so that we could eventually
 * replace the use of boost shared pointers with other implementations.
 */

/**
 * \def USING_POINTERS
 * To be called in a file to use the necessary namespaces for shared_ptr
 * and weak_ptr to be properly found.
 */
#define USING_POINTERS \
	using boost::shared_ptr; \
	using boost::weak_ptr

/**
 * \def ENABLE_SHARED_FROM_THIS(T)
 * Represents a class from which it is possible to get a shared_ptr
 * to this (so a member function can create a shared_ptr to "this" as soon
 * as a shared_ptr to this instance already exists outside the class.
 * This macro is supposed to be called in an inheritance declaration:
 * class A : public ENABLE_SHARED_FROM_THIS(A) { ... };
 */	
#define ENABLE_SHARED_FROM_THIS(T) \
	boost::enable_shared_from_this<T>

/**
 * \def SHARED_FROM_THIS()
 * Can be called within a member function of a class A that inherites
 * from ENABLE_SHARED_FROM_THIS(A); returns a shared_ptr to "this".
 * Works only if a shared_ptr to this instance already exists outside
 * the class.
 */
#define SHARED_FROM_THIS() \
	shared_from_this()

/**
 * \def SHARED_FROM_BASE_OF_THIS(BASE,CHILD)
 * In the context of a class B that inherites from a class A which
 * itself inherites from ENABLE_SHARED_FROM_THIS(A), calling
 * SHARED_FROM_THIS directly will return something of type shared_ptr<A>
 * instead of shared_ptr<B>. In order to get a shared_ptr<B>, one must
 * call SHARED_FROM_BASE_OF_THIS(A,B).
 */
#define SHARED_FROM_BASE_OF_THIS(BASE,CHILD) \
	boost::static_pointer_cast<CHILD>( \
		BASE::shared_from_this())

/**
 * \def UNLESS_POINTER_EXPIRED(ptr,ret)
 * Helper macro that takes a shared_ptr, tests if it has expired and
 * executes a block. Example:
 * UNLESS_POINTER_EXPIRED(x) { do something } else { do other things }
 */
#define UNLESS_POINTER_EXPIRED(ptr) \
		if(not ptr.expired())

#endif
