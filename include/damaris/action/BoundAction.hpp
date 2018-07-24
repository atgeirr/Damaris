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
#ifndef __DAMARIS_BOUND_ACTION_H
#define __DAMARIS_BOUND_ACTION_H

#include "Damaris.h"
#include "damaris/util/Deleter.hpp"
#include "damaris/util/Manager.hpp"
#include "damaris/action/Action.hpp"

namespace damaris {

/**
 * BoundAction is a special type of Action that is created by the user through 
 * a call to damaris_bind_function. Contrary to DynamicAction, it does not seek 
 * functions in a shared library or in the main program, but already has a 
 * pointer to it.
 */
class BoundAction : public Action {
	
	friend class Deleter<BoundAction>;

	protected:
		
		signal_t function_; /*!< Pointer to the loaded function */
		model::Exec execLocation_; /*!< Execution location */
		model::Scope scope_; /*!< Scope of the action */
	
		/**
		 * Constructor.
		 */
		BoundAction(const std::string& name, 
			const model::Exec& e = model::Exec::coupled,
			const model::Scope& s = model::Scope::core)
		: Action(name), function_(NULL), execLocation_(e), scope_(s) {}
			
		/**
		 * Destructor.
		 */
		virtual ~BoundAction() {}

	public:		
		
		/**
		 * \see Action::operator()
		 */
		virtual void Call(int32_t source, int32_t iteration,
				const char* args = NULL) {
			if(function_ != NULL)
				(*function_)(GetName().c_str(),source,
						iteration,args);
		}

		/**
		 * \see Action::IsExternallyVisible
		 */
		virtual bool IsExternallyVisible() const { 
			return false; 
		}

		/**
		 * \see Action::GetExecLocation
		 */
		virtual model::Exec GetExecLocation() const { 
			return execLocation_; 
		}

		/**
		 * \see Action::GetScope
		 */
		virtual model::Scope GetScope() const {
			return scope_;
		}

		/**
		 * Creates a BoundAction given a function pointer.
		 */
		template<typename SUPER>
		static std::shared_ptr<SUPER> New(signal_t f, 
			const std::string& name)
		{
			if(f == NULL) return std::shared_ptr<SUPER>();
			std::shared_ptr<BoundAction> a(new BoundAction(name),
						Deleter<BoundAction>());
			a->function_ = f;
			return a;
		}

};

}

#endif
