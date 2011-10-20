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
 * \date September 2011
 * \author Matthieu Dorier
 * \version 0.3
 * \see Client.hpp
 */
#include <string.h>
#include <string>
#include <stdlib.h>
#include <iostream>

#include "common/Debug.hpp"
#include "common/ChunkHandle.hpp"
#include "common/ShmChunk.hpp"
#include "common/Message.hpp"
#include "client/Client.hpp"

namespace Damaris {
	
	Client::Client(std::string* configfile, int32_t coreID)
	{
		/* creates the configuration object from the configuration file */
		std::auto_ptr<Model::simulation_mdl> 
			mdl(Model::simulation(configfile->c_str(),xml_schema::flags::dont_validate));

		Configuration::initialize(mdl,configfile);
		config = Configuration::getInstance();

		Environment::initialize(mdl,coreID);
		env = Environment::getInstance();
		id = env->getID();

		metadataManager = config->getMetadataManager();
		actionsManager = config->getActionsManager();
		/* initializes the shared structures */
		try {
#ifdef __SYSV
			msgQueue = SharedMessageQueue::open(sysv_shmem,
					config->getMsgQueueName()->c_str());
			segment = SharedMemorySegment::open(sysv_shmem,
					config->getSegmentName()->c_str());
#else
			msgQueue = SharedMessageQueue::open(posix_shmem,
					config->getMsgQueueName()->c_str());
			segment = SharedMemorySegment::open(posix_shmem,
					config->getSegmentName()->c_str());
#endif
			DBG("Client initialized successfully for core " << id 
			    << " with configuration \"" << *configfile << "\"");
		}
		catch(interprocess_exception &ex) {
			ERROR("While initializing shared memory objects:  " << ex.what());
			exit(-1);
		}

		metadataManager = config->getMetadataManager();
		actionsManager = config->getActionsManager();
	}
	
	void* Client::alloc(std::string* varname, int32_t iteration)
	{

		/* check that the variable is know in the configuration */
		std::string name(*varname);
		Variable* variable = metadataManager->getVariable(name);

        	if(variable == NULL) {
			ERROR("Variable \""<< varname->c_str() 
				<< "\" not defined in configuration");
			return NULL;
        	}

		Layout* layout = variable->getLayout();

		std::vector<int> si(layout->getDimensions()),ei(layout->getDimensions());
		for(unsigned int i=0; i < layout->getDimensions(); i++)	{
			ei[i] = layout->getExtentAlongDimension(i)-1;
			si[i] = 0;
		}

		try {
			ShmChunk* chunk = 
				new ShmChunk(segment,layout->getType(),layout->getDimensions(),si,ei);
			chunk->setSource(id);
			chunk->setIteration(iteration);
			variable->attachChunk(chunk);
			return chunk->data();

		} catch (...) {
			ERROR("While allocating \"" << varname->c_str() 
				<< "\", allocation failed");
		}

		return NULL;
	}
	
	int Client::commit(std::string* varname, int32_t iteration)
	{		
		Variable* v = metadataManager->getVariable(*varname);
		if(v == NULL)
			return -1;

		ShmChunk* chunk = NULL;
		// get the pointer to the allocated chunk and delete it from the
		// variable container.
		// TODO gets only the Chunk which iteration is the right one.
		if(v->getAllChunks().empty())
			return -2;

		try {
			chunk = dynamic_cast<ShmChunk*>(v->getAllChunks().back());
		} catch(exception &e) {
			ERROR("When doing dynamic cast: " << e.what());
			v->getAllChunks().pop_back();
		}

		// create notification message
		Message message;
		message.source = id;

		message.iteration = iteration;
		message.type = MSG_VAR;
		message.handle = chunk->getHandle();
		message.object = v->getID();
                // send message
		msgQueue->send(&message,sizeof(Message),0);
                // free message
		DBG("Variable \"" << varname->c_str() << "\" has been commited");
		// delete the chunk
		delete chunk;

		return 0;
	}
	
	int Client::write(std::string* varname, int32_t iteration, const void* data)
	{
		/* check that the variable is know in the configuration */
		std::string name(*varname);
		Variable* variable = metadataManager->getVariable(name);

        	if(variable == NULL) {
			ERROR("Variable \""<< varname->c_str() 
				<< "\" not defined in configuration");
			return -1;
        	}

		Layout* layout = variable->getLayout();

		std::vector<int> si(layout->getDimensions()),ei(layout->getDimensions());
                for(unsigned int i=0; i < layout->getDimensions(); i++) {
                        ei[i] = layout->getExtentAlongDimension(i)-1;
                        si[i] = 0;
                }

		ShmChunk* chunk = NULL;
                try {
                        chunk = new ShmChunk(segment,layout->getType(),
						layout->getDimensions(),si,ei);
                        chunk->setSource(id);
                        chunk->setIteration(iteration);
                } catch (...) {
                        ERROR("While writing \"" << varname->c_str() << "\", allocation failed");
                	return -2;
		}

		// copy data
		size_t size = chunk->getDataMemoryLength();
		memcpy(chunk->data(),data,size);
		
		// create message
		Message message;
		
		message.source = id;
		message.iteration = iteration;
		message.object = variable->getID();
		message.type = MSG_VAR;
		message.handle = chunk->getHandle();
		
		// send message
		msgQueue->send(&message,sizeof(Message),0);
		DBG("Variable \"" << varname->c_str() << "\" has been written");
	
		delete chunk;
		return size;
	}

	int Client::write(std::string* varname, int32_t iteration, int64_t chunkh, const void* data)
	{
		/* check that the variable is know in the configuration */
		std::string name(*varname);
		Variable* variable = metadataManager->getVariable(name);

        	if(variable == NULL) {
			ERROR("Variable \""<< varname->c_str() << "\" not defined in configuration");
			return -1;
        	}

		ChunkHandle* chunkHandle = (ChunkHandle*)chunkh;

		/* check if the chunk matches the layout boundaries */
		Layout* layout = variable->getLayout();
		if(not chunkHandle->within(layout)) {
			ERROR("Chunk boundaries do not match variable's layout");
			return -3;
		}

		ShmChunk* chunk = NULL;
                try {
			Types::basic_type_e t = chunkHandle->getType();
			unsigned int d = chunkHandle->getDimensions();
			std::vector<int> si(d);
			std::vector<int> ei(d);

			for(unsigned int i=0;i<d; i++) {
				si[i] = chunkHandle->getStartIndex(i);
				ei[i] = chunkHandle->getEndIndex(i);
			}

                        chunk = new ShmChunk(segment,t,d,si,ei);
                        chunk->setSource(id);
                        chunk->setIteration(iteration);
                } catch (...) {
                        ERROR("While writing \"" << varname->c_str() << "\", allocation failed");
                	return -2;
		}

		// copy data
		size_t size = chunk->getDataMemoryLength();
		memcpy(chunk->data(),data,size);
		
		// create message
		Message message;
		
		message.source = id;
		message.iteration = iteration;
		message.object = variable->getID();
		message.type = MSG_VAR;
		message.handle = chunk->getHandle();
		
		// send message
		msgQueue->send(&message,sizeof(Message),0);
		DBG("Variable \"" << varname->c_str() << "\" has been written");
	
		// free message	
		delete chunk;
		
		return size;
	}

	int Client::signal(std::string* signal_name, int32_t iteration)
	{
		Message sig;
		sig.source = id;
		sig.iteration = iteration;
		sig.type = MSG_SIG;
		sig.handle = 0;
		sig.object = 0; // TODO : put the Id of the action
		
		try {
			msgQueue->send(&sig,sizeof(Message),0);
			return 0;
		} catch(interprocess_exception &e) {
			ERROR("Error while sending event \"" << *signal_name << "\", " << e.what());
			return -1;
		}
	}

	int Client::getParameter(std::string* paramName, void* buffer)
	{
		/*
		Types::basic_type_e t;
		if(config->getParameterType(paramName->c_str(),&t)) 
		{
			config->getParameterValue(paramName->c_str(),buffer);
			return 0;
		} else {
			ERROR("Parameter \""<< paramName->c_str() <<"\"not found in the configuration");
			return -1;
		}*/
		// TODO THIS FUNCTION HAS TO BE RE-IMPLEMENTED
		return -1;
	}
		
	int Client::killServer()
	{
		static int killed;
		if(!killed) {
			Message kill;
			kill.type = MSG_INT;
			kill.source = id;
			kill.iteration = -1;
			kill.object = KILL_SERVER;
			msgQueue->send(&kill,sizeof(Message),0);
			return 0;
		} else {
			WARN("Trying to send kill signal multiple times to the server");
			return -1;
		}
	}
	
	Client::~Client() 
	{
		delete msgQueue;
		delete segment;

		config->finalize();
		env->finalize();

		INFO("Client destroyed successfuly");
	}
	
}

