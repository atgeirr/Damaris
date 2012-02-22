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
	Process::kill();
}
	

void* StdAloneClient::alloc(const std::string & varname, int32_t iteration)
{
	// this is basically the same code than in Client, we copy it here
	//  to be sure that any modification on Client won't affect it. 

	// check that the variable is known in the configuration 
	Variable* variable = process->getMetadataManager()->getVariable(varname);

	if(variable == NULL) {
		ERROR("Variable \""<< varname 
				<< "\" not defined in configuration");
		return NULL;
	}

	// the variable is known, get its layout
	Layout* layout = variable->getLayout();

	// prepare variable to initialize a chunk
	std::vector<int> si(layout->getDimensions()),ei(layout->getDimensions());
	for(unsigned int i=0; i < layout->getDimensions(); i++)	{
		ei[i] = layout->getExtentAlongDimension(i)-1;
		si[i] = 0;
	}

	// try initializing the chunk in shared memory
	try {
		ShmChunk* chunk = 
			new ShmChunk(process->getSharedMemorySegment(),layout->getType(),
					layout->getDimensions(),si,ei);
		chunk->setSource(process->getEnvironment()->getID());
		chunk->setIteration(iteration);
		variable->attachChunk(chunk);
		// chunk initialized, returns the data! 
		return chunk->data();

	} catch (...) {
		ERROR("While allocating \"" << varname 
				<< "\", allocation failed");
	}
	// on failure, returns NULL 
	return NULL;
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

int StdAloneClient::write(const std::string & varname, int32_t iteration, const void* data)
{
	// check that the variable is known in the configuration 
	Variable* variable = process->getMetadataManager()->getVariable(varname);

	if(variable == NULL) {
		ERROR("Variable \""<< varname 
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
		chunk = new ShmChunk(process->getSharedMemorySegment(),layout->getType(),
				layout->getDimensions(),si,ei);
		chunk->setSource(process->getEnvironment()->getID());
		chunk->setIteration(iteration);
	} catch (...) {
		ERROR("While writing \"" << varname << "\", allocation failed");
		return -2;
	}

	// copy data
	size_t size = chunk->getDataMemoryLength();
	memcpy(chunk->data(),data,size);

	variable->attachChunk(chunk);	
		
	// send message
	DBG("Variable \"" << varname << "\" has been written");

	return size;
}

int StdAloneClient::chunk_write(chunk_h chunkh, const std::string & varname, 
		int32_t iteration, const void* data)
{
	// check that the variable is know in the configuration 
	Variable* variable = process->getMetadataManager()->getVariable(varname);

	if(variable == NULL) {
		ERROR("Variable \""<< varname << "\" not defined in configuration");
		return -1;
	}

	ChunkHandle* chunkHandle = (ChunkHandle*)chunkh;

	// check if the chunk matches the layout boundaries 
	Layout* layout = variable->getLayout();
	if(not chunkHandle->within(layout)) {
		ERROR("Chunk boundaries do not match variable's layout");
		return -3;
	}

	ShmChunk* chunk = NULL;
	try {
		Types::basic_type_e t = layout->getType();
		unsigned int d = chunkHandle->getDimensions();
		std::vector<int> si(d);
		std::vector<int> ei(d);

		for(unsigned int i=0;i<d; i++) {
			si[i] = chunkHandle->getStartIndex(i);
			ei[i] = chunkHandle->getEndIndex(i);
		}

		chunk = new ShmChunk(process->getSharedMemorySegment(),t,d,si,ei);
		chunk->setSource(process->getEnvironment()->getID());
		chunk->setIteration(iteration);
	} catch (...) {
		ERROR("While writing \"" << varname << "\", allocation failed");
		return -2;
	}

	// copy data
	size_t size = chunk->getDataMemoryLength();
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
