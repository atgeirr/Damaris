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
 
#include "common/Debug.hpp"
#include "common/Chunk.hpp"

namespace Damaris {

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

} // namespace Damaris
