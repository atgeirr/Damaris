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
#ifndef __DAMARIS_STORE_H
#define __DAMARIS_STORE_H

#include <stdint.h>
#include <string>

#include "Damaris.h"
#include "damaris/util/Pointers.hpp"
#include "damaris/util/ForwardDcl.hpp"
#include "damaris/util/Deleter.hpp"
#include "damaris/util/Configurable.hpp"
#include "damaris/data/Variable.hpp"
#include "damaris/model/Model.hpp"

namespace damaris {

/**
 * The Store object is an interface to a storage layer.
 * It is an abstract class, child classes must overwrite the 
 * Store::Start, Store::Add, and Store::Complete functions.
 */
class Store : public Configurable<model::Store> {
	
	friend class Manager<Store>;
	friend class Deleter<Store>;

	std::string name_; /*!< Name of the Store. */
	int id_; 	  /*!< ID given to the store when set in
				the StorageManager. */
	std::vector<weak_ptr<Variable> > vars_; /*!< List of variables managed by this Store. */

	protected:
		
	/**
	 * Constructor. 
	 */
	Store(const model::Store& mdl) 
	: Configurable<model::Store>(mdl), name_(mdl.name()), id_(-1) {}
			
	/**
	 * Destructor.
	 */
	virtual ~Store() {}
		
	public:
	
	/**
	 * Gets the ID of the action.
	 */		
	int GetID() const { return id_; }

	/**
	 * Gets the name of the action.
	 */
	const std::string& GetName() const { return name_; }	

	/**
	 * This function is called on all variables that require to be
	 * stored using this Store object.
	 */
	void AddVariable(const shared_ptr<Variable>& v) {
		vars_.push_back(v);
	}

	/**
	 * Returns a reference to the list of variables this store is in
	 * charge of.
	 */
	const std::vector< weak_ptr<Variable> >& GetVariables() const {
		return vars_;
	}

	/**
	 * This function is called after an iteration completed
	 * and is meant to output all variables into files.
	 */
	virtual void Output(int32_t iteration) = 0;

	/**
 	* This function checks to see if there is any data to be written into an store object or not.
 	* If there is no data, the Update function in each store will return. <Maybe set it as configuratble at future>
 	*/
	virtual bool IterationIsEmpty(int iteration){
		vector<weak_ptr<Variable> >::const_iterator w;
		w = GetVariables().begin();

		// for each variable ...
		for (; w != GetVariables().end(); w++) {
			shared_ptr<Variable> v = w->lock();

			if (v->CountTotalBlocks(iteration) > 0)
				return false;
		}

		return true;
	}
};

}

#endif
