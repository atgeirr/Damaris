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

#ifndef __DAMARIS_DATA_SPACE_H
#define __DAMARIS_DATA_SPACE_H

#include "damaris/util/Debug.hpp"
#include "damaris/util/Pointers.hpp"
//#include "damaris/util/TypeWrapper.hpp"

namespace damaris {
	
USING_POINTERS;

/**
 * The DataSpace class looks like a smart pointer that counts the
 * number of reference to a region of the memory, except that when
 * the count goes down to 0 it will call the Delete method of an 
 * "owner" object that has been passed to the DataSpace when
 * constructing it.
 * 
 * Just like most smart pointers, private operators "new" and "delete" 
 * of this class prevents it from being allocated on the heap (RAII).
 *
 * The main reason this class exists is to implement DataSpace<Buffer>
 * and for a Buffer to be responsible for its DataSpaces, which means
 * being able to track if the user has kept somewhere an instance of
 * its DataSpace in order not to delete it.
 *
 * The OWNER class of a DataSpace is expected to provide a Deallocate()
 * function taking a void* pointer to the memory to be deleted.
 * If the Deallocate() function takes something else such as a typed pointer,
 * the type can be provided as second template parameter of the DataSpace,
 * e.g. DataSpace<OwnerClass,DataType>.
 * 
 * The DataSpace class also contains a GetSize() function which gives the
 * size in bytes of the allocated memory. This size is only informative
 * and is never actually used by the owner.
 *
 * The DataSpace class only has a weak pointer to its owner, and will
 * check if it has expired before trying to use its Deallocate() function.
 * If the pointer has expired, then it will output a MEMORY LEAK trace.
 */
template<class OWNER, typename T = void>
class DataSpace
{
//	friend class TypeWrapper<OWNER>::type;

	private:

	weak_ptr<OWNER> owner_; 	/*!< owner object */
	T* address_;			/*!< address of the data */
	size_t size_;			/*!< size of the memory region */
	shared_ptr<int> ref_count_;	/*!< count of references */
	shared_ptr<bool> data_owner_;	/*!< true if Deallocate should be called
						when ref_count_ == 0 */

	void * operator new   (size_t) { return NULL; }
	void * operator new[] (size_t) { return NULL; }
	void operator delete   (void*) {}
	void operator delete[] (void*) {}
		
	
	public:

	/**
	 * Default constructor. Builds a non-initialized DataSpace.
	 * Post-condition: ds.IsValid() returns false.
	 */
	DataSpace()
	{
		address_ = NULL;
		size_ = 0;
		ref_count_ = shared_ptr<int>(new int(1));
		data_owner_ = shared_ptr<bool>(new bool(false));
	}

	/**
	 * Builds a DataSpace from an owner, an address and an optional size.
	 * Post-condition: ds.IsValid() <=> the address passed is not-null.
	 *
	 * \param[in] owner : owner of the DataSpace.
	 * \param[in] addr : address of the actual data.
	 * \param[in] size : size (in bytes) of the data.
	 */
	DataSpace(const shared_ptr<OWNER>& owner, void* addr, size_t size) 
	: owner_(owner), address_(addr), 
	  size_(size), ref_count_(new int(0)), data_owner_(new bool(true)) {
		if(addr != 0) {
			*ref_count_ = 1;
		} else {
			*ref_count_ = 0;
		}
	}

	/**
	 * Copy-constructor, will update the reference counter and link the
	 * new DataSpace to the same data than the copied one.
	 * If the initial DataSpace is valid, the new one will also be.
	 * If the initial DataSpace is not valid, the new one will not be valid.
	 */
	DataSpace(const DataSpace<OWNER>& ds)
	{
		address_ = NULL;
		size_ = 0;
		Swap(ds);
	}
	
	/**
	 * Swap (affectation). If the passed DataSpace instance is not linked
	 * to any data, this operation will unlink this DataSpace.
	 * 
	 * \param[in] ds : DataSpace to copy.
	 */
	void Swap(const DataSpace<OWNER>& ds) 
	{
		if(ds.IsLinked()) {
			owner_ = ds.owner_;
			address_ = ds.address_;
			size_ = ds.size_;
			ref_count_ = ds.ref_count_;
			data_owner_ = ds.data_owner_;
			(*ref_count_) += 1;
		} else {
			if(IsLinked()) {
				Unlink();
			}
		}
	}
	
	/**
	 * Affectation operator.
	 */
	DataSpace<OWNER>& operator=(const DataSpace<OWNER>& ds)
	{
		Swap(ds);
		return *this;
	}

	/**
	 * Destructor. Calls the Deallocate() function of the owner if this
	 * DataSpace was linked and is the last reference.
	 * If the owner of the DataSpace does not exist anymore, and
	 * __LEAK flag is enabled, then a message is output on stderr to
	 * inform of the memory leak.
	 */
	virtual ~DataSpace() 
	{
		Unlink();
	}
	
	/**
	 * This function resets the parameters of this DataSpace instance,
	 * linking it to an owner and a memory region.
	 * If this DataSpace was previously uninitialized (IsValid() == false)
	 * then this function will make it valid, provided that the passed 
	 * address is not NULL. If this DataSpace was already linked, then
	 * a call to this function is equivalent to destroying an re-creating
	 * the DataSpace (the Deallocate function from the owner will be called).
	 *
	 * \param[in] owner : new owner of the DataSpace.
	 * \param[in] addr : address of the data.
	 * \param[in] size : size (in bytes) of the data.
	 */
	virtual bool Link(const shared_ptr<OWNER>& owner, 
			void* addr, size_t size = 0)
	{
		Unlink();
		owner_ = owner;
		address_ = addr;
		size_ = size;
		ref_count_.reset();
		ref_count_ = shared_ptr<int>(new int(1));
		data_owner_.reset();
		data_owner_ = shared_ptr<bool>(new bool(true));
		return true;
	}

	/**
	 * Returns a raw pointer to the data.
	 * Will return NULL if the DataSpace is not linked.
	 */
	virtual T* GetData() const {
		return address_;
	}

	/**
	 * Returns the size of the memory region handled by this DataSpace.
	 */
	virtual size_t GetSize() const {
		return size_;
	}

	/**
	 * Returns the number of references to this memory regions.
	 * Returns 0 if the DataSpace is not linked.
	 */
	virtual int RefCount() const {
		if(IsLinked())
			return (*ref_count_);
		else
			return 0;
	}

	/**
	 * Returns a pointer to the owner of this DataSpace.
	 */
	virtual shared_ptr<OWNER> GetOwner() const {
		if(owner_.expired()) return shared_ptr<OWNER>();
		return owner_.lock();
	}

	/**
	 * Returns true if the DataSpace has been linked to a memory region
	 * and a owner.
	 */
	virtual bool IsLinked() const {
		return (address_ != (T*)NULL);
	}
	
	/**
	 * Unlinks the DataSpace from its owner, possibly triggering memory
	 * release if the count of reference goes down to 0.
	 * Returns true if the DataSpace has been properly unlinked, false
	 * if the DataSpace was not linked in the first place or if the owner
	 * has disappeared.
	 */
	virtual bool Unlink() {
		size_ = 0;
		if(IsLinked()) {
			(*ref_count_)--;
			if((address_ != 0) && ((*ref_count_) == 0)) {
				if(not owner_.expired()) {
					if(*data_owner_) {
					owner_.lock()->Deallocate(address_);
					}
					
					address_ = NULL;
				} else {
					LEAK("Owner expired before DataSpace,"
					<< " probable memory leak");
					address_ = NULL;
					return false;
				}
			}
			return true;
		} else {
			return false;
		}
	}

	/**
	 * Returns true if the DataSpace (or any copy of it) should
	 * free the memory when all copies are deleted.
	 */
	virtual bool GetDataOwnership() const {
		return *data_owner_;
	}

	/**
	 * This DataSpace and all copies made of it lose the responsibility
	 * for freeing the memory when all copies have disappeared.
	 */
	virtual void LoseDataOwnership() {
		*data_owner_ = false;
	}
	
	/**
	 * This DataSpace and all copies made of it gain the responsibility
	 * for freeing the memory when all copies have disappeared. 
	 */
	virtual void GainDataOwnership() {
		*data_owner_ = true;
	}
};

}

#endif
