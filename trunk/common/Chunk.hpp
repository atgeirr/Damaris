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
 * \file Chunk.hpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 *
 * This file defines the Chunk object.
 */
#ifndef __DAMARIS_CHUNK_H
#define __DAMARIS_CHUNK_H

#include <stdlib.h>
#include <vector>
#include "common/Types.hpp"
#include "common/Layout.hpp"
#include "common/Serializable.hpp"

namespace Damaris {

/**

 */
class Chunk : public Serializable {
		
//		Chunk(ChunkHeader* h,int src,int it, void* ptr);

//		ChunkHeader* header;

	private:
		Types::basic_type_e type;
		unsigned int dimensions;

		std::vector<int> startIndices;
		std::vector<int> endIndices;

	protected:
		int source;
		int iteration;

		Chunk();
		Chunk(Types::basic_type_e t, unsigned int d, std::vector<int> &si, std::vector<int> &ei);

	public:
		~Chunk();
		
		int getSource();
		void setSource(int s);
		
		int getIteration();
		void setIteration(int i);

		unsigned int getDimensions();

		Types::basic_type_e getType();

		int getStartIndex(int i);
		int getEndIndex(int i);

		size_t getRequiredMemoryLength();

		bool within(Layout* enclosing);
		bool within(Chunk* enclosing);

		void toBuffer(void*);
		void fromBuffer(const void*);

		size_t size();

		virtual void* data() = 0;
		virtual bool remove() = 0;

}; // class Chunk
	
} // namespace Damaris

#endif

