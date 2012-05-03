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
 * \file SharedMemorySegment.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/xsi_shared_memory.hpp>
#include "common/Debug.hpp"
#include "common/SharedMemorySegment.hpp"

namespace Damaris {

SharedMemorySegment::SharedMemorySegment() {}

SharedMemorySegment* SharedMemorySegment::create(Model::BufferModel* model)
{
	std::string& name = model->name();
	Model::ShmType& type = model->type();
	unsigned int& count = model->blocks();
	size_t size = model->size();

	switch(type) {

	case Model::ShmType::posix :
		if(count > 1)
			return new SharedMemorySegment::CompositeShMem(name,size,count,posix_shmem);
		else
			return new SharedMemorySegment::POSIX_ShMem(name,size);
		break;

	case Model::ShmType::sysv :
		if(count > 1)
			return new SharedMemorySegment::CompositeShMem(name,size,count,sysv_shmem);
		else
			return new SharedMemorySegment::SYSV_ShMem(name,size);
	}

	ERROR("Unknown shared memory type \"" << type << "\"");
	return NULL;
}

SharedMemorySegment* SharedMemorySegment::open(Model::BufferModel* model)
{
	std::string& name = model->name();
	Model::ShmType& type = model->type();
	unsigned int& count = model->blocks();

	switch(type) {

	case Model::ShmType::posix :
		if(count > 1)
			return new SharedMemorySegment::CompositeShMem(name,count,posix_shmem);
		else
			return new SharedMemorySegment::POSIX_ShMem(name);
	break; 

	case Model::ShmType::sysv :
			if(count > 1)
				return new SharedMemorySegment::CompositeShMem(name,count,sysv_shmem);
			else
				return new SharedMemorySegment::SYSV_ShMem(name);
	}

	ERROR("Unknown shared memory type \"" << type << "\"");
	return NULL;
}

bool SharedMemorySegment::remove(Model::BufferModel* model)
{
	std::string& name = model->name();
	Model::ShmType& type = model->type();
	unsigned int& count = model->blocks();

	try {

	bool res = false;
	std::stringstream ss;	
	int id;

	switch(type) {
	
	case Model::ShmType::posix :
		if(count > 1) {
			for(int k = 0; k < (int)count; k++) {
				ss << name << "_" << k;
				res = res || shared_memory_object::remove(ss.str().c_str());
			}	
			return res;
		} else {	
			return shared_memory_object::remove(name.c_str());
		}
	break;

	case Model::ShmType::sysv :
		if(count > 1) {
			for(int k = 0; k < (int)count; k++) {
				xsi_key key(name.c_str(),k);
            	id = shmget(key.get_key(),0,0600);
            	res = res || xsi_shared_memory::remove(id);
			}
			return res;
		} else {
			id = shmget(xsi_key(name.c_str(),0).get_key(),0,0600);
			return xsi_shared_memory::remove(id);
		}	
	}
	} catch(...) {
	}
	return false;
}

SharedMemorySegment::POSIX_ShMem::POSIX_ShMem(const std::string &name, int64_t size)
{
	impl = new managed_shared_memory(create_only,name.c_str(),size);
	// shared memory created, now create the size manager
	size_manager = 
		impl->construct<SharedMemorySegment::size_manager_s>
			("size_manager",std::nothrow)[1](getFreeMemory());
	FATAL((size_manager == NULL), "Unable to create the size manager");
}

SharedMemorySegment::POSIX_ShMem::POSIX_ShMem(const std::string &name)
{
	impl = new managed_shared_memory(open_only,name.c_str());
	std::pair<SharedMemorySegment::size_manager_s*,size_t> ret 
		= impl->find<SharedMemorySegment::size_manager_s>("size_manager");
	FATAL((ret.second != 1),
			"Size manager not found or multiple initializations");
	size_manager = ret.first;
}

SharedMemorySegment::SYSV_ShMem::SYSV_ShMem(const xsi_key &key, int64_t size)
{
	impl = new managed_xsi_shared_memory(create_only,key,size);
    // shared memory created, now create the size manager
    size_manager =
        impl->construct<SharedMemorySegment::size_manager_s>
			("size_manager",std::nothrow)[1](getFreeMemory());
    FATAL((size_manager == NULL),"Unable to create the size manager");
}

SharedMemorySegment::SYSV_ShMem::SYSV_ShMem(const std::string &name, int64_t size)
{
	key = xsi_key(name.c_str(),0);
	impl = new managed_xsi_shared_memory(create_only,key,size);
	// shared memory created, now create the size manager
	size_manager = 
		impl->construct<SharedMemorySegment::size_manager_s>
			("size_manager",std::nothrow)[1](getFreeMemory());
	FATAL((size_manager == NULL),"Unable to create the size manager");
}

SharedMemorySegment::SYSV_ShMem::SYSV_ShMem(const std::string &name)
{
	key = xsi_key(name.c_str(),0);
	impl = new managed_xsi_shared_memory(open_only,key);
	std::pair<SharedMemorySegment::size_manager_s*,size_t> ret 
		= impl->find<SharedMemorySegment::size_manager_s>("size_manager");
	FATAL((ret.second != 1), "Size manager not found of multiple initializations");
	size_manager = ret.first;
}

SharedMemorySegment::SYSV_ShMem::SYSV_ShMem(const xsi_key &key)
{
    impl = new managed_xsi_shared_memory(open_only,key);
    std::pair<SharedMemorySegment::size_manager_s*,size_t> ret
        = impl->find<SharedMemorySegment::size_manager_s>("size_manager");
    FATAL((ret.second != 1), "Size manager not found of multiple initializations");
    size_manager = ret.first;
}

SharedMemorySegment::ptr SharedMemorySegment::POSIX_ShMem::getAddressFromHandle(handle_t h)
{
	return impl->get_address_from_handle(h.value);;
}

handle_t SharedMemorySegment::POSIX_ShMem::getHandleFromAddress(SharedMemorySegment::ptr p)
{
	return impl->get_handle_from_address(p);
}

SharedMemorySegment::ptr SharedMemorySegment::POSIX_ShMem::allocate(size_t size)
{
	SharedMemorySegment::ptr t = impl->allocate(size,std::nothrow);
	return t;
}

void SharedMemorySegment::POSIX_ShMem::deallocate(void* addr)
{
	scoped_lock<interprocess_mutex> lock(size_manager->lock);
	impl->deallocate(addr);
	size_t newsize = getFreeMemory();
	size_manager->size = newsize;
	size_manager->cond_size.notify_all();
}

size_t SharedMemorySegment::POSIX_ShMem::getFreeMemory()
{
	return impl->get_free_memory();
}

bool SharedMemorySegment::POSIX_ShMem::pointerBelongsToSegment(void* p)
{
	bool res = (int64_t)impl->get_address() <= (int64_t)p;
	res = res && (int64_t)p < (int64_t)((char*)impl->get_address() + impl->get_size());
	return res;
}

SharedMemorySegment::ptr SharedMemorySegment::SYSV_ShMem::getAddressFromHandle(handle_t h)
{
	return impl->get_address_from_handle(h.value);
}

handle_t SharedMemorySegment::SYSV_ShMem::getHandleFromAddress(SharedMemorySegment::ptr p) 
{               
	return impl->get_handle_from_address(p);;
}

SharedMemorySegment::ptr SharedMemorySegment::SYSV_ShMem::allocate(size_t size)
{
	return impl->allocate(size,std::nothrow);
}

void SharedMemorySegment::SYSV_ShMem::deallocate(void* addr) 
{               
	impl->deallocate(addr);
}

size_t SharedMemorySegment::SYSV_ShMem::getFreeMemory()
{
	return impl->get_free_memory();
}

bool SharedMemorySegment::SYSV_ShMem::pointerBelongsToSegment(void* p)
{
    bool res = (int64_t)impl->get_address() <= (int64_t)p;
    res = res && (int64_t)p < (int64_t)((char*)impl->get_address() + impl->get_size());
    return res; 
}

bool SharedMemorySegment::waitAvailable(size_t size)
{
	if(size > size_manager->max) return false;

	scoped_lock<interprocess_mutex> lock(size_manager->lock);
    while(size > size_manager->size) {
        size_manager->cond_size.wait(lock);
    }
	return true;
}

SharedMemorySegment::CompositeShMem::CompositeShMem(const std::string &name, int64_t size, 
													int count, posix_shmem_t p)
{
	nbseg = count;
	blocks.resize(count,NULL);

	for(int i=0; i<nbseg; i++) {
		std::stringstream ss;
		ss << name << "_" << i;
		blocks[i] = new SharedMemorySegment::POSIX_ShMem(ss.str(),size);
	}

	size_manager = blocks[0]->size_manager;
}

SharedMemorySegment::CompositeShMem::CompositeShMem(const std::string &name, int64_t size,
													int count, sysv_shmem_t s)
{
	nbseg = count;
    blocks.resize(count,NULL);
	
	for(int i=0; i<nbseg; i++) {
		xsi_key k(name.c_str(),i);
		blocks[i] = new SharedMemorySegment::SYSV_ShMem(k,size);
	}

	size_manager = blocks[0]->size_manager;
}

SharedMemorySegment::CompositeShMem::CompositeShMem(const std::string &name, int count, 
													posix_shmem_t p)
{
	nbseg = count;
    blocks.resize(count,NULL);
	
	for(int i=0; i<nbseg; i++) {
        std::stringstream ss;
        ss << name << "_" << i;
        blocks[i] = new SharedMemorySegment::POSIX_ShMem(ss.str());
    }

	size_manager = blocks[0]->size_manager;
}

SharedMemorySegment::CompositeShMem::CompositeShMem(const std::string &name, int count,
													sysv_shmem_t s)
{
	nbseg = count;
    blocks.resize(count,NULL);
    
	for(int i=0; i<nbseg; i++) {
        xsi_key k(name.c_str(),i);
        blocks[i] = new SharedMemorySegment::SYSV_ShMem(k);
    }

	size_manager = blocks[0]->size_manager;
}

SharedMemorySegment::ptr SharedMemorySegment::CompositeShMem::getAddressFromHandle(handle_t h)
{
	if(h.objid < 0 or h.objid >= nbseg) {
		ERROR("Invalid object id");
		return NULL;
	}
	return blocks[h.objid]->getAddressFromHandle(h);
}

handle_t SharedMemorySegment::CompositeShMem::getHandleFromAddress(SharedMemorySegment::ptr p)
{
	int id = 0;
	bool found = false;
	for(int i=0;i<nbseg;i++) {
		if(blocks[i]->pointerBelongsToSegment(p)) {
			id = i;
			found = true;
			break;
		}
	}
	if(not found) {
		ERROR("Pointer does not belong to any segment");
		return handle_t();
	}
	handle_t h = blocks[id]->getHandleFromAddress(p);
	h.objid = id;
	return h;
}

SharedMemorySegment::ptr SharedMemorySegment::CompositeShMem::allocate(size_t size)
{
	// tries to find a segment that have enough free space
	void* res = NULL;
	for(int i = 0; i < nbseg; i++) {
		res = blocks[i]->allocate(size);
		if(res != NULL) return res;
	}
	return NULL;
}

void SharedMemorySegment::CompositeShMem::deallocate(void* addr)
{
	// find the segment in charge of this address
	for(int i=0; i < nbseg; i++) {
		if(blocks[i]->pointerBelongsToSegment(addr)) {
			blocks[i]->deallocate(addr);
			return;
		}
	}
}

size_t SharedMemorySegment::CompositeShMem::getFreeMemory()
{
	size_t total = 0;
	for(int i = 0 ; i<nbseg; i++)
	{
		total += blocks[i]->getFreeMemory();
	}
	return total;
}

bool SharedMemorySegment::CompositeShMem::pointerBelongsToSegment(void* p)
{
    bool res = false;
    for(int i = 0; i < nbseg; i++) {
		res = res || blocks[i]->pointerBelongsToSegment(p);
	}
	return res; 
}

}

