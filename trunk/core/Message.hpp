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
 * \file Message.hpp
 * \date February 2012 
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_MESSAGE_H
#define __DAMARIS_MESSAGE_H

#include "core/SharedMemory.hpp"

namespace Damaris {

/**
 * The msg_type_e enumeration provides several types of messages
 * to distinguish between notifications of writes, user-defined events;
 * internal events.
 */	
	enum msg_type_e {
		MSG_VAR, /*!< Notifies that a variable has been written. */
		MSG_SIG, /*!< Notifies of an incoming event. */
		MSG_INT  /*!< Internal events. */
	};

/*
 * The internal events naming.
 */
	enum int_msg_e {
		CLIENT_CONNECTED,
		KILL_SERVER,
		URGENT_CLEAN,
		LOST_DATA
	};

/**
 * The Message structure describes a message, used to communicate
 * from simulation's cores to dedicated cores.
 */
	struct Message {
		msg_type_e 	type;		/*!< The type of message (MSG_VAR, MSG_SIG, MSG_INT). */
		int32_t 	source;		/*!< The id of the source sending the message. */
		int32_t 	iteration;	/*!< The iteration number in the simulation. */
		int32_t		object;		/*!< An identifier to an object defined in 
						     the configuration, or an argument for
						     internal events. */
		handle_t 	handle;		/*!< Pointer to data relatively to the 
						     beginning of the shared memory segment */
	};
}

#endif
