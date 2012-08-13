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

//			boost::any value; /*!< Value of the parameter. */
			void* value;		
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
			const std::string& getName() const;
			
			/**
			 * Returns the id of the parameter.
			 */
			int getID() const;

			/**
			 * Gets the value of the parameter.
			 */
			template<typename T>
			T getValue() const;

			/**
			 * Sets the value of the parameter.
			 */
			template<typename T>
			void setValue(const T& v);

			/**
			 * Copy the value to a buffer of given maximum size.
			 */
			int toBuffer(void* buffer, unsigned int size) const;
		
			/**
		 	 * Get the value from a buffer.
			 */
			int fromBuffer(const void* buffer, unsigned int size);

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
	setValue<T>(v);
	//value = boost::any(v);
}

template<typename T>
T Parameter::getValue() const 
{
	/*
	try {
		return boost::any_cast<T>(value);
	} catch(boost::bad_any_cast &e) {
	*/
/*	try {
		T* t = (T*)value;
		return *t;
	} catch(std::bad_cast &e) {
		ERROR("Bad type cast in Parameter::getValue(),"
			<< " returning the default value for the type");
		return T();
	}
*/
	T t;
	toBuffer((void*)(&t), sizeof(T));
	return t;
}

template<typename T>
void Parameter::setValue(const T& v)
{
	fromBuffer(&v,sizeof(T));
}
}
