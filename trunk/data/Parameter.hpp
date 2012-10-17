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
 * \file Parameter.hpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_PARAMETER_H
#define __DAMARIS_PARAMETER_H

#include <boost/any.hpp>
#include <string>

#include "xml/Model.hpp"
#include "data/Types.hpp"
#include "core/Observable.hpp"
#include "core/Configurable.hpp"
#include "core/Manager.hpp"
#include "core/Debug.hpp"

namespace Damaris {
	/**
	 * The Parameter class is based on boost::any to hold
	 * any type of data and associate it with a name.
	 */	
	class Parameter : public Configurable<Model::Parameter>, public Observable {

		friend class Manager<Parameter>;

		private:
			std::string name; /*!< Name of the parameter. */
			int id; /*!< id of the parameter, provided by the ParameterManager when created. */

			void* value; /*!< Pointer to the memory where the actual value is stored. */

			/**
			 * Constructor. This constructor is private, use New to create an instance.
			 */
			template<typename T>
			Parameter(const Model::Parameter& mdl, const std::string& name, const T& v);

		public:

			/**
			 * Creates a new Parameter object according to a provided model and a name.
			 */
			static Parameter* New(const Model::Parameter& mdl, const std::string& name);

			/**
			 * Gets the name of the parameter.
			 */
			const std::string& GetName() const
			{ return name; }
			
			/**
			 * Returns the id of the parameter.
			 */
			int GetID() const
			{ return id; }

			/**
			 * Gets the value of the parameter.
			 */
			template<typename T>
			T GetValue() const;

			/**
			 * Sets the value of the parameter.
			 */
			template<typename T>
			void SetValue(const T& v);

			/**
			 * Copy the value to a buffer of given maximum size.
			 */
			int ToBuffer(void* buffer, unsigned int size) const;
		
			/**
		 	 * Get the value from a buffer.
			 */
			int FromBuffer(const void* buffer, unsigned int size);

			/**
			 * Destructor.
			 */
			virtual ~Parameter();
	};
}

#endif

namespace Damaris {

template<typename T>
Parameter::Parameter(const Model::Parameter& mdl, const std::string& n, const T& v)
: Configurable<Model::Parameter>(mdl)
{
	value = NULL;
	name = n;
	SetValue<T>(v);
}

template<typename T>
T Parameter::GetValue() const 
{
	T t;
	ToBuffer((void*)(&t), sizeof(T));
	return t;
}

template<typename T>
void Parameter::SetValue(const T& v)
{
	FromBuffer(&v,sizeof(T));
}
}
