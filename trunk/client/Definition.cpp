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
 * \file Definition.cpp
 * \date August 2012
 * \author Matthieu Dorier
 * \version 0.6
 */

#include "core/ActionManager.hpp"
#include "event/BoundAction.hpp"
#include "client/Definition.hpp"

extern "C" {

using namespace Damaris;

	int DC_bind_function(const char* event, BoundAction::fun_t f) {
		BoundAction* a = BoundAction::New(f,std::string(event));
		if(a == NULL) return -1;
		if(not ActionManager::Add<BoundAction>(a)) {
			delete a;
			return -1;
		}
		return 0;
	}

}
