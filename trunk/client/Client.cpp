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
 * \date February 2012 
 * \author Matthieu Dorier
 * \version 0.4
 * \see Client.hpp
 */
#include <string.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <exception>

#include "common/Debug.hpp"
#include "common/ShmChunk.hpp"
#include "common/Message.hpp"
#include "client/Client.hpp"

namespace Damaris {
	
	Client* Client::New(const std::string &cfg, int32_t id)
	{
		Process::initialize(cfg,id);
		Process* p = Process::get();
		p->openSharedStructures();
		return new Client(p);
	}

	Client::Client(Process* p)
	{
		FATAL((p == NULL),"Fatal error in Client constructor, Process pointer is NULL");
		process = p;
	}	

	void* Client::alloc(const std::string & varname, int32_t iteration, bool blocking)
	{

		// check that the variable is known in the configuration
		Variable* variable = process->getMetadataManager()->getVariable(varname);

        if(variable == NULL) {
			ERROR("Variable \""<< varname 
				<< "\" not defined in configuration");
			return NULL;
        }

		// the variable is known, get its layout
		Layout* layout = variable->getLayout();
	
		if(layout->isUnlimited()) {
			ERROR("Trying to allocate memory for an unlimited layout");
			return NULL;
		}

		// initialize the chunk descriptor
		ChunkDescriptor cd(*layout);

		// try allocating the required memory
		size_t size = sizeof(ChunkHeader)+cd.getDataMemoryLength();			
		void* location = process->getSharedMemorySegment()->allocate(size);	

		if(location == NULL && not blocking) {
			ERROR("Could not allocate memory: not enough available memory");
			return NULL;
		} else if(location == NULL && blocking) {
			while(location == NULL) {
				clean(iteration);
				if(process->getSharedMemorySegment()->waitAvailable(size)) {
					location = process->getSharedMemorySegment()->allocate(size);
				} else {
					ERROR("Could not allocate memory: not enough total memory");
					return NULL;
				}
			}
		}
		// create the chunk header in memory
		int source = process->getEnvironment()->getID();
		ChunkHeader* ch = new(location) ChunkHeader(cd,iteration,source);

		// create the ShmChunk and attach it to the variable
		ShmChunk* chunk = new ShmChunk(process->getSharedMemorySegment(),ch);
		variable->attachChunk(chunk);	

		// return the pointer to data
		return chunk->data();
	}
	
	int Client::commit(const std::string & varname, int32_t iteration)
	{		
		Variable* v = process->getMetadataManager()->getVariable(varname);
		if(v == NULL)
			return -1;

		ShmChunk* chunk = NULL;
		// get the pointer to the allocated chunk
		ChunkIndexByIteration::iterator end;
		ChunkIndexByIteration::iterator it = v->getChunksByIteration(iteration,end);

		if(it == end)
			return -2;
		try {
			chunk = dynamic_cast<ShmChunk*>(it->get());
		} catch(std::exception &e) {
			ERROR("When doing dynamic cast: " << e.what());
			return -3;
		}
		
		// create notification message
		Message message;
		message.source = process->getEnvironment()->getID();

		message.iteration = iteration;
		message.type = MSG_VAR;
		message.handle = chunk->getHandle();
		message.object = v->getID();
		// send message
		process->getSharedMessageQueue()->send(&message);
		// free message
		DBG("Variable \"" << varname << "\" has been commited");

		// we don't need to keep the chunk in the client now,
        // so we erase it from the variable.
        v->detachChunk(it);

		return 0;
	}
	
	int Client::write(const std::string & varname, int32_t iteration, const void* data, bool blocking)
	{
		/* check that the variable is know in the configuration */
		Variable* variable = process->getMetadataManager()->getVariable(varname);

        if(variable == NULL) {
			return -1;
        }

		Layout* layout = variable->getLayout();

		if(layout->isUnlimited()) {
			ERROR("Trying to write a variable" 
				<< " with an unlimited layout (use chunk_write instead)");
			return -3;
		}

		// initialize the chunk descriptor
        ChunkDescriptor cd(*layout);

        // try allocating the required memory
        size_t size = sizeof(ChunkHeader)+cd.getDataMemoryLength();
        void* location = process->getSharedMemorySegment()->allocate(size);

		if(location == NULL && not blocking) {
            ERROR("Could not allocate memory: not enough available memory");
            return -2;
        } else if(location == NULL && blocking) {
            while(location == NULL) {
				clean(iteration);
                if(process->getSharedMemorySegment()->waitAvailable(size)) {
                    location = process->getSharedMemorySegment()->allocate(size);
                } else {
                    ERROR("Could not allocate memory: not enough total memory");
                    return -2;
                }
            }
        }

        // create the chunk header in memory
        int source = process->getEnvironment()->getID();
        ChunkHeader* ch = new(location) ChunkHeader(cd,iteration,source);

        // create the ShmChunk and attach it to the variable
        ShmChunk chunk(process->getSharedMemorySegment(),ch);

		// copy data
		size = cd.getDataMemoryLength();
		memcpy(chunk.data(),data,size);
		
		// create message
		Message message;
		
		message.source = source;
		message.iteration = iteration;
		message.object = variable->getID();
		message.type = MSG_VAR;
		message.handle = chunk.getHandle();
		
		// send message
		process->getSharedMessageQueue()->send(&message);
		DBG("Variable \"" << varname << "\" has been written");
	
		return size;
	}

	int Client::chunk_write(chunk_h chunkh, const std::string & varname, 
			int32_t iteration, const void* data, bool blocking)
	{
		/* check that the variable is know in the configuration */
		Variable* variable = process->getMetadataManager()->getVariable(varname);

        if(variable == NULL) {
			ERROR("Variable \""<< varname << "\" not defined in configuration");
			return -1;
        }

		ChunkDescriptor* cd = (ChunkDescriptor*)chunkh;

		// check if the chunk matches the layout boundaries
		Layout* layout = variable->getLayout();
		if(not cd->within(*layout)) {
			ERROR("Chunk boundaries do not match variable's layout");
			return -3;
		}
	
		// allocate memory
		size_t size = sizeof(ChunkHeader)+cd->getDataMemoryLength();
		void* location = process->getSharedMemorySegment()->allocate(size);

		if(location == NULL && not blocking) {
            ERROR("Could not allocate memory: not enough available memory");
            return -2;
        } else if(location == NULL && blocking) {
            while(location == NULL) {
				clean(iteration);
                if(process->getSharedMemorySegment()->waitAvailable(size)) {
                    location = process->getSharedMemorySegment()->allocate(size);
                } else {
                    ERROR("Could not allocate memory: not enough total memory");
                    return -2;
                }
            }
        }

		// create the ChunkHeader
		int source = process->getEnvironment()->getID();
		ChunkHeader* ch = new(location) ChunkHeader(*cd,iteration,source);

		// create the ShmChunk object		
		ShmChunk chunk(process->getSharedMemorySegment(),ch);

		// copy data
		size = cd->getDataMemoryLength();
		memcpy(chunk.data(),data,size);
		
		// create message
		Message message;
		
		message.source = source;
		message.iteration = iteration;
		message.object = variable->getID();
		message.type = MSG_VAR;
		message.handle = chunk.getHandle();
		
		// send message
		process->getSharedMessageQueue()->send(&message);
		DBG("Variable \"" << varname << "\" has been written");
	
		return size;
	}

	int Client::signal(const std::string & signal_name, int32_t iteration)
	{
		try {
		Action* action = process->getActionsManager()->getAction(signal_name);
		if(action == NULL) {
			DBG("Undefined action \"" << signal_name << "\"");
			return -2;
		}

		Message sig;
		sig.source = process->getEnvironment()->getID();
		sig.iteration = iteration;
		sig.type = MSG_SIG;
		sig.handle = 0;
		sig.object = action->getID();
		
		try {
			process->getSharedMessageQueue()->send(&sig);
		} catch(interprocess_exception &e) {
			ERROR("Error while sending event \"" << signal_name << "\", " << e.what());
			return -1;
		}
		DBG("Event \""<< signal_name << "\" has been sent");
		return 0;
		} catch (std::exception &e) {
			ERROR(e.what());
			return -3;
		}
	}

	int Client::get_parameter(const std::string & paramName, void* buffer)
	{
		// TODO
		return -1;
	}
		
	int Client::kill_server()
	{
		static int killed;
		if(!killed) {
			Message kill;
			kill.type = MSG_INT;
			kill.source = process->getEnvironment()->getID();
			kill.iteration = -1;
			kill.object = KILL_SERVER;
			process->getSharedMessageQueue()->send(&kill);
			return 0;
		} else {
			WARN("Trying to send kill signal multiple times to the server");
			return -1;
		}
	}

	int Client::clean(int iteration)
    {
		Message kill;
		kill.type = MSG_INT;
		kill.source = process->getEnvironment()->getID();
		kill.iteration = iteration;
		kill.object = URGENT_CLEAN;
		process->getSharedMessageQueue()->send(&kill);
		return 0;
    }

	chunk_h Client::chunk_set(unsigned int dimensions,
			const std::vector<int> & startIndices, 
			const std::vector<int> & endIndices)
	{
		Types::basic_type_e t = Types::UNDEFINED_TYPE;
		ChunkDescriptor *c = new ChunkDescriptor(t,dimensions,&startIndices[0],&endIndices[0]);
		return c;
	}

	void Client::chunk_free(chunk_h chunkh) 
	{
		if(chunkh != 0) delete (ChunkDescriptor*)chunkh;
	}

	MPI_Comm Client::mpi_get_client_comm()
	{
		return process->getEnvironment()->getEntityComm();
	}
	
	Client::~Client() 
	{
		Process::kill();
		DBG("Client destroyed successfuly");
	}
	
}

