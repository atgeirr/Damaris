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
 * This class serves as a factory the build identified (named)
 * objects. It keeps a record of all objects created and does
 * not create twice the same named object.
 */
template<typename T>
class Manager {
	
	private:

		typedef typename boost::multi_index_container<
					boost::shared_ptr<T>,
					bmi::indexed_by<
						bmi::ordered_unique<
							bmi::tag<by_id>,
							bmi::const_mem_fun<T,int,
								&T::getID
							> 
						>,
						bmi::ordered_unique<
							bmi::tag<by_name>,
							bmi::const_mem_fun<T,const std::string&,
								&T::getName
							> 
						>,
						bmi::ordered_unique<
							bmi::tag<by_any>,
							bmi::composite_key<
								T,
								bmi::const_mem_fun<
									T,
									const std::string&, &T::getName
								>,
								bmi::const_mem_fun<T,
									int, &T::getID
								>
							>
						>
					>
				> ObjectSet;

		typedef typename ObjectSet::template index<by_name>::type IndexByName;
		typedef typename ObjectSet::template index<by_id>::type IndexById;
		typedef typename ObjectSet::template index<by_any>::type Index;	

		static ObjectSet *objects;

	private:
		Manager() {}

	public:	

		typedef typename ObjectSet::iterator iterator;

	private:
		static iterator empty;
		
	public:

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

		static void Delete(T* t)
		{
			if(t != NULL && objects != NULL)
				objects->erase(t);
		}

		static T* Search(const std::string &name) 
		{
			if(objects == NULL) return NULL;
			typename IndexByName::iterator it = objects->template get<by_name>().find(name);
			if(it == objects->template get<by_name>().end()) {
				return NULL;
			}
			return it->get();

		}

		template<typename FUNCTION>
		static void ForEach(const FUNCTION& f)
		{
			if(IsEmpty()) return;

			iterator it = Begin();
			for(;it != End(); it++) {
				f(*(it->get()));
			}
		}


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

		static int NumObjects()
		{
			if(objects == NULL)
				return 0;
			else
				return objects->size();
		}

		static void DeleteAll()
		{
			if(objects != NULL)
				delete objects;
		}

		static iterator End() 
		{
			return objects->end();
		}

		static iterator Begin()
		{
			return objects->begin();
		}

		static bool IsEmpty()
		{
			if(objects == NULL) return true;
			else return (Begin() == End());
		} 
};

template <typename T>
typename Manager<T>::ObjectSet *Manager<T>::objects = NULL;

}

#endif
