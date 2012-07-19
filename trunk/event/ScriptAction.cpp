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
 * \file ScriptAction.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#include <dlfcn.h>
#include "xml/Model.hpp"
#include "core/Debug.hpp"
#include "core/Environment.hpp"
#include "event/NodeAction.hpp"
#include "event/ScriptAction.hpp"

#ifdef __ENABLE_PYTHON
#include "scripts/python/PyAction.hpp"
#endif

namespace Damaris {

	ScriptAction* ScriptAction::New(const Model::Script& mdl, const std::string& name)
	{
		switch(mdl.scope()) {
			case Model::Scope::core :
				if(mdl.language() == Model::Language::python) {
#ifdef __ENABLE_PYTHON
					return Python::PyAction::New(mdl,name);
#else
					CFGERROR("Damaris has not been compiled with Python support.");
#endif
				 } else {
					CFGERROR("\"" << mdl.language() << "\" is not a valid scripting language.");
				}
				break;
			case Model::Scope::node :
				if(mdl.language() == Model::Language::python)
#ifdef __ENABLE_PYTHON
					return NodeAction<Python::PyAction,Model::Script>::New(mdl,name,Environment::getClientsPerNode());
#else
					CFGERROR("Damaris has not been compiled with Python support.");
#endif
				else {
					CFGERROR("\"" << mdl.language() << "\" is not a valid scripting language.");
				}
				break;
			case Model::Scope::global :
				CFGERROR("Scope \"global\" not implementer yet.");
				break;
			default:
				CFGERROR("Scope \""<< mdl.scope() <<"\" not implementer yet.");		
		}
		return NULL;
	}

}
