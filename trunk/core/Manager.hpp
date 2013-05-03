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
 * \file Manager.hpp
 * \date May 2012
 * \author Matthieu Dorier
 * \version 0.5
 */
#ifndef __DAMARIS_MANAGER_H
#define __DAMARIS_MANAGER_H

#include <utility>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/lambda/lambda.hpp>
#include "core/Debug.hpp"
#include "core/Tags.hpp"

namespace Damaris {

namespace bmi = boost::multi_index;

class Identified;
/**
 * This class serves as a factory to build identified (named)
 * objects. It keeps a record of all objects created and does
 * not create twice the same named object.
 * Each object must have a unique name, and the Manager also gives
 * them a unique integer id.
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
					boost::shared_ptr<T>,
					bmi::indexed_by<
						bmi::ordered_unique<
							bmi::tag<by_id>,
							bmi::const_mem_fun<T,int,
								&T::GetID
							> 
						>,
						bmi::ordered_unique<
							bmi::tag<by_name>,
							bmi::const_mem_fun<T,const std::string&,
								&T::GetName
							> 
						>,
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

		static ObjectSet *objects; /*!< pointer to an ObjectSet that will store all the objects.
									This pointer is initialized only if some objects have to be
									stored, otherwise we save memory by deleting it.*/

	private:
		/**
		 * Constructor is private: all the methods are static and we shouldn't be able
		 * to create on such an object.
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
		 * This function creates an object of type SUBCLASS 
		 * (which must be a subclass of the class T) by calling
		 * its "New" function with a MODEL parameter. The name of the object
		 * must be provided. The object is stored in the multiindex structure
		 * and a pointer to it is returned. If another object with the same name
		 * already exists, the object is not created but the old object is returned
		 * instead. A configuration warning is output on stderr.
		 * 
		 * Example: in a Manager<Action>, one can use
		 * Manager<Action>::Create<DynamicAction>(Model::Event mdl, "a name")
		 * since DynamicAction inherites from Action and has a constructor that
		 * takes a Model::Event instance.
		 */
		template<typename SUBCLASS, typename MODEL>
		static T* Create(const MODEL &mdl, const std::string& name)
		{
			if(objects == NULL) {
				objects = new ObjectSet();
			}

			T* t = static_cast<T*>(SUBCLASS::New(mdl,name));
			if(t == NULL) return NULL;

			t->id = NumObjects();

			std::pair<typename ObjectSet::iterator,bool> ret
				= objects->insert(boost::shared_ptr<T>(t));
			if(ret.second == false) {
				CFGWARN("Duplicate element \""<< name << "\" not created, " 
					<< "returning previous value instead.");
				// no need to delete t here, the shared_ptr does it for us
				return ret.first->get();
			}
			return t;
		}

		/**
		 * Adds in the manager an object that has already been created externally.
		 * The object is not added if another object already exists with the same name.
		 * Adding into the manager will alter the object's id.
		 */
		template<typename SUBCLASS>
		static bool Add(SUBCLASS *t)
		{
			if(t == NULL) return false;

			if(objects == NULL) {
				objects = new ObjectSet();
			}

			std::pair<typename ObjectSet::iterator,bool> ret
				= objects->insert(boost::shared_ptr<T>(t));
			if(ret.second) t->id = NumObjects()-1;

			return ret.second;
		}

		/**
		 * Deletes an object from the structure.
		 */
		static void Delete(T* t)
		{
			if(t != NULL && objects != NULL)
				objects->erase(t);
		}

		/**
		 * Deletes an object using an iterator.
		 */
		static iterator Delete(iterator& it)
		{
			if(objects != NULL)
				return objects->erase(it);
		}

		/**
		 * Searches an object by its name, return a pointer to the object
		 * if it exists, NULL otherwise.
		 */
		static T* Search(const std::string &name) 
		{
			if(objects == NULL) return NULL;
			typename IndexByName::iterator it = objects->template get<by_name>().find(name);
			if(it == objects->template get<by_name>().end()) {
				return NULL;
			}
			return it->get();

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
		 * Searches an object by its id, return a pointer to the object if found,
		 * NULL otherwise.
		 */
		static T* Search(const int &id) 
		{
			if(objects == NULL) return NULL;
			typename IndexById::iterator it 
				= objects->template get<by_id>().find(id);
			if(it == objects->template get<by_id>().end()) {
				return NULL;
			}
			return it->get();
		}

		/**
		 * Returns the number of objects contained in the set.
		 */
		static int NumObjects()
		{
			if(objects == NULL)
				return 0;
			else
				return objects->size();
		}

		/**
		 * Remove all the objects from the set and deletes the set
		 * (can be reinitialized later by adding new objects).
		 */
		static void DeleteAll()
		{
			if(objects != NULL)
				delete objects;
		}

		/**
		 * Returns an iterator over the end of the index.
		 * The index must NOT be empty: the user has to check this condition
		 * using IsEmtpy() before trying to call this function.
		 */
		static iterator End() 
		{
			return objects->end();
		}

		/**
		 * Returns an iterator over the begining of the index.
		 * The index must NOT be empty: the use has to check this condition
		 * using IsEmpty() before trying to call this function.
		 */
		static iterator Begin()
		{
			return objects->begin();
		}

		/**
		 * Indicates is the index is empty or not.
		 */
		static bool IsEmpty()
		{
			return (NumObjects() == 0);
		} 
};

template <typename T>
typename Manager<T>::ObjectSet *Manager<T>::objects = NULL;

}

#endif
