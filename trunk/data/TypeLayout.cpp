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
 * \file TypeLayout.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */

#include <cstring>

#include "core/Debug.hpp"
#include "data/Types.hpp"
#include "data/TypeLayout.hpp"

namespace Damaris {

	TypeLayout::TypeLayout(const Model::Type& mdl)
	: Layout((std::string)mdl), Configurable<Model::Type>(mdl)
	{
	}
	
	Model::Type TypeLayout::getType() const
	{
		return model;
	}
	
	unsigned int TypeLayout::getDimensions() const
	{
		return 1;
	}
	
	size_t TypeLayout::getExtentAlongDimension(unsigned int dim) const
	{
		if(dim == 0)
			return 1;
		else
			return 0;
	}

	bool TypeLayout::isUnlimited() const
	{
		return false;
	}

	Layout* TypeLayout::New(const Model::Type& mdl, const std::string &name)
	{
		return new TypeLayout(mdl);
	}

	TypeLayout::~TypeLayout()
	{
	}
}
