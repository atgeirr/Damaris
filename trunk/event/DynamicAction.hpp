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
 * \file DynamicAction.hpp
 * \date February 2012 
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_DYNAMIC_ACTION_H
#define __DAMARIS_DYNAMIC_ACTION_H

#include "xml/Model.hpp"
#include "core/Manager.hpp"
#include "event/Action.hpp"
#include "core/Configurable.hpp"

namespace Damaris {

/**
 * DynamicAction is a child class of Action used for actions located in
 * dynamic libraries (.so on Linux, .dylib on MacOSX). The dynamic library
 * is loaded only the first time the action is called, thus avoiding
 * unnecessary memory usage.
 */
class DynamicAction : public Action, public Configurable<Model::Event> {

	private:
		void (*function)(std::string,int32_t,int32_t,const char*); /*!< Pointer to the loaded function */
		void* handle; /*!< Handle for the opened dynamic library. */
		bool loaded;

	protected:
		/**
		 * \brief Condtructor.
		 * This constructor is private and can only be called by the ActionsManager
		 * (friend class), because it does not provide the action's ID and name.
		 * 
		 * \param[in] fun : Name of the function to load.
		 * \param[in] file : Name of the dynamic library to load (the file must be
		 * in a directory set in the LD_LIBRARY_PATH environment variable).
		 */	
		DynamicAction(const Model::Event& mdl, const std::string& name);

		virtual void load();

		/**
		 * \brief Destructor.
		 */
		virtual ~DynamicAction();
	
	public:	
		/**
		 * \brief Another way of calling the inner function.
		 * \see Damaris::Action::operator()
		 */
		virtual void call(int32_t iteration, int32_t sourceID, const char* args = NULL);

		/**
		 * Return true if the action can be called from outside the simulation 
		 * (typically from VisIt).
		 */
		virtual bool IsExternallyVisible() { return model.external(); }

		/**
		 * Tries to create a DynamicAction given a model.
		 */
		static DynamicAction* New(const Model::Event& ev, const std::string& name);
};

}

#endif
