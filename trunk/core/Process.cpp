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
 * \file Process.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 * \see Process.hpp
 */
#include <string.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <exception>

#include "core/Debug.hpp"
#include "core/VariableManager.hpp"
#include "core/MeshManager.hpp"
#include "core/LayoutManager.hpp"
#include "core/ParameterManager.hpp"
#include "core/ActionManager.hpp"
#include "core/Process.hpp"
#ifdef __ENABLE_PYTHON
#include "scripts/python/PyInterpreter.hpp"
#endif

namespace Damaris {

	Process* Process::_instance = NULL;

	void Process::Init(std::auto_ptr<Model::Simulation> mdl, int32_t id)
	{
		if(_instance != NULL) {
			ERROR("Calling Process:initialize() twice");
			return;
		}
		_instance = new Process(mdl,id);
	}

	Process* Process::Get()
	{
		if(_instance == NULL) {
			ERROR("Calling Process::get() before Process::initialize()");
			return NULL;
		}
		return _instance;
	}

	bool Process::Kill()
	{
		if(_instance == NULL) {
			ERROR("Calling Process::kill() twice or on an empty instance");
			return false;
		}
		delete _instance;
		_instance = NULL;
		return true;
	}
	
	Process::Process(std::auto_ptr<Model::Simulation> mdl, int32_t i)
	{
		DBG("Starting process initialization");
		model = mdl;
		id = i;

#ifdef __ENABLE_PYTHON
		if(model->python().present()) {
			Python::PyInterpreter::SetParameters(model->python().get());
		}
#endif
		DBG("Configuration file succefuly read");
		Environment::Init(*(model.get()));
		DBG("Environment initialized");
		ParameterManager::Init(model->data());
		LayoutManager::Init(model->data());
		VariableManager::Init(model->data());
		MeshManager::Init(model->data());
		DBG("Data Managers initialized");
		ActionManager::Init(model->actions());
		DBG("ActionsManager initialized");
		segment = NULL;
		msgQueue = NULL;	
		DBG("Process initialized");
	}

	void Process::openSharedStructures()
	{
		DBG("Calling Process::openSharedStructures()");
		sharedStructuresOwner = false;
		if(segment != NULL) {
			ERROR("Shared structures already opened");
			return;
		}
		try {
			msgQueue = SharedMessageQueue::Open(&(model->architecture().queue()));
			segment  = SharedMemorySegment::Open(&(model->architecture().buffer()));
		}
		catch(interprocess_exception &ex) {
			ERROR("While initializing shared memory objects:  " << ex.what());
			exit(-1);
		}
	}

	void Process::createSharedStructures()
	{
		DBG("Calling Process::createSharedStructures()");
		sharedStructuresOwner = true;
		if(segment != NULL) {
			ERROR("Shared structures already created");
			return;
		}
		try {
			SharedMessageQueue::Remove(&(model->architecture().queue()));
			SharedMemorySegment::Remove(&(model->architecture().buffer()));

			msgQueue = SharedMessageQueue::Create(&(model->architecture().queue()));
			segment  = SharedMemorySegment::Create(&(model->architecture().buffer()));
		}
		catch(interprocess_exception &ex) {
			ERROR("While initializing shared memory objects:  " << ex.what());
			exit(-1);
		}
	}

	Process::~Process()
	{
		if(sharedStructuresOwner) {
			SharedMessageQueue::Remove(&(model->architecture().queue()));
			SharedMemorySegment::Remove(&(model->architecture().buffer()));	
		}
	}
}

