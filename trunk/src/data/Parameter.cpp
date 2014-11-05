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

#include "data/Parameter.hpp"

namespace damaris {

int Parameter::ToBuffer(void* b, size_t maxsize) const
{
        size_t s1 = TypeSize(GetModel().type());
        size_t s = std::min(s1,maxsize);
	memcpy(b,value_,s);
	return DAMARIS_OK;
}

int Parameter::FromBuffer(const void* b, size_t maxsize) 
{
	size_t s1 = TypeSize(GetModel().type());
        size_t s = std::min(s1,maxsize);
	if(value_ == NULL) {
		value_ = malloc(s);
	}
	memcpy(value_,b,s);
	Changed();
	return DAMARIS_OK;
}

Parameter::~Parameter()
{
	if(value_ != NULL) {
		free(value_);
	}
}

}
