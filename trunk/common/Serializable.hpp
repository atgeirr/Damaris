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
 * \file Serializable.hpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */

#ifndef __DAMARIS_SERIALIZABLE_H
#define __DAMARIS_SERIALIZABLE_H

#include <stdlib.h>

namespace Damaris {

	/**
	 * The serializable abstract class defines the required function
	 * for a class to be loadable from memory / writtable into memory.
	 */
	class Serializable 
	{
		public:
			/**
			 * Gets the size (in bytes) required to store the 
			 * serialized representation in memory.
			 */
			virtual size_t size() = 0;

			/**
			 * Stores the representation in memory.
			 */
			virtual void toBuffer(void*) = 0;
		
			/**
			 * Retrieve the state of the object from memory.
			 */
			virtual	void fromBuffer(const void*) = 0;
	};
}

#endif
