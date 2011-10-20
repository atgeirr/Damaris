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
 * \file SharedMessageQueue.hpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#ifndef __DAMARIS_SH_MSG_QUEUE_H
#define __DAMARIS_SH_MSG_QUEUE_H

#include <boost/interprocess/ipc/message_queue.hpp>
#include "common/xsi_msg_queue.hpp"
#include "common/SharedMemory.hpp"

using namespace boost::interprocess;

namespace Damaris {
	/**
	 * The SharedMessageQueue builds a message queue on top
	 * of a shared memory segment. Access to the queue is handled by
	 * posix mutexes. This class is abstract and has two main implementations
	 * using either posix or xsi shared memory.
	 */
	class SharedMessageQueue {
		private:
			class POSIX_ShMsgQueue;
			class SYSV_ShMsgQueue;

		public:
			/**
			 * Creates a POSIX shared message queue.
			 */
			static SharedMessageQueue* create(posix_shmem_t posix_shmem, 
				const char* name, size_t num_msg, size_t size_msg);
			/**
			 * Creates an XSI shared messahe queue.
			 */
			static SharedMessageQueue* create(sysv_shmem_t sysv_shmem, 
				const char* name, size_t num_msg, size_t size_msg);
	
			/**
			 * Opens an existing POSIX shared message queue.
			 */
			static SharedMessageQueue* open(posix_shmem_t posix_shmem, 
				const char* name);

			/**
			 * Opens an existing XSI shared message queue.
			 */
			static SharedMessageQueue* open(sysv_shmem_t sysv_shmem, 
				const char* name);
	
			/**
			 * Removes a POSIX shared message queue.
			 */
			static bool remove(posix_shmem_t posix_shmem, 
				const char* name);

			/**
			 * Removes an XSI shared message queue.
			 */
			static bool remove(sysv_shmem_t sysv_shmem, 
				const char* name);
	
			/**
			 * Sends a message. Blocks if the queue is full.
			 */
			virtual void send(const void* buffer, size_t size, 
					unsigned int priority) = 0;

			/**
			 * Try to send a message. Return false without blocking
			 * if the queue is full.
			 */
			virtual bool trySend(const void* buffer, size_t size, 
					unsigned int priority) = 0;
			/**
			 * Receives a message from the queue. Block if the queue
			 * is empty.
			 */
  			virtual void receive(void* buffer, size_t buffer_size, 
					size_t &recv_size, unsigned int &priority) = 0;
  
			/**
			 * Try getting a message. Return false if the message queue is empty.
			 */
			virtual bool tryReceive(void *, size_t buffer_size, 
					size_t &recv_size, unsigned int &priority) = 0;
  			
			/**
			 * Get the maximum number of messages that que queue can hold.
			 */
			virtual size_t getMaxMsg() const = 0;

			/**
			 * Get the size of the messages that the queue holds.
			 */
  			virtual size_t getMaxMsgSize() const = 0;

			/**
			 * Get the current number of messages in the message queue.
			 */
  			virtual size_t getNumMsg() = 0;
	};

	class SharedMessageQueue::POSIX_ShMsgQueue : public SharedMessageQueue {
		private:
			message_queue* impl;
		public:
			POSIX_ShMsgQueue(const char* name);
			POSIX_ShMsgQueue(const char* name, size_t num_msg, size_t msg_size);
	
			void send(const void* buffer, size_t size, 
					unsigned int priority);
			bool trySend(const void* buffer, size_t size, 
					unsigned int priority);
			void receive(void* buffer, size_t buffer_size, size_t &recv_size, 
					unsigned int &priority);
			bool tryReceive(void *, size_t buffer_size, size_t &recv_size, 
					unsigned int &priority);
			size_t getMaxMsg() const;
			size_t getMaxMsgSize() const;
			size_t getNumMsg();
	};

	class SharedMessageQueue::SYSV_ShMsgQueue : public SharedMessageQueue {
		private:
			xsi_message_queue* impl;	
		public:
			SYSV_ShMsgQueue(const char* name);
			SYSV_ShMsgQueue(const char* name, size_t num_msg, size_t msg_size);

			void send(const void* buffer, size_t size, 
					unsigned int priority);
			bool trySend(const void* buffer, size_t size, 
					unsigned int priority);
			void receive(void* buffer, size_t buffer_size, size_t &recv_size, 
					unsigned int &priority);
			bool tryReceive(void *, size_t buffer_size, size_t &recv_size, 
					unsigned int &priority);
			size_t getMaxMsg() const;
			size_t getMaxMsgSize() const;
			size_t getNumMsg();
	};
}
#endif
