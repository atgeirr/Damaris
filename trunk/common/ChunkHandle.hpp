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
 * \file ChunkHandle.hpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#ifndef __DAMARIS_CHUNKHANDLE_H
#define __DAMARIS_CHUNKHANDLE_H

#include "common/Chunk.hpp"

namespace Damaris {

/**
 * A ChunkHandle is a particular child class of Chunk used to
 * only describe the data without holding any pointer to actual data.
 * It is created/deleted through the client-side API and passed by
 * the user as a hint regarding data being written.
 */
class ChunkHandle : public Chunk {
		
	public:
		/**
		 * \brief Constructor. Simply call the parent-class constructor.
		 */
		ChunkHandle(Types::basic_type_e t, unsigned int d, std::vector<int> &si, std::vector<int> &ei)
		: Chunk(t,d,si,ei) {}
	
		/**
		 * \brief Returns NULL, as no data is hold by this type of chunk.
		 */
		void* data() { return NULL; }
		
		/**
		 * \brief Returns false, as no data is to be deleted.
		 */
		virtual bool remove() { return false; }

}; // class ChunkHandle

} // namespace Damaris

#endif
