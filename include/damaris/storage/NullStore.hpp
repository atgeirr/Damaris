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
#ifndef __DAMARIS_NULL_STORE_H
#define __DAMARIS_NULL_STORE_H

#include <stdint.h>
#include <string>

#include "storage/Store.hpp"

namespace damaris {

/**
 * NullStore is a Store that does not write anything.
 */
class NullStore : public Store {

	friend class Deleter<NullStore>;	
	/**
	 * Constructor. 
	 */
	NullStore(const model::Store& mdl);
			
	/**
	 * Destructor.
	 */
	virtual ~NullStore() {}
		
	public:
	
	/**
	 * This function is called after an iteration completed
	 * and is meant to output all variables into files.
	 */
	virtual void Output(int32_t iteration);

	template<typename SUPER>
	static std::shared_ptr<SUPER> New(const model::Store& mdl, const std::string& name) {
		return std::shared_ptr<SUPER>(new NullStore(mdl), Deleter<NullStore>());
	}
};

}

#endif
