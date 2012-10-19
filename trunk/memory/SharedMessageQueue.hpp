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
 * \date February 2O12
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_SH_MSG_QUEUE_H
#define __DAMARIS_SH_MSG_QUEUE_H

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/xsi_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include "xml/Model.hpp"
#include "core/Debug.hpp"
#include "memory/Channel.hpp"
#include "memory/SharedMemory.hpp"

using namespace boost::interprocess;

namespace Damaris {
	/**
	 * The SharedMessageQueue builds a message queue on top
	 * of a shared memory segment. Access to the queue is handled by
	 * posix mutexes.
	 */
	class SharedMessageQueue : public Channel {
		private:
			/**
			 * The SharedMessageQueue::shm_queue_hdr is a header
			 * to be stored in shared memory for the processes to access it.
			 */
			struct shm_queue_hdr {

				interprocess_mutex main_lock; /*!< To fully lock the queue. */
				interprocess_condition cond_recv; /*!< Condition for blocked receivers to wait. */
				interprocess_condition cond_send; /*!< Condition for blocked senders to wait. */
				int maxMsg; /*!< Maximum number of messages in the queue. */
				int sizeMsg; /*!< Size of each message. */
				int head; /*!< Index of the head. */
				int tail; /*!< Index of the tail. */
				int numMsg; /*!< Number of messages. */

				/**
				 * Constructor.
				 * \param[in] numItems : size of the queue in number of messages.
				 * \param[in] sizeItems : size of each message.
				 */
				shm_queue_hdr(int numItems, int sizeItems) : 
					main_lock(), cond_recv(), cond_send(),
					maxMsg(numItems), sizeMsg(sizeItems), head(0), tail(0),
					numMsg(0)
				{
				};

			} *shmq_hdr; /*!< Pointer to the header of the queue in shared memory. */
			char* data; /*!< Pointer to the data region in shared memory. */
			mapped_region *region; /*!< Pointer to the mapped region (so we can remove it). */

			/**
			 * Constructor for SharedMessageQueue. This constructor is private,
			 * use create and open functions to retrieve an instance.
			 * \param[in] mem : an implementation of a mapped_region on which to store the queue.
			 */
			SharedMessageQueue(mapped_region *mem);
		
		public:
			/**
			 * Creates a SharedMessageQueue based on a model.
			 * \param[in] mdl : base model from which to create the SharedMessageQueue.
			 */
			static SharedMessageQueue* Create(Model::Queue* mdl);


			/**
			 * Helper function to create a POSIX shared message queue.
			 */
			static SharedMessageQueue* Create(posix_shmem_t posix_shmem, 
				const std::string& name, size_t num_msg, size_t size_msg);

			/**
			 * Helper function to creates an XSI shared messahe queue.
			 */
			static SharedMessageQueue* Create(sysv_shmem_t sysv_shmem, 
				const std::string& name, size_t num_msg, size_t size_msg);

			/**
			 * Opens a SharedMessageQueue based on a model.
			 * \param[in] mdl : base model from which to create the SharedMessageQueue.
			 */
			static SharedMessageQueue* Open(Model::Queue* mdl);
	
			/**
			 * Opens an existing POSIX shared message queue.
			 */
			static SharedMessageQueue* Open(posix_shmem_t posix_shmem, 
				const std::string& name, size_t num_msg = 0, size_t size_msg = 0);

			/**
			 * Opens an existing XSI shared message queue.
			 */
			static SharedMessageQueue* Open(sysv_shmem_t sysv_shmem, 
				const std::string& name, size_t num_msg = 0, size_t size_msg = 0);
	
			/**
			 * Removes a SharedMessageQueue described by a model.
			 */
			static bool Remove(Model::Queue* mdl);
	
			/**
			 * Removes a POSIX shared message queue.
			 */
			static bool Remove(posix_shmem_t posix_shmem, const std::string& name);

			/**
			 * Removes an XSI shared message queue.
			 */
			static bool Remove(sysv_shmem_t sysv_shmem, const std::string& name);
	
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
			
			/**
			 * Destructor. Will not remove the shared structures.
			 */
			virtual ~SharedMessageQueue();
	};
}
#endif
