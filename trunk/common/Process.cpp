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
 * \file Client.cpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 * \see Client.hpp
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
	bool Process::_instanciated = false;
	
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
		
		if(_instanciated) {
                        ERROR("Double instanciation of a Process object");
                }
                _instanciated = true;
                _instance = this;

		DBG("Configuration file succefuly read");
		environment     = new Environment(model.get());
		environment->setID(id);
		DBG("Environment initialized");
		metadataManager = new MetadataManager(&(model->data()),environment);
		DBG("MetadataManager initialized");
		actionsManager  = new ActionsManager(&(model->actions()),environment);
		DBG("ActionsManager initialized");
		
		DBG("Process initialized");
	}

/*
	void Process::init(std::auto_ptr<Damaris::Model::SimulationModel> mdl)
	{
		model = mdl;
		init();
	}
*/
	void Process::openSharedStructures()
	{
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

/*
	Process::Process(const std::string & configfile, int32_t id)
	{
		init(configfile);
		openSharedStructures();
	}
*/
/*
	Process::Process(std::auto_ptr<Damaris::Model::SimulationModel> mdl, int32_t id, struct open o)
	{
		init(mdl);
		openSharedStructures();
	}
	
	Process::Process(std::auto_ptr<Damaris::Model::SimulationModel> mdl, int32_t id, struct create o)
        {
		init(mdl);
		createSharedStructures();
        }
*/	
	Process::Process(const std::string & configfile, int32_t id)//, struct create c)
	{
		init(configfile, id);
	}

	Process::~Process()
	{
		_instance = NULL;
		_instanciated = false;
		if(sharedStructuresOwner) {
			SharedMessageQueue::remove(&(model->architecture().queue()));
			SharedMemorySegment::remove(&(model->architecture().buffer()));	
		}
	}
}

