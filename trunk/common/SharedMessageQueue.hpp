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
 * \date September 2011
 * \author Matthieu Dorier
 * \version 0.3
 *
 */
#ifndef __DAMARIS_SH_MSG_QUEUE_H
#define __DAMARIS_SH_MSG_QUEUE_H

#include <boost/interprocess/ipc/message_queue.hpp>
#include "common/xsi_msg_queue.hpp"
#include "common/SharedMemory.hpp"
using namespace boost::interprocess;

namespace Damaris {

class SharedMessageQueue {
	private:
		class POSIX_ShMsgQueue;
		class SYSV_ShMsgQueue;

	public:
		static SharedMessageQueue* create(posix_shmem_t posix_shmem, const char* name, size_t num_msg, size_t size_msg);
		static SharedMessageQueue* create(sysv_shmem_t sysv_shmem, const char* name, size_t num_msg, size_t size_msg);

		static SharedMessageQueue* open(posix_shmem_t posix_shmem, const char* name);
		static SharedMessageQueue* open(sysv_shmem_t sysv_shmem, const char* name);

		static bool remove(posix_shmem_t posix_shmem, const char* name);
		static bool remove(sysv_shmem_t sysv_shmem, const char* name);

		virtual void send(const void* buffer, size_t size, unsigned int priority) = 0;
		virtual bool trySend(const void* buffer, size_t size, unsigned int priority) = 0;
  		virtual void receive(void* buffer, size_t buffer_size, size_t &recv_size, unsigned int &priority) = 0;
  		virtual bool tryReceive(void *, size_t buffer_size, size_t &recv_size, unsigned int &priority) = 0;
  		
		virtual size_t getMaxMsg() const = 0;
  		virtual size_t getMaxMsgSize() const = 0;
  		virtual size_t getNumMsg() = 0;
};

class SharedMessageQueue::POSIX_ShMsgQueue : public SharedMessageQueue {
	private:
		message_queue* impl;
	public:
		POSIX_ShMsgQueue(const char* name);
		POSIX_ShMsgQueue(const char* name, size_t num_msg, size_t msg_size);

		void send(const void* buffer, size_t size, unsigned int priority);
		bool trySend(const void* buffer, size_t size, unsigned int priority);
		void receive(void* buffer, size_t buffer_size, size_t &recv_size, unsigned int &priority);
		bool tryReceive(void *, size_t buffer_size, size_t &recv_size, unsigned int &priority);
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

		void send(const void* buffer, size_t size, unsigned int priority);
		bool trySend(const void* buffer, size_t size, unsigned int priority);
		void receive(void* buffer, size_t buffer_size, size_t &recv_size, unsigned int &priority);
		bool tryReceive(void *, size_t buffer_size, size_t &recv_size, unsigned int &priority);
		size_t getMaxMsg() const;
		size_t getMaxMsgSize() const;
		size_t getNumMsg();
};
}
#endif
