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

std::shared_ptr< BoxLayout::CalcType > BoxLayout::_calc_;

bool BoxLayout::ObserveDependentParameters()
{
	// start parsing the dimensions, ghosts, and global strings to find the names
	// of the parameters. Every time we find one, search in the
	// ParameterManager and connect to it.
	std::string dims = GetModel().dimensions()+" "+GetModel().global()+" "+GetModel().ghosts();
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
				std::shared_ptr<Parameter> p 
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
		_calc_ = std::shared_ptr<BoxLayout::CalcType>
				(new BoxLayout::CalcType(
					ParameterManager::ParameterMap<int>()));
	}

	{ // interpret the dimensions
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

	if(GetModel().global() == "#") {
		global_extents_ = extents_;
	} else { // interpret the global dimensions
		std::vector<int> e;
		std::string str = (std::string)(GetModel().global());
		std::string::const_iterator iter = str.begin();
		std::string::const_iterator end = str.end();
		bool r = boost::spirit::qi::phrase_parse(iter, end, *_calc_,
				boost::spirit::ascii::space, e);
		if((!r) || (iter != str.end())) {
			ERROR("While parsing global dimension descriptor for layout \""
					<< GetModel().name() << "\"");
		}

		if((GetModel().language() == model::Language::fortran)
				|| ((GetModel().language() == model::Language::unknown)
					&& Environment::GetDefaultLanguage() 
					== model::Language::fortran)) {
			std::vector<int> rdims(e.rbegin(),e.rend());
			global_extents_ = rdims;
		} else {
			global_extents_ = e;
		}
	}
	if(extents_.size() != global_extents_.size()) {
		ERROR("While parsing global descriptor for layour \""
		<< GetModel().name() << "\", number of dimensions does not match");
	}

	if(GetModel().ghosts() == "#") {
		ghosts_.clear();
		for(unsigned int i=0; i<extents_.size(); i++) {
			ghosts_.push_back(
				std::make_pair<size_t,size_t>(0,0));
		}
	} else { // interpret the ghost zones
		std::vector<int> e;
		std::string str = (std::string)(GetModel().ghosts());
		// replace the ':' by ',' so that it can be parsed
		std::replace(str.begin(), str.end(), ':', ',');
		std::string::const_iterator iter = str.begin();
		std::string::const_iterator end = str.end();
		bool r = boost::spirit::qi::phrase_parse(iter, end, *_calc_,
				boost::spirit::ascii::space, e);
		if((!r) || (iter != str.end())) {
			ERROR("While parsing ghost descriptor for layout \""
					<< GetModel().name() << "\"");
		}
		ghosts_.clear();
		str = (std::string)(GetModel().ghosts());
		// fill in the ghost array
		int i=0; // current ghost
		int f=0; // first or second member of ghost
		for(unsigned int j=0; j < str.size(); j++) {
			if(str[j] == ':') {
				if(f == 1) {
					ERROR("While parsing ghost descriptor for layout \""
						<< GetModel().name() << "\"");
					break;
				}
				f = 1;
				size_t g = e[i];
				i += 1;
				ghosts_.push_back(std::make_pair(g,(size_t)0));
			} else if((str[j] == ',') || (j == str.size()-1)) {
				size_t g = e[i];
				i += 1;
				if(f == 0) { // only one ghost provided
					ghosts_.push_back(std::make_pair(g,g));
				} else {
					ghosts_.rbegin()->second = g;
				}
				f = 0;
			}
		}

		if((GetModel().language() == model::Language::fortran)
				|| ((GetModel().language() == model::Language::unknown)
					&& Environment::GetDefaultLanguage() 
					== model::Language::fortran)) {
			std::vector<std::pair<size_t,size_t> > rg(ghosts_.rbegin(),ghosts_.rend());
			ghosts_ = rg;
		}
		if(ghosts_.size() != extents_.size()) {
			ERROR("While ghost descriptor for layout \""
				<< GetModel().name() << "\" has an incorrect number of dimensions");
		}
	}
}

}
