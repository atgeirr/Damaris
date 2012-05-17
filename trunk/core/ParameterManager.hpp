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
 * \file ParameterSet.hpp
 * \date February 2012 
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_PARAMETER_SET_H
#define __DAMARIS_PARAMETER_SET_H

#include <map>
#include "xml/Model.hpp"
#include "core/Debug.hpp"
#include "core/Configurable.hpp"
#include "data/Parameter.hpp"

namespace Damaris {

/**
 * The ParameterSet class describes an object that holds
 * the parameters and offers an access by name.
 */
class ParameterManager : public Manager<Parameter,Model::Parameter> {
	private:
		ParameterManager();

	public:
		/**
		 * Constructor.
		 * \param[in] mdl : base model to initialize from.
		 */
		static void Init(const Model::Data& mdl);
	
		template<typename T>
		class ParameterMap {
			public:
			/**
			 * Access parameters by name, only int parameters are considered.
			 * This operator is here to help accing the int parameters from a Calc object.
			 */
			T operator[](const std::string& n)
			{
				Parameter* p = ParameterManager::Search(n);
				if(p == NULL) return T();
				return p->getValue<T>();
			}
		};
};

}
#endif
