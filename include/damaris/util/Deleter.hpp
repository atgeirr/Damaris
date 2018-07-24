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

#ifndef __DAMARIS_DELETER_H
#define __DAMARIS_DELETER_H

//#include "damaris/util/TypeWrapper.hpp"

namespace damaris 
{

/**
 * The Deleter class is used to provide a way of deleting std::shared_ptr of
 * objects with a private or protected destructor. Such a class A should declare
 * the class Deleter<A> as friend.
 */
template<typename T>
class Deleter
{
//	friend class TypeWrapper<T>::type;
	
	public:
		
	/**
	 * Private constructor, only the class for which the Deleter is defined
	 * can create a Deleter for itself.
	 */
	Deleter() {}
			
	public:
		
	/**
	 * Equivalent of the delete operation on the pointer.
	 *
	 * \param[in] p : pointer to delete.
	 */
	void operator()(T* p) { delete p; }
};

}

#endif
