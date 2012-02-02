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

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/xsi_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include "xml/Model.hpp"
#include "common/Debug.hpp"
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
			struct shm_queue_hdr {

				interprocess_mutex main_lock;
				interprocess_condition cond_recv;
				interprocess_condition cond_send;
				int maxMsg;
				int sizeMsg;
				int head;
				int tail;

				shm_queue_hdr(int numItems, int sizeItems) : 
					main_lock(), cond_recv(), cond_send(),
					maxMsg(numItems), sizeMsg(sizeItems), head(0), tail(0)
				{
				};

				int current_num_msg()
				{
					if(tail >= head)
						return (tail - head);
					else
						return (tail + maxMsg - head);
				}

			} *shmq_hdr;

			char* data;
			mapped_region *region;

			SharedMessageQueue(mapped_region *mem);
		
		public:
			static SharedMessageQueue* create(Model::QueueModel* mdl);
			/**
			 * Creates a POSIX shared message queue.
			 */
			static SharedMessageQueue* create(posix_shmem_t posix_shmem, 
				const std::string& name, size_t num_msg, size_t size_msg);
			/**
			 * Creates an XSI shared messahe queue.
			 */
			static SharedMessageQueue* create(sysv_shmem_t sysv_shmem, 
				const std::string& name, size_t num_msg, size_t size_msg);
	
			static SharedMessageQueue* open(Model::QueueModel* mdl);
			/**
			 * Opens an existing POSIX shared message queue.
			 */
			static SharedMessageQueue* open(posix_shmem_t posix_shmem, 
				const std::string& name);

			/**
			 * Opens an existing XSI shared message queue.
			 */
			static SharedMessageQueue* open(sysv_shmem_t sysv_shmem, 
				const std::string& name);
	
			static bool remove(Model::QueueModel* mdl);
			/**
			 * Removes a POSIX shared message queue.
			 */
			static bool remove(posix_shmem_t posix_shmem, const std::string& name);

			/**
			 * Removes an XSI shared message queue.
			 */
			static bool remove(sysv_shmem_t sysv_shmem, const std::string& name);
	
			/**
			 * Sends a message. Blocks if the queue is full.
			 */
			virtual void send(const void* buffer);

			/**
			 * Try to send a message. Return false without blocking
			 * if the queue is full.
			 */
			virtual bool trySend(const void* buffer);
			/**
			 * Receives a message from the queue. Block if the queue
			 * is empty.
			 */
  			virtual void receive(void* buffer, size_t buffer_size);
  
			/**
			 * Try getting a message. Return false if the message queue is empty.
			 */
			virtual bool tryReceive(void *buffer, size_t buffer_size);
  			
			/**
			 * Get the maximum number of messages that que queue can hold.
			 */
			virtual size_t getMaxMsg() const;

			/**
			 * Get the size of the messages that the queue holds.
			 */
  			virtual size_t getMaxMsgSize() const;

			/**
			 * Get the current number of messages in the message queue.
			 */
  			virtual size_t getNumMsg();

			~SharedMessageQueue();
	};
}
#endif
