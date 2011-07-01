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

#ifndef __DAMARIS_MESSAGE_H
#define __DAMARIS_MESSAGE_H

#include <boost/interprocess/managed_shared_memory.hpp>

using namespace boost::interprocess;

namespace Damaris {

/**
 * The msg_type_e enumeration provides two types of messages
 * to distinguish between notifications of writes and user-defined event.
 */	
	enum msg_type_e {
		MSG_VAR,
		MSG_SIG
	};
/**
 * The Message structure describes a message, used to communicate
 * from simulation's cores to dedicated cores.
 */
	struct Message {
		msg_type_e type;			/*!< The type of message (MSG_VAR, MSG_SIG). */
		int32_t sourceID;			/*!< The id of the source sending the message. */
		int32_t iteration;			/*!< The iteration number in the simulation. */
		char content[64];			/*!< The content (name of the variable or the event). */
		int64_t layoutInfo[8]; 			/*!< Information related to the data layout. */
		managed_shared_memory::handle_t handle; /*!< Pointer to data (handle can be understood by all processes in their own space) */
	};
	
}

#endif
