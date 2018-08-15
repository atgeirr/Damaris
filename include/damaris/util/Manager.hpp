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

#ifndef __DAMARIS_MANAGER_H
#define __DAMARIS_MANAGER_H

#include <utility>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/lambda/lambda.hpp>

#include "damaris/util/Pointers.hpp"
#include "damaris/util/ForwardDcl.hpp"
#include "damaris/util/Deleter.hpp"
#include "damaris/util/Debug.hpp"
#include "damaris/util/Tags.hpp"

namespace damaris {

namespace bmi = boost::multi_index;


/**
 * This class serves as a factory to build identified (named) objects. It keeps
 * a record of all objects created and does not create twice the same named 
 * object. Each object must have a unique name, and the Manager also gives them 
 * a unique integer id.
 */
template<typename T>
class Manager {
	
	private:

	/**
	 * ObjectSet is a boost MultiIndex to store all the pointers
	 * to objects. The objects are stored using boost::shared_ptr
	 * so as soon as they are removed from the set, they are destroyed
	 * (unless another shared_ptr exists somewhere else for these objects.
	 */
	typedef typename boost::multi_index_container<
		std::shared_ptr<T>,
		bmi::indexed_by<
            // index by id
			bmi::ordered_unique<
				bmi::tag<by_id>,
				bmi::const_mem_fun<T,int,
					&T::GetID
				> 
			>,
			// index by name
			bmi::ordered_unique<
				bmi::tag<by_name>,
				bmi::const_mem_fun<T,const std::string&,
					&T::GetName
				> 
			>,
			// index by both id and name
			bmi::ordered_unique<
				bmi::tag<by_any>,
					bmi::composite_key<
					T,
					bmi::const_mem_fun<
						T,
						const std::string&, &T::GetName
					>,
					bmi::const_mem_fun<T,
						int, &T::GetID
					>
				>
			>
		>
	> ObjectSet;

	typedef typename ObjectSet::template index<by_name>::type IndexByName;
	typedef typename ObjectSet::template index<by_id>::type IndexById;
	typedef typename ObjectSet::template index<by_any>::type Index;	

	static ObjectSet _objects_; /*!< Instance of ObjectSet to
					keep track of created objects */

	private:
	
	/**
	 * Constructor is private: all the methods are static and we 
	 * shouldn't be able to create on such an object.
	 */
	Manager() {}

	public:
		
	/**
	 * From outside the Manager object, an iterator type is defined to 
	 * iterator over elements of the set.
	 */
	typedef typename ObjectSet::iterator iterator;

	public:

	/**
	 * This function creates an object of type SUBCLASS (which must be a 
	 * subclass of the class T) by calling its "New" function with a MODEL 
	 * parameter. The object is stored in the multiindex structure and a 
	 * pointer to it is returned. If another object with the same name
	 * already exists, the object is not created but the old object is 
	 * returned instead. A configuration warning is output on stderr.
	 * 
	 * Example: in a Manager<Action>, one can use
	 * Manager<Action>::Create<DynamicAction>(Model::Event mdl)
	 * since DynamicAction inherites from Action and has a constructor that
	 * takes a Model::Event instance.
	 *
	 * \param[in] mdl : model from which to create the object.
	 * \param[in] name : name to give to the object (if it needs to be renamed).
	 */
	template<typename SUBCLASS, typename MODEL>
	static std::shared_ptr<T> Create(const MODEL &mdl, const std::string& name)
	{
		std::shared_ptr<T> t(
		SUBCLASS::template New<T>(mdl,name));
		if(not (bool)t) return t;

		t->id_ = GetNumObjects();
		//t->name_ = name;

		std::pair<typename ObjectSet::iterator,bool> ret
			= _objects_.insert(t);
		if(ret.second == false) {
			CFGERROR("Duplicate element \""<< name << 
				"\" not created, " 
				<< "returning previous value instead.");
			// no need to delete t here, the shared_ptr does it for us
			return *(ret.first);
		}
		
		return t;
	}
	
	/**
	 * Creates an object from a model.
	 *
	 * \param[in] mdl : model from which to create the object.
	 */
	template<typename SUBCLASS, typename MODEL>
	static std::shared_ptr<T> Create(const MODEL &mdl)
	{
		return Create<SUBCLASS,MODEL>(mdl,mdl.name());
	}

	/**
	 * Adds in the manager an object that has already been created 
	 * externally. The object is not added if another object already exists 
	 * with the same name. Adding into the manager will alter the object's 
	 * id.
	 *
	 * \param[in] t : object to be inserted.
	 */
	template<typename SUBCLASS>
	static bool Add(const std::shared_ptr<SUBCLASS>& t)
	{
		if(not (bool)t) return false;
		int32_t backup = t->id_;
		t->id_ = GetNumObjects();
		std::pair<typename ObjectSet::iterator,bool> ret
                = _objects_.insert(std::shared_ptr<T>(t));
		if(!ret.second) t->id_ = backup;

		return ret.second;
	}

	/**
	 * Deletes an object from the structure.
	 *
	 * \param[in] t : object to be deleted.
	 */
	static void Delete(const std::shared_ptr<T>& t)
	{
		if(t) {
			_objects_.template get<by_name>().erase(t->GetName());
		} 
	}

	/**
	 * Deletes an object using an iterator.
	 *
	 * \param[in] it : iterator pointing to the element to be deleted.
	 */
	static iterator Delete(iterator& it)
	{
		return _objects_.erase(it);
	}

	/**
	 * Searches an object by its name, return a pointer to the object
	 * if it exists, NULL otherwise.
	 * 
	 * \param[in] name : name of the object to find.
	 */
	static std::shared_ptr<T> Search(const std::string &name)
	{
		typename IndexByName::iterator it =
			_objects_.template get<by_name>().find(name);
		if(it == _objects_.template get<by_name>().end()) {
			return std::shared_ptr<T>();
		}
		return *it;
	}

	/**
	 * Applies a function to each objects of the set.
	 * This mimics functional languages and eases the design of
	 * procedures that have to be executed on all objects.
	 * The template FUNCTION must have the following arguments:
	 * (Manager<T>::iterator& it, const Manager<T>::iterator& end, T* obj)
	 * If the procedure only works on the state of obj, it can use
	 * the pointer (guaranted to be not-null). If the purpose
	 * of the iteration is to erase objects, the iterator
	 * provided as a first argument must be used and updated consistently.
	 * For exemple: 
	 * it = Manager<T>::Delete(it);
	 * No assumption can be made on the order the objects are visited.
	 *
	 * \param[in] f : function to apply to all objects.
	 */
	template<typename FUNCTION>
	static void ForEach(FUNCTION& f)
	{
		if(IsEmpty()) return;

		iterator it = Begin();
		for(;it != End(); it++) {
			f(it,End(),it->get());
		}
	}

	/**
	 * Searches an object by its id, return a pointer to the object if 
	 * found, a void shared_ptr otherwise.
	 * 
	 * \param[in] id : id of the object to search for.
	 */
	static std::shared_ptr<T> Search(const int &id)
	{
		typename IndexById::iterator it 
			= _objects_.template get<by_id>().find(id);
		if(it == _objects_.template get<by_id>().end()) {
			return std::shared_ptr<T>();
		}
		return *it;
	}

	/**
	 * Returns the number of objects contained in the set.
	 */
	static int GetNumObjects()
	{
		return _objects_.size();
	}

	/**
	 * Remove all the objects from the set and deletes the set
	 * (can be reinitialized later by adding new objects).
	 */
	static void DeleteAll()
	{
		_objects_.clear();
	}

	/**
	 * Returns an iterator over the end of the index.
	 * The index must NOT be empty: the user has to check this condition
	 * using IsEmtpy() before trying to call this function.
	 */
	static iterator End() 
	{
		return _objects_.end();
	}

	/**
	 * Returns an iterator over the begining of the index.
	 * The index must NOT be empty: the use has to check this condition
	 * using IsEmpty() before trying to call this function.
	 */
	static iterator Begin()
	{
		return _objects_.begin();
	}

	/**
	 * Indicates is the index is empty or not.
	 */
	static bool IsEmpty()
	{
		return (GetNumObjects() == 0);
	} 
};

template <typename T>
typename Manager<T>::ObjectSet Manager<T>::_objects_;

}

#endif
