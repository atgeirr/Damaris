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
 * \file Layout.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */

#include <cstring>

#include "core/Debug.hpp"
#include "core/Environment.hpp"
#include "data/Types.hpp"
#include "data/Layout.hpp"

namespace Damaris {

	Calc<std::string::const_iterator,ParameterManager::ParameterMap<int> > 
			*Layout::calc;
	
	Layout::Layout(const Model::Layout& mdl,const std::string &n, const std::vector<int> &e)
	: Configurable<Model::Layout>(mdl), name(n), extents(e)
	{
	}
	
	const std::string& Layout::getName() const
	{
		return name;
	}

	int Layout::getID() const
	{
		return id;
	}
	
	Model::Type Layout::getType() const
	{
		return model.type();
	}
	
	unsigned int Layout::getDimensions() const
	{
		return extents.size();
	}
	
	size_t Layout::getExtentAlongDimension(unsigned int dim) const
	{
		if(dim < extents.size())
			return (size_t)(extents[dim]);
		else
			return 0;
	}

	bool Layout::isUnlimited() const
	{
		return (extents.size() == 1 && extents[0] == -1) 
				|| (Types::basicTypeSize(model.type()) == -1);
	}

	Layout* Layout::New(const Model::Layout& mdl, const std::string &name)
	{
		if(calc == NULL) {
			calc = new Calc<std::string::const_iterator,ParameterManager::ParameterMap<int> >
				(ParameterManager::ParameterMap<int>()); 
		}

		std::vector<int> e;
		std::string str = (std::string)(mdl.dimensions());
		std::string::const_iterator iter = str.begin();
		std::string::const_iterator end = str.end();
		bool r = boost::spirit::qi::phrase_parse(iter, end, *calc,
					boost::spirit::ascii::space, e);
		if((!r) || (iter != str.end())) {
			ERROR("While parsing dimension descriptor for layout \""
							<< mdl.name() << "\"");
			return NULL;
		}

		if((mdl.language() == Model::Language::fortran)
				|| (mdl.language() == Model::Language::unknown
					&& Environment::getDefaultLanguage() == Model::Language::fortran)) {
			std::vector<int> rdims(e.rbegin(),e.rend());
			e = rdims;
		}

		return new Layout(mdl,name,e);
	}

}
