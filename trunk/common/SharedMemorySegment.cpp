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
	std::string& type = model->type();
	size_t size = model->size();
	if(type == "posix") 	return new SharedMemorySegment::POSIX_ShMem(name,size);
	else if(type == "sysv") return new SharedMemorySegment::SYSV_ShMem(name,size);
	else {
		ERROR("Unknown shared memory type \"" << type << "\"");
		return NULL;
	}
}

SharedMemorySegment* SharedMemorySegment::open(Model::BufferModel* model)
{
        std::string& name = model->name();
        std::string& type = model->type();
        if(type == "posix")     return new SharedMemorySegment::POSIX_ShMem(name);
        else if(type == "sysv") return new SharedMemorySegment::SYSV_ShMem(name);
        else {
                ERROR("Unknown shared memory type \"" << type << "\"");
                return NULL;
        }
}

bool SharedMemorySegment::remove(Model::BufferModel* model)
{
	std::string& name = model->name();
	std::string& type = model->type();
	if(type == "posix") {
		return shared_memory_object::remove(name.c_str());
	}
	else if(type == "sysv") 
	{
		xsi_key key(name.c_str(),0);
		int id = shmget(key.get_key(),0,0600);
		return xsi_shared_memory::remove(id);
	}
	else {
			return false;
	}
}

SharedMemorySegment::POSIX_ShMem::POSIX_ShMem(const std::string &name, int64_t size)
{
	impl = new managed_shared_memory(create_only,name.c_str(),size);
	// shared memory created, now create the size manager
	size_manager = 
		impl->construct<SharedMemorySegment::size_manager_s>("size_manager",std::nothrow)[1](getFreeMemory());
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

SharedMemorySegment::SYSV_ShMem::SYSV_ShMem(const std::string &name, int64_t size)
{
	key = xsi_key(name.c_str(),0);
	impl = new managed_xsi_shared_memory(create_only,key,size);
	// shared memory created, now create the size manager
	size_manager = 
		impl->construct<SharedMemorySegment::size_manager_s>("size_manager",std::nothrow)[1](getFreeMemory());
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

SharedMemorySegment::ptr SharedMemorySegment::POSIX_ShMem::getAddressFromHandle(handle_t h)
{
	return impl->get_address_from_handle(h);;
}

handle_t SharedMemorySegment::POSIX_ShMem::getHandleFromAddress(SharedMemorySegment::ptr p)
{
	return impl->get_handle_from_address(p);
}

SharedMemorySegment::ptr SharedMemorySegment::POSIX_ShMem::allocate(size_t size)
{
	TRACE("Shared memory space = " << getFreeMemory());
	SharedMemorySegment::ptr t = impl->allocate(size,std::nothrow);
	TRACE("Shared memory space = " << getFreeMemory());
	return t;
}

void SharedMemorySegment::POSIX_ShMem::deallocate(void* addr)
{
	scoped_lock<interprocess_mutex> lock(size_manager->lock);
	TRACE("Shared memory space = " << getFreeMemory());
	impl->deallocate(addr);
	TRACE("Shared memory space = " << getFreeMemory());
	size_manager->size = getFreeMemory();
	size_manager->cond_size.notify_all();
}

size_t SharedMemorySegment::POSIX_ShMem::getFreeMemory()
{
	return impl->get_free_memory();
}

SharedMemorySegment::ptr SharedMemorySegment::SYSV_ShMem::getAddressFromHandle(handle_t h)
{
	return impl->get_address_from_handle(h);
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

bool SharedMemorySegment::waitAvailable(size_t size)
{
	if(size > size_manager->max) return false;

	scoped_lock<interprocess_mutex> lock(size_manager->lock);
    while(size > size_manager->size) {
        size_manager->cond_size.wait(lock);
    }
	return true;
}
}

