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

#ifndef __DAMARIS_PARAMETER_MANAGER_H
#define __DAMARIS_PARAMETER_MANAGER_H

#include <map>

#include "damaris/util/Debug.hpp"
#include "damaris/util/Configurable.hpp"
#include "damaris/model/Model.hpp"
#include "damaris/data/Parameter.hpp"

namespace damaris {

/**
 * The ParameterManager class holds
 * the parameters and offers an access by name and by id.
 */
class ParameterManager : public Manager<Parameter> {

	public:
	/**
	 * Constructor. Goes through all the objects described in the XML model
	 * and creates them.
	 *
	 * \param[in] mdl : base model to initialize from.
	 */
	static void Init(const model::Data& mdl);
	
	/**
	 * Some parts of the program require to have an object that exposes
	 * the parameters, and cannot work with the static functions, so
	 * we provide a class that wraps the ParameterManager in an object.
	 */
	template<typename T>
	class ParameterMap {
		public:
		/**
		 * Access parameters by name.
		 * This operator is here to help accing the int parameters from 
		 * a Calc object.
		 */
		T operator[](const std::string& n)
		{
			shared_ptr<Parameter> p = ParameterManager::Search(n);
			if(not p) {
				ERROR("Parameter \"" << n << "\" not found");
				return T();
			}
			return p->GetValue<T>();
		}
	};
};

}
#endif
