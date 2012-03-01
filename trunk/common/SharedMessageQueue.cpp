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
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/xsi_shared_memory.hpp>
#include "common/Debug.hpp"
#include "common/Message.hpp"
#include "common/SharedMessageQueue.hpp"

namespace Damaris {

SharedMessageQueue::SharedMessageQueue(mapped_region* mem)
{
	region = mem;
	shmq_hdr = (struct shm_queue_hdr*)(mem->get_address());
	data = ((char*)shmq_hdr) + sizeof(struct shm_queue_hdr);
}

SharedMessageQueue::~SharedMessageQueue()
{
	delete region;
}

SharedMessageQueue* SharedMessageQueue::create(Model::QueueModel* mdl)
{
	std::string& name = mdl->name();
	size_t num_msg = mdl->size();
	size_t size_msg = sizeof(Message);
	std::string& type = mdl->type();

	if(type == "posix") return create(posix_shmem_t(),name,num_msg,size_msg);
	else if(type == "sysv") return create(sysv_shmem_t(),name,num_msg,size_msg);
	else {
		ERROR("Unknown shared memory type \"" << type << "\"");
		return NULL;
	}
}

SharedMessageQueue* SharedMessageQueue::create(posix_shmem_t posix_shmem, 
		const std::string& name, size_t num_msg, size_t size_msg)
{
	shared_memory_object base(create_only,name.c_str(),read_write);
	size_t size = num_msg*size_msg + sizeof(struct shm_queue_hdr);
	base.truncate(size);

	mapped_region *region = new mapped_region(base,read_write);
	void* addr = region->get_address();
	new (addr) shm_queue_hdr(num_msg,size_msg);
	
	return new SharedMessageQueue(region);
}

SharedMessageQueue* SharedMessageQueue::create(sysv_shmem_t sysv_shmem, 
		const std::string& name, size_t num_msg, size_t size_msg)
{
	size_t size = num_msg*size_msg + sizeof(struct shm_queue_hdr);
	xsi_shared_memory base(create_only,xsi_key(name.c_str(),1),size);
	
	mapped_region *region = new mapped_region(base,read_write);
	void* addr = region->get_address();
	new (addr) shm_queue_hdr(num_msg,size_msg);

	return new SharedMessageQueue(region);
}

SharedMessageQueue* SharedMessageQueue::open(Model::QueueModel* mdl)
{
	std::string& name = mdl->name();
	std::string& type = mdl->type();

	if(type == "posix") return open(posix_shmem_t(),name);
	else if(type == "sysv") return open(sysv_shmem_t(),name);
	else {
		ERROR("Unknown shared memory type \"" << type << "\"");
		return NULL;
	}
}

SharedMessageQueue* SharedMessageQueue::open(posix_shmem_t posix_shmem, 
		const std::string& name)
{
	shared_memory_object base(open_only,name.c_str(),read_write);
	mapped_region *region = new mapped_region(base,read_write);

	return new SharedMessageQueue(region);
}

SharedMessageQueue* SharedMessageQueue::open(sysv_shmem_t sysv_shmem, 
		const std::string& name)
{
	xsi_shared_memory base(open_only,xsi_key(name.c_str(),1));
	mapped_region *region = new mapped_region(base,read_write);

	return new SharedMessageQueue(region);
}

bool SharedMessageQueue::remove(Model::QueueModel* mdl)
{
        std::string& name = mdl->name();
        std::string& type = mdl->type();

	try {

        if(type == "posix") return remove(posix_shmem_t(),name);
        else if(type == "sysv") return remove(sysv_shmem_t(),name);
        else {
                return false;
        }
	} catch(...) {
		return false;
	}
}

bool SharedMessageQueue::remove(posix_shmem_t posix_shmem, const std::string& name)
{
	return shared_memory_object::remove(name.c_str());
}

bool SharedMessageQueue::remove(sysv_shmem_t sysv_shmem, const std::string& name)
{
	int id = xsi_shared_memory(open_only,xsi_key(name.c_str(),1)).get_shmid();
	return xsi_shared_memory::remove(id);
}

void SharedMessageQueue::send(const void* buffer)
{
	scoped_lock<interprocess_mutex> lock(shmq_hdr->main_lock);
	while(shmq_hdr->current_num_msg() == shmq_hdr->maxMsg) {
		INFO(shmq_hdr->current_num_msg());
		shmq_hdr->cond_send.wait(lock);
	}

	char* dst = data + (shmq_hdr->tail * shmq_hdr->sizeMsg);
	std::memcpy(dst,buffer,shmq_hdr->sizeMsg);
	shmq_hdr->tail += 1;
	shmq_hdr->tail %= shmq_hdr->maxMsg;

	shmq_hdr->cond_recv.notify_one();
}

bool SharedMessageQueue::trySend(const void* buffer)
{
	scoped_lock<interprocess_mutex> lock(shmq_hdr->main_lock);

	if(shmq_hdr->current_num_msg() == shmq_hdr->maxMsg) return false;

	char* dst = data + (shmq_hdr->tail * shmq_hdr->sizeMsg);
	std::memcpy(dst,buffer,shmq_hdr->sizeMsg);
	shmq_hdr->tail += 1;
	shmq_hdr->tail %= shmq_hdr->maxMsg;

	shmq_hdr->cond_recv.notify_one();

	return true;
}

void SharedMessageQueue::receive(void* buffer, size_t buffer_size)
{
	scoped_lock<interprocess_mutex> lock(shmq_hdr->main_lock);
	while(shmq_hdr->current_num_msg() == 0) {
		shmq_hdr->cond_recv.wait(lock);
	}
	char* src = data + (shmq_hdr->head * shmq_hdr->sizeMsg);
	std::memcpy(buffer,src,std::min((int)buffer_size,shmq_hdr->sizeMsg));
	shmq_hdr->head += 1;
	shmq_hdr->head %= shmq_hdr->maxMsg;

	shmq_hdr->cond_send.notify_one();	
}

bool SharedMessageQueue::tryReceive(void* buffer, 
		size_t buffer_size)
{
	scoped_lock<interprocess_mutex> lock(shmq_hdr->main_lock);

	if(shmq_hdr->current_num_msg() == 0) return false;
	char* src = data + (shmq_hdr->head * shmq_hdr->sizeMsg);
	int s = std::min((int)buffer_size,shmq_hdr->sizeMsg);
	std::memcpy(buffer,src,s);
	shmq_hdr->head += 1;
	shmq_hdr->head %= shmq_hdr->maxMsg;
	
	shmq_hdr->cond_send.notify_one();

	return true;
}

size_t SharedMessageQueue::getMaxMsg() const
{
	return shmq_hdr->maxMsg;
}

size_t SharedMessageQueue::getMaxMsgSize() const
{
	return shmq_hdr->sizeMsg;
}

size_t SharedMessageQueue::getNumMsg()
{
	scoped_lock<interprocess_mutex> lock(shmq_hdr->main_lock);
	return shmq_hdr->current_num_msg();
}

}
