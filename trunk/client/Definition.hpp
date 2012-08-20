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
 * \file Definition.hpp
 * \date August 2012
 * \author Matthieu Dorier
 * \version 0.6
 *
 * This file contains the API to define objects that are not available
 * in the configuration file.
 */
#ifndef __DAMARIS_DEFINITION_H
#define __DAMARIS_DEFINITION_H

#include "event/BoundAction.hpp"

extern "C" {

/**
 * This function associates an event to a function's pointer. It can
 * only be used in coupled mode. This function has to be called by
 * all the processes (clients and server) before they start.
 *
 * \param[in] event : name of the event.
 * \param[out] f : function to bind, must have the prototype of a amaris::BoundAction::fun_t.
 * \return 0 in case of success, -1 if the event already exists (from the configuration file),
 */
int DC_bind_function(const char* event, Damaris::BoundAction::fun_t f);

}

#endif
