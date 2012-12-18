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
 * \file ScriptAction.hpp
 * \date February 2012 
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_SCRIPT_ACTION_H
#define __DAMARIS_SCRIPT_ACTION_H

#include "xml/Model.hpp"
#include "core/Manager.hpp"
#include "event/Action.hpp"
#include "core/Configurable.hpp"

namespace Damaris {

/**
 * ScriptAction describes an Action that wraps an interpretable script,
 * such as a Python script. This class is virtual and inherited, for
 * example, by PyAction.
 */
class ScriptAction : public Action, public Configurable<Model::Script> {

	protected:
		/**
		 * \brief Condtructor.
		 */
		ScriptAction(const Model::Script& mdl, const std::string& name)
		: Action(name), Configurable<Model::Script>(mdl)
		{ }

		/**
		 * Destructor.
		 */
		virtual ~ScriptAction() {}

	public:	
		/**
		 * \brief Another way of calling the inner function.
		 * \see Damaris::Action::operator()
		 */
		virtual void Call(int32_t iteration, int32_t sourceID, const char* args = NULL) = 0;

		/**
		 * Tells if the action can be called from outside the simulation.
		 */
		virtual bool IsExternallyVisible() { return model.external(); }

		/**
		 * Creates a new instance of an inherited class of ScriptAction according
		 * to the "language" field in the description.
		 */
		static ScriptAction* New(const Model::Script& ev, const std::string& name);
};

}

#endif
