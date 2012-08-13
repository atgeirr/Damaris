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
 * \file ParameterManager.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#include "core/ParameterManager.hpp"
#include "data/Types.hpp"
#include "core/Debug.hpp"

namespace Damaris {

	void ParameterManager::Init(const Model::Data& mdl) 
	{
		Model::Data::parameter_const_iterator p(mdl.parameter().begin());
		for(; p < mdl.parameter().end(); p++) {
			DBG("Creating parameter \"" << p->name() << "\"");
			Create<Parameter>(*p,(std::string)(p->name()));
			DBG("Parameter \""  << p->name() << "\" created");
		}
	}
}

