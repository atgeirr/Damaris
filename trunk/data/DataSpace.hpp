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
 * \file DataSpace.hpp
 * \date May 2013
 * \author Matthieu Dorier
 * \version 0.8
 */
#ifndef __DAMARIS_DATASPACE_H
#define __DAMARIS_DATASPACE_H

#include <stdlib.h>

namespace Damaris {
	/**
	 */
	class DataSpace {
		private:
			void* address;
			size_t size;

		public:	
			/**
			 * Constructor. Builds a dataspace from an existing
			 * pointer and size.
			 */
			DataSpace(void* addr, size_t s)
			: address(addr), size(s) {}

			/**
			 * Destructor (does nothing since it's an abstract class)
			 */
			virtual ~DataSpace() {}

			/**
			 * Returns a pointer over the actual data 
			 * (to be overloaded in child classes).
			 */
			virtual void* Data() {
				return address;
			}
                        
                        virtual size_t Size() {
				return size;
			}

			/**
			 * Copy data from a pointer to the DataSpace.
			 * Returns the size of the copied data.
			 */
			virtual size_t MemCopy(const void* addr) {
				if(address != NULL && size != 0) {
					memcpy(address,addr,size);
					return size;
				} else {
					return 0;
				}
			}
	}; // class DataSpace 

} // namespace Damaris

#endif
