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
 * \file BoundAction.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#include "core/Debug.hpp"
#include "event/NodeAction.hpp"
#include "event/BoundAction.hpp"

namespace Damaris {

	BoundAction::BoundAction(fun_t f, const std::string &name)
	: Action(name)
	{
		function	= f;
	}

	BoundAction::~BoundAction()
	{
	}

	void BoundAction::call(int32_t iteration, int32_t sourceID, const char* args)
	{
		if(function != NULL)
			(*function)(getName(),iteration,sourceID,args);
	}

	BoundAction* BoundAction::New(fun_t f, const std::string& name)
	{
		if(f == NULL) return NULL;
		return new BoundAction(f,name);
	}
}
