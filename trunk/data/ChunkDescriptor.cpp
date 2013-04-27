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
 * \file ChunkHeader.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#include <string.h>
#include "data/ChunkDescriptor.hpp"

namespace Damaris {

	ChunkDescriptor* ChunkDescriptor::New(unsigned int d, 
			const int* lb, const int* ub)
	{
		return new ChunkDescriptor(d,lb,ub);
	}

	ChunkDescriptor* ChunkDescriptor::New(const Layout& l)
	{
		return new ChunkDescriptor(l);
	}

	void ChunkDescriptor::Delete(ChunkDescriptor* cd)
	{
		if(cd != NULL) {
			delete cd;
		}
	}

	ChunkDescriptor::ChunkDescriptor(unsigned int d, 
			const int* lb, const int* ub)
	{
		dimensions = d;
		memset(lbounds,0,d*sizeof(int));
		memcpy(lbounds,lb,d*sizeof(int));
		memset(ubounds,0,d*sizeof(int));
		memcpy(ubounds,ub,d*sizeof(int));
	}

	ChunkDescriptor::ChunkDescriptor(const Layout& l)
	{
		dimensions = l.GetDimensions();
		for(int i = 0; i < (int)dimensions; i++ ) {
			lbounds[i] = 0;
			ubounds[i] = l.GetExtentAlongDimension(i)-1;
		}
	}

	ChunkDescriptor::ChunkDescriptor(const ChunkDescriptor& ch)
	{
		dimensions = ch.dimensions;
		memset(lbounds,0,dimensions*sizeof(int));
		memcpy(lbounds,ch.lbounds,dimensions*sizeof(int));
		memset(ubounds,0,dimensions*sizeof(int));
		memcpy(ubounds,ch.ubounds,dimensions*sizeof(int));
	}

	size_t ChunkDescriptor::GetDataMemoryLength(const Model::Type &type) const
	{
		size_t result = 1;
		result *= Types::basicTypeSize(type);
		for(unsigned int i = 0; i < dimensions; i++) {
			size_t d = (ubounds[i]-lbounds[i]+1);
			result *= d;
		}
		return result;
	}

	int ChunkDescriptor::GetStartIndex(int i) const 
	{
		if(0 <= i && i < (int)dimensions)
			return lbounds[i];
		else
			return 0;
	}

	int ChunkDescriptor::GetEndIndex(int i) const
	{
		if(0 <= i && i < (int)dimensions)
			return ubounds[i];
		else
			return 0;
	}

	void ChunkDescriptor::Move(const Position* p)
	{
		for(unsigned int i = 0; i<dimensions; i++) {
			int l = lbounds[i];
			int dl = p->GetOffset(i) - l;
			lbounds[i] += dl;
			ubounds[i] += dl;
		}
	}

	bool ChunkDescriptor::Within(const Layout& enclosing) const
	{
		if(enclosing.IsUnlimited()) return true;

		bool b = (enclosing.GetDimensions() == GetDimensions());
		if(b) {
			for(unsigned int i=0; i < GetDimensions();i++) {
				b = b && (GetStartIndex(i) >= 0);
				b = b && (GetEndIndex(i) < (int)enclosing.GetExtentAlongDimension(i));
			}
		}
		return b;
	}

} // namespace Damaris

