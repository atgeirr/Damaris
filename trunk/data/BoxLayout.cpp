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
 * \file BoxLayout.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */

#include <cstring>

#include "core/Debug.hpp"
#include "core/Environment.hpp"
#include "data/Types.hpp"
#include "data/BoxLayout.hpp"

namespace Damaris {

	Calc<std::string::const_iterator,ParameterManager::ParameterMap<int> > 
			*BoxLayout::calc;
	
	BoxLayout::BoxLayout(const Model::Layout& mdl, const std::string &n, const std::vector<int> &e)
	: Layout(n), Configurable<Model::Layout>(mdl), extents(e)
	{
		ObserveDependentParameters();
		InterpretDimensions();
	}
	
	Model::Type BoxLayout::GetType() const
	{
		return model.type();
	}
	
	unsigned int BoxLayout::GetDimensions() const
	{
		return extents.size();
	}

	size_t BoxLayout::GetExtentAlongDimension(unsigned int dim) const
	{
		if(dim < extents.size())
			return (size_t)(extents[dim]);
		else
			return 0;
	}

	bool BoxLayout::IsUnlimited() const
	{
		return (extents.size() == 1 && extents[0] == -1) 
				|| (Types::basicTypeSize(model.type()) == -1);
	}

	Layout* BoxLayout::New(const Model::Layout& mdl, const std::string &name)
	{
		std::vector<int> e;
		return new BoxLayout(mdl,name,e);
	}

	void BoxLayout::ObserveDependentParameters()
	{
		// start parsing the dimensions string to find the names
		// of the parameters. Every time we find one, search in the
		// ParameterManager and connect to it.
		std::string dims = model.dimensions();
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
				if((not (isalnum(dims[i]) || (dims[i] == '_'))) || (i == dims.size() - 1)){
					reading = false;
					buffer[j] = '\0';
					std::string param(&(buffer[0]));
					Parameter* p = ParameterManager::Search(param);
					if(p != NULL) {
						p->AddObserver((Observer*)this);
					}
				}
			}
		}
		// do the same for the "blocks" field
/*
		buffer = std::vector<char>(model.blocks().size()+1);
		buffer[0] = '\0';
		j = 0;
		std::string b = model.blocks();
		reading = false;
		for(unsigned int i = 0; i < b.size(); i++) {
			if((isalpha(b[i]) || b[i] == '_') && !reading) {
				reading = true;
				j = 0;
			}
			if(reading) {
				if(isalnum(b[i]) || (b[i] == '_')) {
					buffer[j] = b[i];
					j++;
				}
				if((not (isalnum(b[i]) || (b[i] == '_'))) || (i == b.size() - 1)){
					reading = false;
					buffer[j] = '\0';
					std::string param(&(buffer[0]));
					Parameter* p = ParameterManager::Search(param);
					if(p != NULL) {
						p->AddObserver((Observer*)this);
					}
				}
			}
		}
*/
	}

	void BoxLayout::InterpretDimensions()
	{
		if(calc == NULL) {
			calc = new Calc<std::string::const_iterator,ParameterManager::ParameterMap<int> >
				(ParameterManager::ParameterMap<int>());
		}

		std::vector<int> e;
		std::string str = (std::string)(model.dimensions());
		std::string::const_iterator iter = str.begin();
		std::string::const_iterator end = str.end();
		bool r = boost::spirit::qi::phrase_parse(iter, end, *calc,
				boost::spirit::ascii::space, e);
		if((!r) || (iter != str.end())) {
			ERROR("While parsing dimension descriptor for layout \""
					<< model.name() << "\"");
		}

		if((model.language() == Model::Language::fortran)
				|| (model.language() == Model::Language::unknown
					&& Environment::DefaultLanguage() == Model::Language::fortran)) {
			std::vector<int> rdims(e.rbegin(),e.rend());
			extents = rdims;
		} else {
			extents = e;
		}
/*
		e.resize(0);
		str = (std::string)(model.blocks());
		iter = str.begin();
		end = str.end();
		r = boost::spirit::qi::phrase_parse(iter, end, *calc,
				boost::spirit::ascii::space, e);
		if((!r) || (iter != str.end())) {
			ERROR("While parsing blocks descriptor for layout \""
					<< model.name() << "\"");
		}
		blocks = 1;
		if(e.size() != 1) {
			ERROR("\"blocks\" field must have exactly one dimension in layout \""
					<< model.name() << "\"");
		} else {
			blocks = e[0];
		}
*/
		DBG("Re-interpreting dimensions and for layout " << name);
	}

	void BoxLayout::Notify()
	{
		InterpretDimensions();
	}

	BoxLayout::~BoxLayout()
	{
	}
}
