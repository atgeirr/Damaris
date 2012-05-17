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
 * \file Action.cpp
 * \date February 2012 
 * \author Matthieu Dorier
 * \version 0.4
 */
#include "common/Action.hpp"

namespace Damaris {

	Action::Action()
	{
		loaded = false;
	}

	Action::Action(std::string n, int i)
	{
		loaded = false;
		name = n;
		id = i;
	}
	
	Action::~Action()
	{}
	
	void Action::operator()(int32_t iteration, int32_t sourceID)
	{
		call(iteration,sourceID);
	}
}
