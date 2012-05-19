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
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */

#include <string.h>
 
#include "core/Debug.hpp"
#include "data/Chunk.hpp"

namespace Damaris {

int Chunk::NbrOfItems() const
{
	int acc = 1;
	for(unsigned int i = 0; i < getDimensions(); i++) {
		acc *= (getEndIndex(i) - getStartIndex(i) + 1);
	}
	return acc;
}

bool Chunk::within(const Layout& enclosing) const
{
    if(enclosing.isUnlimited()) return true;

    bool b = (enclosing.getDimensions() == getDimensions());
    if(b) {
        for(unsigned int i=0; i < getDimensions();i++) {
            b = b && (getStartIndex(i) >= 0);
            b = b && (getEndIndex(i) < (int)enclosing.getExtentAlongDimension(i));
        }
    }
    return b;
}

bool Chunk::within(const Chunk& enclosing) const
{
    bool b = (enclosing.getDimensions() == getDimensions());
    if(b) {
        for(unsigned int i=0; i < getDimensions(); i++) {
            b = b && (getStartIndex(i) >= enclosing.getStartIndex(i));
            b = b && (getEndIndex(i) <= enclosing.getEndIndex(i));
        }
    }
    return b;
}

#ifdef __ENABLE_VISIT
bool Chunk::FillVisItDataHandle(visit_handle hdl)
{
	int nb_items = NbrOfItems();
	const Model::Type& t = getType();
	if(hdl != VISIT_INVALID_HANDLE) {
		if(t == Model::Type::int_ or t == Model::Type::integer) {
			VisIt_VariableData_setDataI(hdl, VISIT_OWNER_SIM, 1, nb_items, (int*)data());
		} else if(t == Model::Type::float_ or t == Model::Type::real) {
			VisIt_VariableData_setDataF(hdl, VISIT_OWNER_SIM, 1, nb_items, (float*)data());
		} else if(t == Model::Type::double_) {
			VisIt_VariableData_setDataD(hdl, VISIT_OWNER_SIM, 1, nb_items, (double*)data());
		} else if(t == Model::Type::char_ or Model::Type::character) {
			VisIt_VariableData_setDataC(hdl, VISIT_OWNER_SIM, 1, nb_items, (char*)data());
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
