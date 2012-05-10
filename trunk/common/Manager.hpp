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

#include "common/Debug.hpp"
#include "common/Tags.hpp"

namespace Damaris {

namespace bmi = boost::multi_index;

class Identified
{
	virtual const std::string& getName() const = 0;

	virtual int getID() const = 0;
};

template<typename T, typename M>
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
									const std::string&, &T::getName>,
									bmi::const_mem_fun<T,int, &T::getID>
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

		static T* New(const M &mdl)
		{
			if(objects == NULL) {
				objects = new ObjectSet();
			}

			T* t = new T(mdl);
			std::pair<typename ObjectSet::iterator,bool> ret
				= objects->insert(boost::shared_ptr<T>(t));
			if(ret.second == false) {
				WARN("Duplicate element \""<< t->getName() << "\" not created, " 
					<< "returning previous value instead.");
				
				delete t;
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
			typename IndexByName::iterator it 
				= objects->get().find(name);
			if(it == objects->get().end()) {
				return NULL;
			}
			return it->get();

		}


		static T* Search(const int &id) 
		{
			typename IndexById::iterator it 
				= objects->get().find(id);
			if(it == objects->get().end()) {
				return NULL;
			}
			return it->get();

		return NULL;
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
};

template <typename T, typename M>
typename Manager<T,M>::ObjectSet *Manager<T,M>::objects = NULL;

}

#endif
