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
 * \file StdAloneClient.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#include <iostream>
#include <list>
#include "common/Debug.hpp"
#include "common/Message.hpp"
#include "common/ShmChunk.hpp"
#include "common/Layout.hpp"
#include "client/StdAloneClient.hpp"

namespace Damaris {

StdAloneClient::StdAloneClient(Process* p)
: Client(p)
{ }


/* destructor */
StdAloneClient::~StdAloneClient()
{
}

void* StdAloneClient::alloc(const std::string & varname, int32_t iteration, bool blocking)
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

		// This piece of code changes from Client.cpp: we don't want to block
		// if there is no way to get more memory.
        if(location == NULL && blocking) {
                clean(iteration);
                location = process->getSharedMemorySegment()->allocate(size);
        }
		if(location == NULL) {
            ERROR("Could not allocate memory: not enough available memory");
            return NULL;
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

int StdAloneClient::commit(const std::string & varname, int32_t iteration)
{		
	Variable* v = process->getMetadataManager()->getVariable(varname);
	if(v == NULL)
		return -1;

	// get the pointer to the allocated chunk
	ChunkIndexByIteration::iterator end;
	ChunkIndexByIteration::iterator it = v->getChunksByIteration(iteration,end);

	if(it == end)
		return -2;
	try {
		dynamic_cast<ShmChunk*>(it->get());
	} catch(std::exception &e) {
		ERROR("When doing dynamic cast: " << e.what());
		return -3;
	}

	// nothing to do actually, the server already knows the variable

	DBG("Variable \"" << varname << "\" has been commited");

	return 0;
}

int StdAloneClient::write(const std::string & varname, int32_t iteration, const void* data, bool blocking)
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

		// This piece of code changes from Client.cpp: we don't want to block
        // if there is no way to get more memory.
        if(location == NULL && blocking) {
                clean(iteration);
                location = process->getSharedMemorySegment()->allocate(size);
        }
        if(location == NULL) {
            ERROR("Could not allocate memory: not enough available memory");
            return -2;
        }
		// create the chunk header in memory
		int source = process->getEnvironment()->getID();
		ChunkHeader* ch = new(location) ChunkHeader(cd,iteration,source);

		// create the ShmChunk and attach it to the variable
		ShmChunk* chunk = new ShmChunk(process->getSharedMemorySegment(),ch);

		// copy data
		size = cd.getDataMemoryLength();
		memcpy(chunk->data(),data,size);

		variable->attachChunk(chunk);	

		DBG("Variable \"" << varname << "\" has been written");

		return size;
}

int StdAloneClient::chunk_write(chunk_h chunkh, const std::string & varname, 
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

		        // This piece of code changes from Client.cpp: we don't want to block
        // if there is no way to get more memory.
        if(location == NULL && blocking) {
                clean(iteration);
                location = process->getSharedMemorySegment()->allocate(size);
        }
        if(location == NULL) {
            ERROR("Could not allocate memory: not enough available memory");
            return -2;
        }

        // create the ChunkHeader
        int source = process->getEnvironment()->getID();
        ChunkHeader* ch = new(location) ChunkHeader(*cd,iteration,source);

        // create the ShmChunk object       
        ShmChunk* chunk = new ShmChunk(process->getSharedMemorySegment(),ch);

		// copy data
        size = cd->getDataMemoryLength();
        memcpy(chunk->data(),data,size);
		
		variable->attachChunk(chunk);	
		DBG("Variable \"" << varname << "\" has been written");

		return size;
}

int StdAloneClient::signal(const std::string & signal_name, int32_t iteration)
{
	Action* action = process->getActionsManager()->getAction(signal_name);
	if(action == NULL) {
		DBG("Undefined action \"" << signal_name << "\"");
		return -2;
	}

	action->call(iteration,process->getEnvironment()->getID());

	DBG("Event \""<< signal_name << "\" has been sent");
	return 0;
}

int StdAloneClient::kill_server()
{
	WARN("Synchronous server cannot be killed (you own your process, man!)");
	return -1;
}

int StdAloneClient::clean(int iteration)
{
	return signal("clean",iteration);
}
}