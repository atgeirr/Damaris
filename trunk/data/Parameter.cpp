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
 * \file Parameter.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#include "data/Parameter.hpp"

namespace Damaris {

const std::string& Parameter::getName() const
{
	return name;
}

int Parameter::getID() const
{
	return id;
}

Parameter* Parameter::New(const Model::Parameter& mdl, const std::string& name)
{
	try {

	switch(mdl.type()) {
		case Model::Type::short_:
			return new Parameter(mdl,name,boost::lexical_cast<short>(mdl.value()));
		case Model::Type::int_:
			return new Parameter(mdl,name,boost::lexical_cast<int>(mdl.value()));
		case Model::Type::integer:
			return new Parameter(mdl,name,boost::lexical_cast<int>(mdl.value()));
		case Model::Type::long_:
			return new Parameter(mdl,name,boost::lexical_cast<long>(mdl.value()));
		case Model::Type::float_:
			return new Parameter(mdl,name,boost::lexical_cast<float>(mdl.value()));
		case Model::Type::real :
			return new Parameter(mdl,name,boost::lexical_cast<float>(mdl.value()));
		case Model::Type::double_:
			return new Parameter(mdl,name,boost::lexical_cast<double>(mdl.value()));
		case Model::Type::char_:
			return new Parameter(mdl,name,boost::lexical_cast<char>(mdl.value()));
		case Model::Type::character:
			return new Parameter(mdl,name,boost::lexical_cast<char>(mdl.value()));
		case Model::Type::string:
			return new Parameter(mdl,name,boost::lexical_cast<std::string>(mdl.value()));
		default:
			ERROR("Undefined type \"" << mdl.type()
				<< "\" for parameter \""<< mdl.name() << "\"");
	}

	} catch(boost::bad_lexical_cast &) {
		ERROR("Unable to interpret parameter's value \"" << mdl.value() << "\" "
				<< "for parameter \"" << name << "\"");
		return NULL;
	}
	return NULL;
}

int Parameter::toBuffer(void* b, unsigned int maxsize) const
{
//	b = boost::any_cast(&value);
        size_t s1 = Types::basicTypeSize(model.type());
        size_t s = std::min(s1,maxsize);
	memcpy(b,value,s);
	return s;
}
}
