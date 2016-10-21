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

#include <vector>

#include "util/Debug.hpp"
#include "data/Type.hpp"

namespace damaris {
	
static std::vector<size_t> _sizes_;
	
static void FillSizeVector()
{
	_sizes_.resize(model::Type::undefined);
	_sizes_[model::Type::short_] 	= 2;
	_sizes_[model::Type::int_] 	= 4;
	_sizes_[model::Type::integer]	= 4;
	_sizes_[model::Type::long_] 	= 8;
	_sizes_[model::Type::float_]	= 4;
	_sizes_[model::Type::real]	= 4;
	_sizes_[model::Type::double_]	= 8;
	_sizes_[model::Type::char_] 	= 1;
	_sizes_[model::Type::character]	= 1;
	_sizes_[model::Type::label]	= -1;
	_sizes_[model::Type::string]	= -1;
}

size_t TypeSize(const model::Type &t)
{
	if(_sizes_.size() == 0) FillSizeVector();
	if(t != model::Type::undefined)
		return _sizes_[(int)t];
	else {
		WARN("Querying for the size of an undefined type");
		return 0;
	}
}

template <>
model::Type TypeFromCpp<int>() {
	return model::Type::int_;
}

template <>
model::Type TypeFromCpp<long>() {
	return model::Type::long_;
}

template <>
model::Type TypeFromCpp<short>() {
	return model::Type::short_;
}
	
template <>
model::Type TypeFromCpp<double>() {
	return model::Type::double_;
}
	
template <>
model::Type TypeFromCpp<float>() {
	return model::Type::float_;
}
	
template <>
model::Type TypeFromCpp<char>() {
	return model::Type::char_;
}	
	
}
