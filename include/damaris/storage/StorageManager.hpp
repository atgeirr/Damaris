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
#ifndef __DAMARIS_STORAGE_MANAGER_H
#define __DAMARIS_STORAGE_MANAGER_H

#include "damaris/model/Model.hpp"
#include "damaris/util/Manager.hpp"
#include "damaris/util/ForwardDcl.hpp"
#include "damaris/storage/Store.hpp"

namespace damaris {

/**
 * StoreManager is a class that offers an access by name and by id to all 
 * Store objects managed by the program. It inherites from Manager<Store>.
 */
class StorageManager : public Manager<Store> {

public:
	typedef Manager<Store>::iterator iterator;
	/**
	 * Initialize the StorageManager with a model: goes through all the 
	 * stores described in the XML file, and creates the appropriate
	 * Store instances.
	 */
	static void Init(const model::Storage& mdl);

	/**
	 * This function is called at the end of every iteration to store
	 * variables into the right Store object.
	 */
	static void Update(int32_t iteration);
};

}

#endif
