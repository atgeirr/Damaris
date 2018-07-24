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

#ifndef __DAMARIS_PARAMETER_H
#define __DAMARIS_PARAMETER_H

#include <boost/any.hpp>
#include <string>

#include "Damaris.h"

#include "damaris/util/Unused.hpp"
#include "damaris/util/Observable.hpp"
#include "damaris/util/Configurable.hpp"
#include "damaris/util/Manager.hpp"
#include "damaris/util/Deleter.hpp"
#include "damaris/util/Debug.hpp"

#include "damaris/model/Model.hpp"

#include "damaris/data/Type.hpp"


namespace damaris {

	
/**
 * The Parameter class  holds any type of data and associate it with a name.
 */	
class Parameter : public Configurable<model::Parameter>, 
		  public Observable,
		  public ENABLE_SHARED_FROM_THIS(Parameter) {
	
	friend class Manager<Parameter>;
	friend class Deleter<Parameter>;

	private:
	std::string name_; /*!< Name of the parameter. */
	int id_; /*!< id of the parameter, provided by the 
			ParameterManager when created. */
	void* value_; /*!< Pointer to the memory where 
			the actual value is stored. */

	/**
	 * This constructor is private, use New to create an instance.
	 * 
	 * \param[in] mdl : model from which to instanciate the parameter.
	 * \param[in] v : initial value of the parameter.
	 */
	template<typename T>
	Parameter(const model::Parameter& mdl, const T& v);

	/**
	 * Destructor.
	 */
	virtual ~Parameter();
	
	public:

	/**
	 * Creates a new Parameter object according to a provided model.
	 *
	 * \param[in] mdl : model from which to initialize the parameter.
	 */
	template <typename SUPER>
	static std::shared_ptr<SUPER> New(const model::Parameter& mdl,
					const std::string& name);

	/**
	 * Gets the name of the parameter.
	 */
	const std::string& GetName() const {
		return name_; 
	}
	
	/**
	 * Returns the id of the parameter.
	 */
	int GetID() const { 
		return id_; 
	}

	/**
	 * Gets the value of the parameter.
	 */
	template<typename T>
	T GetValue() const;

	/**
	 * Get the type of the parameter.
	 */
	const model::Type& GetType() const {
		return GetModel().type();
	}

	/**
	 * Sets the value of the parameter.
	 * 
	 * \param[in] v : new value of the parameter.
	 */
	template<typename T>
	void SetValue(const T& v);

	/**
	 * Copy the value to a buffer of given maximum size. This function can
	 * be unsafe as it doesn't check for the type of the input buffer.
	 * Use GetValue whenever possible.
	 *
	 * \param[out]  buffer : buffer into which to store the value.
	 * \param[in] size : maximum size (in bytes) of the buffer.
	 */
	int ToBuffer(void* buffer, size_t size) const;
		
	/**
	 * Get the value from a buffer. This function can be unsafe as it
	 * doesn't check for the type of the input buffer. Use SetValue whenever
	 * possible.
	 *
	 * \param[in] buffer : buffer from which to get the value.
	 * \param[in] size : size of the buffer.
	 */
	int FromBuffer(const void* buffer, size_t size);
};

template<typename T>
Parameter::Parameter(const model::Parameter& mdl, const T& v)
: Configurable<model::Parameter>(mdl)
{
	value_ = NULL;
	name_ = mdl.name();
	SetValue<T>(v);
}

template<typename T>
T Parameter::GetValue() const 
{
	T t;
	model::Type requested_type = TypeFromCpp<T>();
	model::Type this_type = GetType();
	if(this_type == model::Type::integer) this_type = model::Type::int_;
	if(this_type == model::Type::real) this_type = model::Type::float_;

	if(requested_type != this_type) {
		ERROR("Requesting access to parameter's value with"
		<< " a wrong type (parameter type is " << GetType()
		<< ", requested " << TypeFromCpp<T>() << ")");
		return t;
	}
	
	ToBuffer((void*)(&t), sizeof(T));
	return t;
}

template<typename T>
void Parameter::SetValue(const T& v)
{
	model::Type requested_type = TypeFromCpp<T>();
	model::Type this_type = GetType();
	if(this_type == model::Type::integer) this_type = model::Type::int_;
  if(this_type == model::Type::real) this_type = model::Type::float_;

	if(requested_type != this_type) {
		ERROR("Requesting access to parameter's value with"
		<< " a wrong type (parameter type is " << GetType()
		<< ", requested " << TypeFromCpp<T>() << ")");
		return;
	}
	
	FromBuffer(&v,sizeof(T));
}

template<typename SUPER>
std::shared_ptr<SUPER> Parameter::New(const model::Parameter& mdl, 
				const std::string& UNUSED(name))
{
	try {

	switch(mdl.type()) {
		case model::Type::short_:
			return std::shared_ptr<SUPER>(
				new Parameter(mdl,
				boost::lexical_cast<short>(mdl.value())),
				Deleter<Parameter>());
		case model::Type::int_:
			return std::shared_ptr<SUPER>(
				new Parameter(mdl,
				boost::lexical_cast<int>(mdl.value())),
				Deleter<Parameter>());
		case model::Type::integer:
			return std::shared_ptr<SUPER>(
				new Parameter(mdl,
				boost::lexical_cast<int>(mdl.value())),
				Deleter<Parameter>());
		case model::Type::long_:
			return std::shared_ptr<SUPER>(
				new Parameter(mdl,
				boost::lexical_cast<long>(mdl.value())),
				Deleter<Parameter>());
		case model::Type::float_:
			return std::shared_ptr<SUPER>(
				new Parameter(mdl,
				boost::lexical_cast<float>(mdl.value())),
				Deleter<Parameter>());
		case model::Type::real :
			return std::shared_ptr<SUPER>(
				new Parameter(mdl,
				boost::lexical_cast<float>(mdl.value())),
				Deleter<Parameter>());
		case model::Type::double_:
			return std::shared_ptr<SUPER>(
				new Parameter(mdl,
				boost::lexical_cast<double>(mdl.value())),
				Deleter<Parameter>());
		case model::Type::char_:
			return std::shared_ptr<SUPER>(
				new Parameter(mdl,
				boost::lexical_cast<char>(mdl.value())),
				Deleter<Parameter>());
		case model::Type::character:
			return std::shared_ptr<SUPER>(
				new Parameter(mdl,
				boost::lexical_cast<char>(mdl.value())),
				Deleter<Parameter>());
		default:
			ERROR("Type \"" << mdl.type()
				<< "\" for parameter \""<< mdl.name() 
				<< " is not allowed\"");
	}

	} catch(boost::bad_lexical_cast &) {
		ERROR("Unable to interpret parameter's value \"" 
			<< mdl.value() << "\" " 
			<< "for parameter \"" << mdl.name() << "\"");
	}
	return std::shared_ptr<SUPER>();
}

}

#endif
