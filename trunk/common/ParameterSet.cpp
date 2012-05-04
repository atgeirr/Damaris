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
 * \file ParameterSet.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#include "common/ParameterSet.hpp"
#include "common/Types.hpp"
#include "common/Debug.hpp"

namespace Damaris {

	ParameterSet::ParameterSet(Model::DataModel* mdl) 
		: Configurable<ParameterSet,Model::DataModel>(mdl)
	{
		init();
	}

	void ParameterSet::init() 
	{
		if(model == NULL) return;

		Model::DataModel::parameter_const_iterator p(model->parameter().begin());
		for(; p < model->parameter().end(); p++) {
			std::string name(p->name());
			switch(p->type()) {
			case Model::TypeModel::short_:
				set<short>(name,boost::lexical_cast<short>(p->value()));
				break;
			case Model::TypeModel::int_:
				set<int>(name,boost::lexical_cast<int>(p->value()));
				break;
			case Model::TypeModel::integer:
				set<int>(name,boost::lexical_cast<int>(p->value()));
				break;
			case Model::TypeModel::long_:
				set<long>(name,boost::lexical_cast<long>(p->value()));
				break;
			case Model::TypeModel::float_:
				set<float>(name,boost::lexical_cast<float>(p->value()));
				break;
			case Model::TypeModel::real :
				set<float>(name,boost::lexical_cast<float>(p->value()));
				break;
			case Model::TypeModel::double_:
				set<double>(name,boost::lexical_cast<double>(p->value()));
				break;
			case Model::TypeModel::char_:
				set<char>(name,boost::lexical_cast<char>(p->value()));
				break;
			case Model::TypeModel::character:
				set<char>(name,boost::lexical_cast<char>(p->value()));
				break;
			case Model::TypeModel::string:
				set<std::string>(name,boost::lexical_cast<std::string>(p->value()));
				break;
			default:
				ERROR("Undefined type \"" << p->type()
					<< "\" for parameter \""<< p->name() << "\"");
			}
		}
	}

	int ParameterSet::operator[](std::string& n)
	{
		return get<int>(n);
	}

}

