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

#include <cstring>

#include "util/Debug.hpp"
#include "env/Environment.hpp"
#include "data/BoxLayout.hpp"

namespace damaris {

shared_ptr< BoxLayout::CalcType > BoxLayout::_calc_;

bool BoxLayout::ObserveDependentParameters()
{
	// start parsing the dimensions string to find the names
	// of the parameters. Every time we find one, search in the
	// ParameterManager and connect to it.
	std::string dims = GetModel().dimensions();
	std::vector<char> buffer(dims.size()+1);
	buffer[0] = '\0';
	int j = 0;
	bool reading = false;
	for(unsigned int i = 0; i < dims.size(); i++) {
		if((isalpha(dims[i]) || dims[i] == '_') && !reading) {
			reading = true;
			j = 0;
		}
		if(reading) {
			if(isalnum(dims[i]) || (dims[i] == '_')) {
				buffer[j] = dims[i];
				j++;
			}
			if((not (isalnum(dims[i]) || (dims[i] == '_'))) 
				|| (i == dims.size() - 1)){
				reading = false;
				buffer[j] = '\0';
				std::string param(&(buffer[0]));
				shared_ptr<Parameter> p 
					= ParameterManager::Search(param);
				if(p) {
					p->AddObserver(
						SHARED_FROM_THIS());
				} else {
					CFGERROR("Unknown parameter " << param);
					return false;
				}
			}
		}
	}
	return true;
}

void BoxLayout::InterpretDimensions()
{
	if(not _calc_) {
		_calc_ = shared_ptr<BoxLayout::CalcType>
				(new BoxLayout::CalcType(
					ParameterManager::ParameterMap<int>()));
	}

	std::vector<int> e;
	std::string str = (std::string)(GetModel().dimensions());
	std::string::const_iterator iter = str.begin();
	std::string::const_iterator end = str.end();
	bool r = boost::spirit::qi::phrase_parse(iter, end, *_calc_,
			boost::spirit::ascii::space, e);
	if((!r) || (iter != str.end())) {
		ERROR("While parsing dimension descriptor for layout \""
				<< GetModel().name() << "\"");
	}

	if((GetModel().language() == model::Language::fortran)
		|| ((GetModel().language() == model::Language::unknown)
			&& Environment::GetDefaultLanguage() 
				== model::Language::fortran)) {
		std::vector<int> rdims(e.rbegin(),e.rend());
		extents_ = rdims;
	} else {
		extents_ = e;
	}
}

}
