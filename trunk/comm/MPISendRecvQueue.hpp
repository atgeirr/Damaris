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
 * \file MPISendRecvQueue.hpp
 * \date November 2O12
 * \author Matthieu Dorier
 * \version 0.7
 */
#ifndef __DAMARIS_MPI_SEND_RECV_QUEUE_H
#define __DAMARIS_MPI_SEND_RECV_QUEUE_H

#include <limits>
#include <mpi.h>

#include "xml/Model.hpp"
#include "core/Debug.hpp"
#include "comm/Channel.hpp"

namespace Damaris {
	class MPISendRecvQueue {
		friend class Channel;

		private:
			size_t msgSize;
			MPI_Comm comm;
			int receiver;
			std::list<boost::shared_ptr<MPI_Request> > pendingSendReq;
			std::list<char*> toDeliver;

			
			/**
			 * Constructor. Takes a communicator, the size of each message,
			 * and the rank of the receiver process. If not specified,
			 * the receiver is the creator of the MPISendRecvQueue.
			 */
			MPISendRecvQueue(MPI_Comm c, size_t msgsize, int recvrank = -1)
			: msgSize(msgsize), comm(c), receiver(recvrank)
			{
				if(recvrank == -1) {
					MPI_Comm_rank(comm,&receiver);
				}
			}

			/**
			 * Tries to update the status of pending requests.
			 */
			void update();

		public:
			/**
			 * Sends a message.
			 */
			virtual void Send(void* buffer);

			/**
			 * Try to send a message. Return false without blocking
			 * if the queue is full.
			 */
			virtual bool TrySend(void* buffer);

			/**
			 * Receives a message from the queue. Block if the queue
			 * is empty.
			 */
  			virtual void Receive(void* buffer, size_t buffer_size);
  
			/**
			 * Try getting a message. Return false if the message queue is empty.
			 */
			virtual bool TryReceive(void *buffer, size_t buffer_size);
  			
			/**
			 * Get the maximum number of messages that que queue can hold.
			 */
			virtual size_t MaxMsg() const 
			{
				return std::numeric_limits<size_t>::max();
			}

			/**
			 * Get the size of the messages that the queue holds.
			 */
  			virtual size_t MaxMsgSize() const
			{
				return msgSize;
			}

			/**
			 * Get the current number of messages in the message queue.
			 */
  			virtual size_t NumMsg();
			
			/**
			 * Destructor.
			 */
			virtual ~MPISendRecvQueue() {}
	};
}
#endif
