/**************************************************************************
This file is part of Damaris.

Damaris is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Damaris is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Damaris.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/xsi_shared_memory.hpp>

#include "util/Debug.hpp"
#include "buffer/BufferManager.hpp"
#include "buffer/SharedMemorySegment.hpp"

namespace damaris {
	
namespace bi = boost::interprocess;

static size_t alignment = 
	bi::rbtree_best_fit<boost::interprocess::mutex_family>::Alignment;

SharedMemorySegment::SharedMemorySegment(
	const std::shared_ptr<mapped_region>& r, 
	const std::shared_ptr<managed_protected_external_buffer>& b)
{
	// The header of the region is initialized in the Create function
	// that called this constructor. No need to initialize it.
	// The managed_external_buffer is also already created and initialized.
	region_ = r;
	buffer_ = b;
	size_manager_ = (struct size_manager_s*)(region_->get_address());
}

std::shared_ptr<SharedMemorySegment> 
	SharedMemorySegment::Create(const model::Buffer& model)
{
	const std::string& name = model.name();
	const model::ShmType& type = model.type();
	const unsigned int& count = model.blocks();
	size_t size = model.size();

	if(count > 1) {
		std::shared_ptr<SharedMemorySegment> shmbuffer 
			= CreateMultiBlock(model);
		shmbuffer->name_ = name;
		BufferManager::Add(shmbuffer);
		return shmbuffer;
	}

	switch(type) {

		case model::ShmType::posix :
			return Create(posix_shmem,name,size);
		case model::ShmType::sysv :
			return Create(sysv_shmem,name,size);
	}

	ERROR("Unknown shared memory type \"" << type << "\"");
	return std::shared_ptr<SharedMemorySegment>();
}

std::shared_ptr<SharedMemorySegment> SharedMemorySegment::Create(posix_shmem_t /*unused*/,
		const std::string &name, size_t size)
{
	try {
		shared_memory_object base(create_only,name.c_str(),read_write);
		size_t s = size + sizeof(struct size_manager_s);
		base.truncate(s);

		std::shared_ptr<mapped_region> region(
			new mapped_region(base,read_write,0,s));
		void* addr = region->get_address();

		void* buff_addr = ((char*)addr) + sizeof(struct size_manager_s);
		buff_addr = ((size_t)buff_addr) % alignment == 0 ? buff_addr
			: ((char*)buff_addr) + alignment - (((size_t)buff_addr) % alignment);
		size_t buff_size = s - ((size_t)(buff_addr)-(size_t)(addr));

		new (addr) size_manager_s(buff_size);

		std::shared_ptr<managed_protected_external_buffer> buffer(
				new managed_protected_external_buffer(
					create_only, buff_addr, buff_size));

		std::shared_ptr<SharedMemorySegment> shmbuffer(
				new SharedMemorySegment(region,buffer),
				Deleter<SharedMemorySegment>());
				
		shmbuffer->name_ = name;
		BufferManager::Add(shmbuffer);
		
		return shmbuffer;

	} catch(interprocess_exception &e) {
		ERROR(e.what());
	}

	return std::shared_ptr<SharedMemorySegment>();
}

std::shared_ptr<SharedMemorySegment> SharedMemorySegment::Create(sysv_shmem_t /*unused*/,
		const std::string &name, size_t size)
{
	try {
		size_t s = size + sizeof(struct size_manager_s);
		xsi_shared_memory base(create_only,xsi_key(name.c_str(),1),s);

		std::shared_ptr<mapped_region> region(
			new mapped_region(base,read_write,0,s));
		void* addr = region->get_address();
		
		void* buff_addr = (char*)addr + sizeof(struct size_manager_s);
		buff_addr = ((size_t)buff_addr) % alignment == 0 ? 
				buff_addr
				: (char*)buff_addr + alignment - (((size_t)buff_addr) % alignment);
		size_t buff_size = s - ((size_t)(buff_addr)-(size_t)(addr));

		new (addr) size_manager_s(buff_size);
		std::shared_ptr<managed_protected_external_buffer> buffer(
			new managed_protected_external_buffer(
				create_only, buff_addr, buff_size));

		std::shared_ptr<SharedMemorySegment> shmbuffer(
			new SharedMemorySegment(region,buffer),
			Deleter<SharedMemorySegment>());
		
		shmbuffer->name_ = name;
		BufferManager::Add(shmbuffer);
			
		return shmbuffer;

	} catch(interprocess_exception &e) {
		ERROR(e.what());
	}
	return std::shared_ptr<SharedMemorySegment>();
}

std::shared_ptr<SharedMemorySegment> SharedMemorySegment::Open(const model::Buffer& model)
{
	const std::string& name = model.name();
	const model::ShmType& type = model.type();
	const unsigned int& count = model.blocks();
	size_t size = model.size();

	if(count > 1) {
		std::shared_ptr<SharedMemorySegment> shmbuffer 
			= OpenMultiBlock(model);
		
		shmbuffer->name_ = name;
		BufferManager::Add(shmbuffer);
		
		return shmbuffer;
	}

	switch(type) {

		case model::ShmType::posix :
			return Open(posix_shmem,name,size);
		case model::ShmType::sysv :
			return Open(sysv_shmem,name,size);
	}

	ERROR("Unknown shared memory type \"" << type << "\"");
	return std::shared_ptr<SharedMemorySegment>();
}

std::shared_ptr<SharedMemorySegment> SharedMemorySegment::Open(posix_shmem_t /*unused*/,
	const std::string &name, size_t size)
{
	try {
		shared_memory_object base(open_only,name.c_str(),read_write);
		size_t s = size + sizeof(struct size_manager_s);
		base.truncate(s);

		std::shared_ptr<mapped_region> region(
			new mapped_region(base,read_write,0,s));
		void* addr = region->get_address();

		void* buff_addr = (char*)addr + sizeof(struct size_manager_s);
		buff_addr = ((size_t)buff_addr) % alignment == 0 ? buff_addr
			: (char*)buff_addr + alignment - (((size_t)buff_addr) % alignment);
		size_t buff_size = s - ((size_t)(buff_addr)-(size_t)(addr));

		std::shared_ptr<managed_protected_external_buffer> buffer(
				new managed_protected_external_buffer(
					open_only, buff_addr, buff_size));

		std::shared_ptr<SharedMemorySegment> shmbuffer(
				new SharedMemorySegment(region,buffer),
				Deleter<SharedMemorySegment>());
		
		shmbuffer->name_ = name;
		BufferManager::Add(shmbuffer);
		
		return shmbuffer;
		
	} catch(interprocess_exception &e) {
		ERROR(e.what());
	}

	return std::shared_ptr<SharedMemorySegment>();
}

std::shared_ptr<SharedMemorySegment> SharedMemorySegment::Open(sysv_shmem_t /*unused*/,
	const std::string &name, size_t size)
{
	try {
		size_t s = size + sizeof(struct size_manager_s);
		xsi_shared_memory base(open_only,xsi_key(name.c_str(),1));

		std::shared_ptr<mapped_region> region(
			new mapped_region(base,read_write,0,s));

		void* addr = region->get_address();
		void* buff_addr = (char*)addr + sizeof(struct size_manager_s);
		buff_addr = ((size_t)buff_addr) % alignment == 0 ? buff_addr
			: (char*)buff_addr + alignment - (((size_t)buff_addr) % alignment);
		size_t buff_size = s - ((size_t)(buff_addr)-(size_t)(addr));

		std::shared_ptr<managed_protected_external_buffer> buffer(
			new managed_protected_external_buffer(
				open_only, buff_addr, buff_size));

		std::shared_ptr<SharedMemorySegment> shmbuffer(
			new SharedMemorySegment(region,buffer),
			Deleter<SharedMemorySegment>());
		
		shmbuffer->name_ = name;
		BufferManager::Add(shmbuffer);
		
		return shmbuffer;

	} catch(interprocess_exception &e) {
		ERROR(e.what());
	}
	return std::shared_ptr<SharedMemorySegment>();
}

bool SharedMemorySegment::Remove(const model::Buffer& model)
{
	const std::string& name = model.name();
	const model::ShmType& type = model.type();
	const unsigned int& count = model.blocks();

	if(count > 1) {
		std::shared_ptr<Buffer> b = BufferManager::Search(name);
		BufferManager::Delete(b);
		return RemoveMultiBlock(model);
	}

	switch(type) {
		case model::ShmType::posix :
			return Remove(posix_shmem,name);
		case model::ShmType::sysv :
			return Remove(sysv_shmem,name);
	}

	return false;
}

bool SharedMemorySegment::Remove(posix_shmem_t /*unused*/, 
	const std::string &name)
{
	try {
		std::shared_ptr<Buffer> b = BufferManager::Search(name);
		BufferManager::Delete(b);
		return shared_memory_object::remove(name.c_str());
	} catch(...) {
	}
	return false;
}

bool SharedMemorySegment::Remove(sysv_shmem_t /*unused*/, 
	const std::string &name)
{
	try {
		std::shared_ptr<Buffer> b = BufferManager::Search(name);
		BufferManager::Delete(b);
		int id = shmget(xsi_key(name.c_str(),0).get_key(),0,0600);
		return xsi_shared_memory::remove(id);
	} catch(...) {
		return false;
	}
}

bool SharedMemorySegment::RemoveMultiBlock(const model::Buffer& b)
{
	int count = b.blocks();
	const std::string& name = b.name();
	try {
		bool res = false;
		int id;

		switch(b.type()) {
			case model::ShmType::posix :

				for(int k = 0; k < (int)count; k++) {
					std::stringstream ss;
					ss << name << "_" << k;
					res = res || 
						shared_memory_object::remove(ss.str().c_str());
				}		
				return res;
				
			case model::ShmType::sysv :
				for(int k = 0; k < (int)count; k++) {
					std::stringstream ss;
					ss << name << "_" << k;
					xsi_key key(ss.str().c_str(),0);
					id = shmget(key.get_key(),0,0600);
					res = res || 
						xsi_shared_memory::remove(id);
				}
				return res;
		}

	} catch(...) {
	}
	return false;
}

Buffer::ptr SharedMemorySegment::GetAddressFromHandle(const Handle& h) const
{
	return buffer_->get_address_from_handle(h.GetValue());
}

Handle SharedMemorySegment::GetHandleFromAddress(const Buffer::ptr p) const
{
	return buffer_->get_handle_from_address(p);
}

DataSpace<Buffer> SharedMemorySegment::Allocate(const size_t& size)
{
	
	Buffer::ptr t = buffer_->allocate(size,std::nothrow);
	if(t != NULL) {
		DataSpace<Buffer> ds(SHARED_FROM_THIS(),t,size);
		return ds;
	}
	return DataSpace<Buffer>();
}

DataSpace<Buffer> SharedMemorySegment::AllocateAligned(const size_t& size)
{
	int pagesize = sysconf(_SC_PAGE_SIZE);
	Buffer::ptr t = buffer_->allocate_aligned(size,pagesize,std::nothrow);
	if(t != NULL) {
		DataSpace<Buffer> ds(SHARED_FROM_THIS(),t,size);
		return ds;
	}
	return DataSpace<Buffer>();
}

void SharedMemorySegment::Deallocate(Buffer::ptr addr)
{
	scoped_lock<interprocess_mutex> lock(size_manager_->lock);
	buffer_->deallocate(addr);
	size_t newsize = GetFreeMemory();
	size_manager_->size = newsize;
	size_manager_->cond_size.notify_all();
}

size_t SharedMemorySegment::GetFreeMemory() const
{
	return buffer_->get_free_memory();
}

bool SharedMemorySegment::PointerBelongsToSegment(const Buffer::ptr p) const
{
	return buffer_->belongs_to_segment(p);
}

bool SharedMemorySegment::WaitAvailable(const size_t& size)
{
	if(size > size_manager_->max) return false;
	
	scoped_lock<interprocess_mutex> lock(size_manager_->lock);
	while(size > GetFreeMemory()) {
		size_manager_->cond_size.wait(lock);
	}
	return true;
}

std::shared_ptr<SharedMemorySegment> 
	SharedMemorySegment::CreateMultiBlock(const model::Buffer& model)
{
	int count = model.blocks();
	size_t size = model.size();
	const std::string &name = model.name();
	const model::ShmType &type = model.type();

	std::vector<std::shared_ptr<SharedMemorySegment> > v(count);

	for(int i=0; i < count; i++) {
		std::stringstream ss;
		ss << name << "_" << i;
		switch(type) {
		case model::ShmType::posix :
			v[i] = SharedMemorySegment::Create(
					posix_shmem,ss.str(),size);
			if(not v[i]) {
				ERROR("While opening composite segment " << i);
				WARN("Other shared memory structures "
					<< "may be left open");
				return std::shared_ptr<SharedMemorySegment>();
			}
			break;
		case model::ShmType::sysv :
			v[i] = SharedMemorySegment::Create(
					sysv_shmem,ss.str(),size);
			if(not v[i]) {
				ERROR("While opening composite segment " << i);
				WARN("Other shared memory structures "
					<< "may be left open");
				return std::shared_ptr<SharedMemorySegment>();
			}
			break;
		default:
			ERROR("Undefined shared memory type " << type);
			return std::shared_ptr<SharedMemorySegment>();
		}
	}
	return std::shared_ptr<SharedMemorySegment>(new CompositeShMem(count,v),
						Deleter<SharedMemorySegment>());
}

std::shared_ptr<SharedMemorySegment> 
	SharedMemorySegment::OpenMultiBlock(const model::Buffer& model)
{
	const int& count = model.blocks();
	const size_t& size = model.size();
	const std::string &name = model.name();
	const model::ShmType &type = model.type();
	
	std::vector<std::shared_ptr<SharedMemorySegment> > v(count);

	for(int i=0; i < count; i++) {
		std::stringstream ss;
		ss << name << "_" << i;
		switch(type) {
		case model::ShmType::posix :
			v[i] = SharedMemorySegment::Open(
				posix_shmem,ss.str(),size);
			if(not v[i]) {
				ERROR("While opening composite segment " << i);
				WARN("Other shared memory structures"
					<< " may be left open");
				return std::shared_ptr<SharedMemorySegment>();
			}
			break;
		case model::ShmType::sysv :
			v[i] = SharedMemorySegment::Open(
				sysv_shmem,ss.str(),size);
			if(not v[i]) {
				ERROR("While opening composite segment " << i);
				WARN("Other shared memory structures"
					<< " may be left open");
				return std::shared_ptr<SharedMemorySegment>();
			}
			break;
		default:
			ERROR("Undefined shared memory type " << type);
			return std::shared_ptr<SharedMemorySegment>();
		}
	} 
	return std::shared_ptr<SharedMemorySegment>(new CompositeShMem(count,v),
						Deleter<SharedMemorySegment>());
}

SharedMemorySegment::CompositeShMem::CompositeShMem(int count, 
	const std::vector<std::shared_ptr<SharedMemorySegment> >& v)
	: SharedMemorySegment(v[0]->region_,v[0]->buffer_)
	// calling the parent's constructor will ensure that size_manager points
	// to the size_manager of the first block. The other size managers will 
	// never be actually used. We should eventually get rid of them in some 
	// way.
{
	nbseg_ = count;
	blocks_ = v;
}

Buffer::ptr 
SharedMemorySegment::CompositeShMem::GetAddressFromHandle(const Handle& h) const
{
	int i = h.GetID();
	if(i < 0 or i >= nbseg_) {
		ERROR("Invalid object id");
		return NULL;
	}
	return blocks_[i]->GetAddressFromHandle(h);
}

Handle SharedMemorySegment::CompositeShMem::GetHandleFromAddress(
	const Buffer::ptr p) const
{
	int id = 0;
	bool found = false;
	for(int i=0;i<nbseg_;i++) {
		if(blocks_[i]->PointerBelongsToSegment(p)) {
			id = i;
			found = true;
			break;
		}
	}
	if(not found) {
		ERROR("Pointer does not belong to any segment");
		return Handle();
	}
	Handle h = blocks_[id]->GetHandleFromAddress(p);
	h.SetID(id);
	return h;
}

DataSpace<Buffer> 
SharedMemorySegment::CompositeShMem::Allocate(const size_t& size)
{
	// tries to find a segment that have enough free space
	DataSpace<Buffer> res;

	if(size == 0) return res;

	for(int i = 0; i < nbseg_; i++) {
		res = blocks_[i]->Allocate(size);
		if(res.GetSize() == size) return res;
	}
	return DataSpace<Buffer>();
}

DataSpace<Buffer>
SharedMemorySegment::CompositeShMem::AllocateAligned(const size_t& size)
{
	if(size == 0) return DataSpace<Buffer>();
	// tries to find a segment that have enough free space
	DataSpace<Buffer> res;
	for(int i = 0; i < nbseg_; i++) {
		res = blocks_[i]->AllocateAligned(size);
		if(res.GetSize() == size) return res;
	}
	return DataSpace<Buffer>();
}

void SharedMemorySegment::CompositeShMem::Deallocate(const Buffer::ptr addr)
{
	// find the segment in charge of this address
	for(int i=0; i < nbseg_; i++) {
		if(blocks_[i]->PointerBelongsToSegment(addr)) {
			blocks_[i]->Deallocate(addr);
			return;
		}
	}
}

size_t SharedMemorySegment::CompositeShMem::GetFreeMemory() const
{
	// this function actually returns the maximum free memory
	// that we can find in a block, since the user in not allowed
	// to allocate more than this amount anyway.
	size_t max = 0;
	for(int i = 0 ; i < nbseg_; i++)
	{
		size_t m = blocks_[i]->GetFreeMemory();
		max = (m > max) ? m : max;
	}
	return max;
}

bool SharedMemorySegment::CompositeShMem::PointerBelongsToSegment(
	const Buffer::ptr p) const
{
	bool res = false;
	for(int i = 0; i < nbseg_; i++) {
		res = res || blocks_[i]->PointerBelongsToSegment(p);
	}
	return res; 
}

}

