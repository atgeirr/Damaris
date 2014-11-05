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

#ifndef __DAMARIS_OBSERVABLE_H
#define __DAMARIS_OBSERVABLE_H

#include <set>

#include "damaris/util/ForwardDcl.hpp"
#include "damaris/util/Pointers.hpp"
#include "damaris/util/Observer.hpp"

namespace damaris {

USING_POINTERS;

/**
 * This class is used by child classes that need to notify other objects that
 * their internal stat has changed.
 *
 * It uses normal pointers internal to avoid all the troubles of interfering
 * with a child class that uses ENABLE_SHARED_FROM_THIS; however it
 * imposes that AddObserver is called with a shared_ptr, to prove that the
 * pointer is well managed externally. Since removing observers/observable
 * is done in the destructor, there is no chance to leave a crazy pointer there.
 */
class Observable {
	friend class Observer;
	
	private:
		std::set<Observer*> observers_; 
		/*!< List of pointers to objects to notify. */
	
		void RemoveObserver(Observer* obs) {
			observers_.erase(obs);
		}

	protected:

		/**
		 * This function has to be called from any member function that 
		 * modifies the Observable.
		 */
		void Changed() const {
			std::set<Observer*>::iterator it 
				= observers_.begin();
			for(;it != observers_.end(); it++) {
				Observer* p = *it;
				p->Notify();
			}
		}
		
		/**
		 * Destructor. Will notify destruction to all Observer observing
		 * this object.
		 */
		~Observable() {
			std::set<Observer*>::iterator it 
				= observers_.begin();
			for(;it != observers_.end(); it++) {
				Observer* p = *it;
				p->NotifyDestruction(this);
			}
		}

	public:
		/**
		 * Adds an object to be notified of any changes in this 
		 * Observable instance.
		 * 
		 * \param[in] obs : observer to be added.
		 */
		template<typename T>
		void AddObserver(const shared_ptr<T>& obs) {
			if(obs) {
				obs->observed_.insert(this);
				observers_.insert(obs.get());
			}
		}

		/**
		 * Removes an object from the list of objects that observe this 
		 * Observable.
		 *
		 * \param[in] obs : observer to be removed.
		 */
		template<typename T>
		void RemoveObserver(const shared_ptr<T>& obs) {
			if(obs) {
				observers_.erase(obs.get());
			}
		}
};

} // namespace Damaris

#endif
