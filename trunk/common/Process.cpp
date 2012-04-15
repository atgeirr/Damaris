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

#include "common/Debug.hpp"
#include "common/Process.hpp"

namespace Damaris {

	Process* Process::_instance = NULL;

	void Process::initialize(const std::string &configfile, int32_t id)
	{
		if(_instance != NULL) {
			ERROR("Calling Process:initialize() twice");
			return;
		}
		_instance = new Process(configfile,id);
	}

	Process* Process::get()
	{
		if(_instance == NULL) {
			ERROR("Calling Process::get() before Process::initialize()");
			return NULL;
		}
		return _instance;
	}

	bool Process::kill()
	{
		if(_instance == NULL) {
			ERROR("Calling Process::kill() twice or on an empty instance");
			return false;
		}
		delete _instance;
		_instance = NULL;
		return true;
	}
	
	void Process::init(const std::string &configfile, int32_t id)
	{
		DBG("Starting process initialization");
		try {
			model = Model::simulation(configfile.c_str(),
						xml_schema::flags::dont_validate);
		} catch(xml_schema::exception &e) {
			ERROR(e.what());
			exit(-1);
		}
		
		DBG("Configuration file succefuly read");
		environment     = new Environment(model.get());
		environment->setID(id);
		DBG("Environment initialized");
		metadataManager = new MetadataManager(&(model->data()),environment);
		DBG("MetadataManager initialized");
		actionsManager  = new ActionsManager(&(model->actions()),environment);
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
			msgQueue = SharedMessageQueue::open(&(model->architecture().queue()));
			segment  = SharedMemorySegment::open(&(model->architecture().buffer()));
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
			SharedMessageQueue::remove(&(model->architecture().queue()));
			SharedMemorySegment::remove(&(model->architecture().buffer()));

			msgQueue = SharedMessageQueue::create(&(model->architecture().queue()));
			segment  = SharedMemorySegment::create(&(model->architecture().buffer()));
		}
		catch(interprocess_exception &ex) {
			ERROR("While initializing shared memory objects:  " << ex.what());
			exit(-1);
		}
	}

	Process::Process(const std::string & configfile, int32_t id)
	{
		init(configfile, id);
	}

	Process::~Process()
	{
		if(sharedStructuresOwner) {
			SharedMessageQueue::remove(&(model->architecture().queue()));
			SharedMemorySegment::remove(&(model->architecture().buffer()));	
		}
		delete environment;
		delete metadataManager;
		delete actionsManager;
	}
}

