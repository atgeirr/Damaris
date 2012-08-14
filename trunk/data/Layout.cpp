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
 * \file Layout.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */

#include <cstring>

#include "core/Debug.hpp"
#include "data/Layout.hpp"

namespace Damaris {

	Layout::Layout(const std::string &n)
	: name(n)
	{
	}
	
	const std::string& Layout::getName() const
	{
		return name;
	}

	int Layout::getID() const
	{
		return id;
	}
	
	Layout::~Layout()
	{
	}
}
