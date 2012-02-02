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
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 *
 * ParameterSet.cpp defines the holder of a set of parameters.
 */
#include "common/ParameterSet.hpp"
#include "common/Debug.hpp"

namespace Damaris {

	ParameterSet::ParameterSet(Model::DataModel* mdl) 
		: Configurable<ParameterSet,Model::DataModel>(mdl)
	{
		init();
	}

	void ParameterSet::init() 
	{
		Model::DataModel::parameter_const_iterator p(model->parameter().begin());
		for(; p < model->parameter().end(); p++) {
			std::string name(p->name());
			if(p->type() == "short") {
				short value = boost::lexical_cast<short>(p->value());
				set<short>(name,value);
			} else
			if(p->type() == "int") {
				std::string name(p->name());
				int value = boost::lexical_cast<int>(p->value());
				set<int>(name,value);
			} else
			if(p->type() == "long") {
				long value = boost::lexical_cast<long>(p->value());
				set<long>(name,value);
			} else
			if(p->type() == "float") {
				float value = boost::lexical_cast<float>(p->value());
				set<float>(name,value);
			} else
			if(p->type() == "double") {
				double value = boost::lexical_cast<double>(p->value());
				set<double>(name,value);
			} else
			if(p->type() == "char") {
				char value = boost::lexical_cast<char>(p->value());
				set<char>(name,value);
			} else
			if(p->type() == "string") {
				std::string value = boost::lexical_cast<std::string>(p->value());
				set<std::string>(name,value);
			} else
			{
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

