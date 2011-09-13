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
 * \file SharedMessageQueue.cpp
 * \date September 2011
 * \author Matthieu Dorier
 * \version 0.3
 *
 */
#include <boost/interprocess/xsi_shared_memory.hpp>
#include "common/SharedMessageQueue.hpp"

namespace Damaris {

SharedMessageQueue* SharedMessageQueue::create(posix_shmem_t posix_shmem, const char* name, size_t num_msg, size_t size_msg)
{
	return new POSIX_ShMsgQueue(name,num_msg,size_msg);
}

SharedMessageQueue* SharedMessageQueue::create(sysv_shmem_t sysv_shmem, const char* name, size_t num_msg, size_t size_msg)
{
	return new SYSV_ShMsgQueue(name,num_msg,size_msg);
}

SharedMessageQueue* SharedMessageQueue::open(posix_shmem_t posix_shmem, const char* name)
{
	return new POSIX_ShMsgQueue(name);
}

SharedMessageQueue* SharedMessageQueue::open(sysv_shmem_t sysv_shmem, const char* name)
{
	return new SYSV_ShMsgQueue(name);
}

bool SharedMessageQueue::remove(posix_shmem_t posix_shmem, const char* name)
{
	return shared_memory_object::remove(name);
}

bool SharedMessageQueue::remove(sysv_shmem_t sysv_shmem, const char* name)
{
	xsi_key key(name,1);
	return xsi_message_queue::remove(key);
}

SharedMessageQueue::POSIX_ShMsgQueue::POSIX_ShMsgQueue(const char* name, size_t num_msg, size_t size_msg)
{
	impl = new message_queue(create_only,name,num_msg,size_msg);
}

SharedMessageQueue::POSIX_ShMsgQueue::POSIX_ShMsgQueue(const char* name)
{
	impl = new message_queue(open_only,name);
}

void SharedMessageQueue::POSIX_ShMsgQueue::send(const void* buffer, size_t size, unsigned int priority)
{
	impl->send(buffer,size,priority);
}

bool SharedMessageQueue::POSIX_ShMsgQueue::trySend(const void* buffer, size_t size, unsigned int priority)
{
	return impl->try_send(buffer,size,priority);
}

void SharedMessageQueue::POSIX_ShMsgQueue::receive(void* buffer, size_t buffer_size, size_t &recv_size, unsigned int &priority)
{
	impl->receive(buffer,buffer_size,recv_size,priority);
}

bool SharedMessageQueue::POSIX_ShMsgQueue::tryReceive(void* buffer, size_t buffer_size, size_t &recv_size, unsigned int &priority)
{
	return impl->try_receive(buffer,buffer_size,recv_size,priority);
}

size_t SharedMessageQueue::POSIX_ShMsgQueue::getMaxMsg() const
{
	return impl->get_max_msg();
}

size_t SharedMessageQueue::POSIX_ShMsgQueue::getMaxMsgSize() const
{
	return impl->get_max_msg_size();
}

size_t SharedMessageQueue::POSIX_ShMsgQueue::getNumMsg()
{
	return impl->get_num_msg();
}


SharedMessageQueue::SYSV_ShMsgQueue::SYSV_ShMsgQueue(const char* name, size_t num_msg, size_t size_msg)
{
	xsi_key key(name,1);
	impl = new xsi_message_queue(create_only,key,num_msg,size_msg);
}

SharedMessageQueue::SYSV_ShMsgQueue::SYSV_ShMsgQueue(const char* name)
{
	xsi_key key(name,1);
	impl = new xsi_message_queue(open_only,key);
}

void SharedMessageQueue::SYSV_ShMsgQueue::send(const void* buffer, size_t size, unsigned int priority)
{
	impl->send(buffer,size,priority);
}

bool SharedMessageQueue::SYSV_ShMsgQueue::trySend(const void* buffer, size_t size, unsigned int priority)
{
	return impl->try_send(buffer,size,priority);
}

void SharedMessageQueue::SYSV_ShMsgQueue::receive(void* buffer, size_t buffer_size, size_t &recv_size, unsigned int &priority)
{
	impl->receive(buffer,buffer_size,recv_size,priority);
}

bool SharedMessageQueue::SYSV_ShMsgQueue::tryReceive(void *buffer, size_t buffer_size, size_t &recv_size, unsigned int &priority)
{
	return impl->try_receive(buffer,buffer_size,recv_size,priority);
}

size_t SharedMessageQueue::SYSV_ShMsgQueue::getMaxMsg() const
{
	return impl->get_max_msg();
}

size_t SharedMessageQueue::SYSV_ShMsgQueue::getMaxMsgSize() const
{
	return impl->get_max_msg_size();
}

size_t SharedMessageQueue::SYSV_ShMsgQueue::getNumMsg()
{
	return impl->get_num_msg();
}

}
