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
 * \file BoundAction.hpp
 * \date February 2012 
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_BOUND_ACTION_H
#define __DAMARIS_BOUND_ACTION_H

#include "core/Manager.hpp"
#include "event/Action.hpp"

namespace Damaris {

/**
 * BoundAction is a special type of Action that is
 * created by the user through a call to DC_bind_function.
 * Contrary to DynamicAction, it does not seek functions in
 * a shared library or in the main program, but already has
 * a pointer to it.
 */
class BoundAction : public Action {

	public:
		typedef void (*fun_t)(const std::string&,int32_t,int32_t,const char*);

	private:
		fun_t function; /*!< Pointer to the loaded function */

	protected:
		/**
		 * Constructor. Takes a function pointer and associates it with a name.
		 */
		BoundAction(fun_t f, const std::string& name);

	public:		
		/**
		 * \brief Destructor.
		 */
		virtual ~BoundAction();
	
		/**
		 * \brief Another way of calling the inner function.
		 * \see Damaris::Action::operator()
		 */
		virtual void Call(int32_t iteration, int32_t sourceID, const char* args = NULL);

		/**
		 * Return true if the action can be called from outside the simulation 
		 * (typically from VisIt).
		 */
		virtual bool IsExternallyVisible() { return false; }

		/**
		 * Tries to create a BoundAction given a function pointer.
		 */
		static BoundAction* New(fun_t f, const std::string& name);

};

}

#endif
