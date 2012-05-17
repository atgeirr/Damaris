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
 */
template<typename MSG>
class Communication {
	public:
		virtual void update() = 0;
		/**
		 * Sends a message (non-blocking). The message will be eventually
		 * delevered by the process identifyed by its ID.
		 */
		virtual void send(int receiver, const MSG *m) = 0;

		/**
		 * Delivers the next message in the queue. If there is no message,
		 * returns false.
		 */
		virtual bool deliver(MSG* m) = 0;

		/**
		 * Broadcast a message to all processes in the communication layer.
		 * The message will be eventually delivered by all processes.
		 */
		virtual void bcast(const MSG* m) = 0;
};

}

#endif
