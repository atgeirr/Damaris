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
 * \file Channel.hpp
 * \date October 2O12
 * \author Matthieu Dorier
 * \version 0.7
 */
#ifndef __DAMARIS_CHANNEL_H
#define __DAMARIS_CHANNEL_H

#include "xml/Model.hpp"
#include "core/Debug.hpp"
#include "memory/SharedMemory.hpp"

using namespace boost::interprocess;

namespace Damaris {
	class Channel {
		public:
			/**
			 * Creates a Channel based on a model.
			 * \param[in] mdl : base model from which to create the Channel.
			 */
			static Channel* Create(Model::Queue* mdl);


			/**
			 * Opens a Channel based on a model.
			 * \param[in] mdl : base model from which to create the Channel.
			 */
			static Channel* Open(Model::Queue* mdl);
	
			/**
			 * Removes a Channel described by a model.
			 */
			static bool Remove(Model::Queue* mdl);
	
			/**
			 * Sends a message. Blocks if the queue is full.
			 */
			virtual void Send(void* buffer) = 0;

			/**
			 * Try to send a message. Return false without blocking
			 * if the queue is full.
			 */
			virtual bool TrySend(void* buffer) = 0;

			/**
			 * Receives a message from the queue. Block if the queue
			 * is empty.
			 */
  			virtual void Receive(void* buffer, size_t buffer_size) = 0;
  
			/**
			 * Try getting a message. Return false if the message queue is empty.
			 */
			virtual bool TryReceive(void *buffer, size_t buffer_size) = 0;
  			
			/**
			 * Get the maximum number of messages that que queue can hold.
			 */
			virtual size_t MaxMsg() const = 0;

			/**
			 * Get the size of the messages that the queue holds.
			 */
  			virtual size_t MaxMsgSize() const = 0;

			/**
			 * Get the current number of messages in the message queue.
			 */
  			virtual size_t NumMsg() = 0;
			
			/**
			 * Destructor. Will not remove the shared structures.
			 */
			virtual ~Channel() {}
	};
}
#endif
