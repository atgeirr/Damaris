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
 * \file Communication.hpp
 * \date April 2012
 * \author Matthieu Dorier
 * \version 0.5
 */

#ifndef __DAMARIS_COMMUNICATION_H
#define __DAMARIS_COMMUNICATION_H

namespace Damaris {

/**
 * This class represents an asynchronous communication layer.
 * Such a layer should be used for distributed algorithms but
 * not to transmit large amounts of data (use directly MPI for that).
 * The MSG template parameter should be a small type (int, double, etc.)
 * providing an implementation of the operators == and <.
 * Additionally it should be copy-contructible.
 */
template<typename MSG>
class Communication {
	public:
		/**
		 * Updates the communication layer by checking
		 * at most n requests (default n = 1).
		 */
		virtual void Update(unsigned int n=1) = 0;

		/**
		 * Sends a message (non-blocking). The message will be eventually
		 * delevered by the process identifyed by its ID.
		 * This function should use FIFO channels: two messages A and B
		 * sent to the same process should be received in the same order.
		 */
		virtual void Send(int receiver, MSG m) = 0;

		/**
		 * Delivers the next message in the queue. If there is no message,
		 * returns false.
		 */
		virtual bool Deliver(MSG* m) = 0;

		/**
		 * Broadcast a message to all processes in the communication layer.
		 * The message will be eventually delivered by all processes.
		 * This primitive must implement a uniform broadcast: if a process
		 * delivers message A then message B (sent from different sources),
		 * then all processes should deliver A then B.
		 */
		virtual void Bcast(MSG m) = 0;

		/**
		 * This primitive ensures that the passed message will be delivered
		 * to all processes only when all processes have sent it. This is
		 * a way of implementing a non-blocking barrier.
		 */
		virtual void Sync(MSG m) = 0;
};

}

#endif
