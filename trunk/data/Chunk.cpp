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
 * \file Chunk.cpp
 * \date Oct. 2012
 * \author Matthieu Dorier
 * \version 0.7
 */
#include "data/Chunk.hpp"

namespace Damaris {

Chunk::Chunk(Buffer* b, ChunkHeader* ch) 
{
	isOwner = false;
	buffer = b;
	header = ch;
	void* addr = ((char*)header)+sizeof(ChunkHeader);
	size_t size = header->GetDataMemoryLength(GetType());
	space = new DataSpace(addr,size);
}

Chunk::Chunk(Buffer* b, handle_t h)
{
	isOwner = false;
	buffer = b;
	header = (ChunkHeader*)buffer->GetAddressFromHandle(h);
	void* addr = ((char*)header)+sizeof(ChunkHeader);
	size_t size = header->GetDataMemoryLength(GetType());
	space = new DataSpace(addr,size);
}

Chunk::~Chunk()
{
	if(isOwner) {
		buffer->Deallocate(header);
	}
}

void* Chunk::Data()
{
	return space->Data();
}

/*
size_t Chunk::MemCopy(const void* src)
{
	size_t size = header->GetDataMemoryLength(GetType());
	memcpy(addr,src,size);
	return size;
}
*/

handle_t Chunk::GetHandle()
{
	return buffer->GetHandleFromAddress(header);
}

int Chunk::NbrOfItems() const
{
	int acc = 1;
	for(unsigned int i = 0; i < GetDimensions(); i++) {
		acc *= (GetEndIndex(i) - GetStartIndex(i) + 1);
	}
	return acc;
}

bool Chunk::Within(const Chunk& enclosing) const
{
    bool b = (enclosing.GetDimensions() == GetDimensions());
    if(b) {
        for(unsigned int i=0; i < GetDimensions(); i++) {
            b = b && (GetStartIndex(i) >= enclosing.GetStartIndex(i));
            b = b && (GetEndIndex(i) <= enclosing.GetEndIndex(i));
        }
    }
    return b;
}

#ifdef __ENABLE_VISIT
bool Chunk::FillVisItDataHandle(visit_handle hdl)
{
	DBG("source is " << GetSource() << ", iteration is " << GetIteration());
	int nb_items = NbrOfItems();
	DBG("Number of items is " << nb_items);
	const Model::Type& t = GetType();
	DBG("Type is " << t);
	if(hdl != VISIT_INVALID_HANDLE) {
		if(t == Model::Type::int_ or t == Model::Type::integer) {
			VisIt_VariableData_setDataI(hdl, VISIT_OWNER_SIM, 1, nb_items, (int*)Data());
		} else if(t == Model::Type::float_ or t == Model::Type::real) {
			VisIt_VariableData_setDataF(hdl, VISIT_OWNER_SIM, 1, nb_items, (float*)Data());
		} else if(t == Model::Type::double_) {
			VisIt_VariableData_setDataD(hdl, VISIT_OWNER_SIM, 1, nb_items, (double*)Data());
		} else if(t == Model::Type::char_ or Model::Type::character) {
			VisIt_VariableData_setDataC(hdl, VISIT_OWNER_SIM, 1, nb_items, (char*)Data());
		} else {
			ERROR("VisIt cannot accept chunk data of type \"" << t << "\"");
			return false;
		}
		return true;
	}
	return false;
}
#endif

} // namespace Damaris
