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
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/xsi_shared_memory.hpp>
#include "core/Debug.hpp"
#include "core/SharedMemorySegment.hpp"

namespace Damaris {

	SharedMemorySegment::SharedMemorySegment(mapped_region* r, managed_external_buffer* b)
	{
		// The header of the region is initialized in the Create function
		// that called this constructor. No need to initialize it.
		// The managed_external_buffer is also already created and initialized.
		region = r;
		buffer = b;
		size_manager = (struct size_manager_s*)(region->get_address());
	}

	SharedMemorySegment::~SharedMemorySegment()
	{
		delete region;
		delete buffer;
	}

	SharedMemorySegment* SharedMemorySegment::Create(Model::Buffer* model)
	{
		std::string& name = model->name();
		Model::ShmType& type = model->type();
		unsigned int& count = model->blocks();
		size_t size = model->size();

		if(count > 1) {
			return CreateMultiBlock(model);
		}

		switch(type) {

			case Model::ShmType::posix :
				return Create(posix_shmem_t(),name,size);
			case Model::ShmType::sysv :
				return Create(sysv_shmem_t(),name,size);
		}

		ERROR("Unknown shared memory type \"" << type << "\"");
		return NULL;
	}

	SharedMemorySegment* SharedMemorySegment::Create(posix_shmem_t posix_shmem,
			const std::string &name, size_t size)
	{
		try {
			shared_memory_object base(create_only,name.c_str(),read_write);
			size_t s = size + sizeof(struct size_manager_s);
			base.truncate(s);

			mapped_region *region = new mapped_region(base,read_write,0,s);
			void* addr = region->get_address();

			new (addr) size_manager_s(size);

			managed_external_buffer *buffer = 
				new managed_external_buffer(create_only, ((char*)addr)+sizeof(struct size_manager_s), size);

			return new SharedMemorySegment(region,buffer);

		} catch(interprocess_exception &e) {
			ERROR(e.what());
		}

		return NULL;
	}

	SharedMemorySegment* SharedMemorySegment::Create(sysv_shmem_t sysv_shmem,
			const std::string &name, size_t size)
	{
		try {
			size_t s = size + sizeof(struct size_manager_s);
			xsi_shared_memory base(create_only,xsi_key(name.c_str(),1),s);

			mapped_region *region = new mapped_region(base,read_write,0,s);
			void* addr = region->get_address();
			new (addr) size_manager_s(size);

			managed_external_buffer *buffer = 
				new managed_external_buffer(create_only, ((char*)addr)+sizeof(struct size_manager_s), size);

			return new SharedMemorySegment(region,buffer);

		} catch(interprocess_exception &e) {
			ERROR(e.what());
		}
		return NULL;
	}

	SharedMemorySegment* SharedMemorySegment::Open(Model::Buffer* model)
	{
		std::string& name = model->name();
		Model::ShmType& type = model->type();
		unsigned int& count = model->blocks();
		size_t size = model->size();

		if(count > 1) {
			return OpenMultiBlock(model);
		}

		switch(type) {

			case Model::ShmType::posix :
				return Open(posix_shmem_t(),name,size);
			case Model::ShmType::sysv :
				return Open(sysv_shmem_t(),name,size);
		}

		ERROR("Unknown shared memory type \"" << type << "\"");
		return NULL;
	}

	SharedMemorySegment* SharedMemorySegment::Open(posix_shmem_t posix_shmem,
			const std::string &name, size_t size)
	{
		try {
			shared_memory_object base(open_only,name.c_str(),read_write);
			size_t s = size + sizeof(struct size_manager_s);
			base.truncate(s);

			mapped_region *region = new mapped_region(base,read_write,0,s);
			void* addr = region->get_address();

			managed_external_buffer *buffer =
				new managed_external_buffer(open_only, ((char*)addr)+sizeof(struct size_manager_s), size);

			return new SharedMemorySegment(region,buffer);

		} catch(interprocess_exception &e) {
			ERROR(e.what());
		}

		return NULL;
	}

	SharedMemorySegment* SharedMemorySegment::Open(sysv_shmem_t sysv_shmem,
			const std::string &name, size_t size)
	{
		try {
			size_t s = size + sizeof(struct size_manager_s);
			xsi_shared_memory base(open_only,xsi_key(name.c_str(),1));

			mapped_region *region = new mapped_region(base,read_write,0,s);

			void* addr = region->get_address();
			managed_external_buffer *buffer =
				new managed_external_buffer(open_only, ((char*)addr)+sizeof(struct size_manager_s), size);

			return new SharedMemorySegment(region,buffer);

		} catch(interprocess_exception &e) {
			ERROR(e.what());
		}
		return NULL;
	}

	bool SharedMemorySegment::Remove(Model::Buffer* model)
	{
		std::string& name = model->name();
		Model::ShmType& type = model->type();
		unsigned int& count = model->blocks();

		if(count > 1) {
			return RemoveMultiBlock(model);
		}

		switch(type) {
			case Model::ShmType::posix :
				return Remove(posix_shmem_t(),name);
			case Model::ShmType::sysv :
				return Remove(sysv_shmem_t(),name);
		}

		return false;
	}

	bool SharedMemorySegment::Remove(posix_shmem_t posix_shmem, const std::string &name)
	{
		try {
			return shared_memory_object::remove(name.c_str());
		} catch(...) {
		}
		return false;
	}

	bool SharedMemorySegment::Remove(sysv_shmem_t sysv_shmem, const std::string &name)
	{
		try {
			int id = shmget(xsi_key(name.c_str(),0).get_key(),0,0600);
			return xsi_shared_memory::remove(id);
		} catch(...) {
			return false;
		}
	}

	bool SharedMemorySegment::RemoveMultiBlock(const Model::Buffer* b)
	{

		int count = b->blocks();
		const std::string& name = b->name();
		try {
			bool res = false;
			int id;

			switch(b->type()) {
				case Model::ShmType::posix :

					for(int k = 0; k < (int)count; k++) {
						std::stringstream ss;
						ss << name << "_" << k;
						res = res || shared_memory_object::remove(ss.str().c_str());
					}		
					return res;

				case Model::ShmType::sysv :
					for(int k = 0; k < (int)count; k++) {
						std::stringstream ss;
						ss << name << "_" << k;
						xsi_key key(ss.str().c_str(),0);
						id = shmget(key.get_key(),0,0600);
						res = res || xsi_shared_memory::remove(id);
					}
					return res;
			}

		} catch(...) {
		}
		return false;
	}

	SharedMemorySegment::ptr SharedMemorySegment::getAddressFromHandle(handle_t h) const
	{
		return buffer->get_address_from_handle(h.value);
	}

	handle_t SharedMemorySegment::getHandleFromAddress(SharedMemorySegment::ptr p) const
	{
		return buffer->get_handle_from_address(p);
	}

	SharedMemorySegment::ptr SharedMemorySegment::allocate(size_t size)
	{
		SharedMemorySegment::ptr t = buffer->allocate(size,std::nothrow);
		if(t != NULL) {
			DBG("Allocated: " << size);
		}
		return t;
	}

	void SharedMemorySegment::deallocate(void* addr)
	{
		size_t oldsize __attribute__((unused)) = getFreeMemory();
		scoped_lock<interprocess_mutex> lock(size_manager->lock);
		buffer->deallocate(addr);
		size_t newsize = getFreeMemory();
		size_manager->size = newsize;
		size_manager->cond_size.notify_all();
		DBG("Deallocated: " << (newsize - oldsize));
	}

	size_t SharedMemorySegment::getFreeMemory() const
	{
		return buffer->get_free_memory();
	}

	bool SharedMemorySegment::pointerBelongsToSegment(void* p) const
	{
		return buffer->belongs_to_segment(p);
	}

	bool SharedMemorySegment::waitAvailable(size_t size)
	{
		if(size > size_manager->max) return false;

		scoped_lock<interprocess_mutex> lock(size_manager->lock);
		while(size > getFreeMemory()) {
			size_manager->cond_size.wait(lock);
		}
		return true;
	}

	SharedMemorySegment* SharedMemorySegment::CreateMultiBlock(Model::Buffer* model)
	{
		int count = model->blocks();
		size_t size = model->size();
		std::string &name = model->name();
		Model::ShmType &type = model->type();

		std::vector<SharedMemorySegment*>* v = new std::vector<SharedMemorySegment*>();
		v->resize(count,NULL);

		for(int i=0; i < count; i++) {
			std::stringstream ss;
			ss << name << "_" << i;
			switch(type) {
				case Model::ShmType::posix :
					(*v)[i] = SharedMemorySegment::Create(posix_shmem_t(),ss.str(),size);
					if((*v)[i] == NULL) {
						ERROR("While opening composite segment " << i);
						WARN("Other shared memory structures may be left open");
						return NULL;
					}
					break;
				case Model::ShmType::sysv :
					(*v)[i] = SharedMemorySegment::Create(sysv_shmem_t(),ss.str(),size);
					if((*v)[i] == NULL) {
						ERROR("While opening composite segment " << i);
						WARN("Other shared memory structures may be left open");
						return NULL;
					}
					break;
				default:
					ERROR("Undefined shared memory type " << type);
					return NULL;
			}
		}
		return new CompositeShMem(count,v);
	}

	SharedMemorySegment* SharedMemorySegment::OpenMultiBlock(Model::Buffer* model)
	{
		int count = model->blocks();
		size_t size = model->size();
		std::string &name = model->name();
		Model::ShmType &type = model->type();

		std::vector<SharedMemorySegment*>* v = new std::vector<SharedMemorySegment*>();
		v->resize(count,NULL);

		for(int i=0; i < count; i++) {
			std::stringstream ss;
			ss << name << "_" << i;
			switch(type) {
				case Model::ShmType::posix :
					(*v)[i] = SharedMemorySegment::Open(posix_shmem_t(),ss.str(),size);
					if((*v)[i] == NULL) {
						ERROR("While opening composite segment " << i);
						WARN("Other shared memory structures may be left open");
						return NULL;
					}
					break;
				case Model::ShmType::sysv :
					(*v)[i] = SharedMemorySegment::Open(sysv_shmem_t(),ss.str(),size);
					if((*v)[i] == NULL) {
						ERROR("While opening composite segment " << i);
						WARN("Other shared memory structures may be left open");
						return NULL;
					}
					break;
				default:
					ERROR("Undefined shared memory type " << type);
					return NULL;
			}
		} 
		return new CompositeShMem(count,v);
	}

	SharedMemorySegment::CompositeShMem::CompositeShMem(int count, std::vector<SharedMemorySegment*>* v)
		: SharedMemorySegment((*v)[0]->region,(*v)[0]->buffer)
	// calling the parent's constructor will ensure that size_manager points to the size_manager of
	// the first block. The other size managers will never be actually used. We should eventually get
	// rid of them in some way.
	{
		nbseg = count;
		blocks = v;
	}

	SharedMemorySegment::ptr SharedMemorySegment::CompositeShMem::getAddressFromHandle(handle_t h) const
	{
		if(h.objid < 0 or h.objid >= nbseg) {
			ERROR("Invalid object id");
			return NULL;
		}
		return (*blocks)[h.objid]->getAddressFromHandle(h);
	}

	handle_t SharedMemorySegment::CompositeShMem::getHandleFromAddress(SharedMemorySegment::ptr p) const
	{
		int id = 0;
		bool found = false;
		for(int i=0;i<nbseg;i++) {
			if((*blocks)[i]->pointerBelongsToSegment(p)) {
				id = i;
				found = true;
				break;
			}
		}
		if(not found) {
			ERROR("Pointer does not belong to any segment");
			return handle_t();
		}
		handle_t h = (*blocks)[id]->getHandleFromAddress(p);
		h.objid = id;
		return h;
	}

	SharedMemorySegment::ptr SharedMemorySegment::CompositeShMem::allocate(size_t size)
	{
		// tries to find a segment that have enough free space
		void* res = NULL;
		for(int i = 0; i < nbseg; i++) {
			res = (*blocks)[i]->allocate(size);
			if(res != NULL) return res;
		}
		return NULL;
	}

	void SharedMemorySegment::CompositeShMem::deallocate(void* addr)
	{
		// find the segment in charge of this address
		for(int i=0; i < nbseg; i++) {
			if((*blocks)[i]->pointerBelongsToSegment(addr)) {
				(*blocks)[i]->deallocate(addr);
				return;
			}
		}
	}

	size_t SharedMemorySegment::CompositeShMem::getFreeMemory() const
	{
		// this function actually returns the maximum free memory
		// that we can find in a block, since the user in not allowed
		// to allocate more than this amount anyway.
		size_t max = 0;
		for(int i = 0 ; i<nbseg; i++)
		{
			size_t m = (*blocks)[i]->getFreeMemory();
			max = (m > max) ? m : max;
		}
		return max;
	}

	bool SharedMemorySegment::CompositeShMem::pointerBelongsToSegment(void* p) const
	{
		bool res = false;
		for(int i = 0; i < nbseg; i++) {
			res = res || (*blocks)[i]->pointerBelongsToSegment(p);
		}
		return res; 
	}

	SharedMemorySegment::CompositeShMem::~CompositeShMem()
	{
		for(int i = 0; i < nbseg; i++) {
			delete (*blocks)[i];
		}
		delete blocks;
	}
}

