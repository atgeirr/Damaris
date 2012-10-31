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

#include "core/Debug.hpp"
#include "data/ChunkImpl.hpp"
#include "memory/Message.hpp"
#include "client/Client.hpp"

#include "core/ParameterManager.hpp"
#include "core/VariableManager.hpp"
#include "core/ActionManager.hpp"

Damaris::Client* __client = NULL;

namespace Damaris {

	Client* Client::New(const std::string &cfg, int32_t id)
	{
		Process::initialize(cfg,id);
		Process* p = Process::get();
		p->openSharedStructures();
		Client* c = new Client(p);
		Environment::SetClient(true);
		c->connect();
		return c;
	}

	Client::Client(Process* p)
	{
		FATAL((p == NULL),"Fatal error in Client constructor, Process pointer is NULL");
		process = p;
		errorOccured = false;
	}

	int Client::connect()
	{
		static bool connected = false;
		if(not connected) {
			INFO("connecting client " << Process::get()->getID());
			Message msg;
			msg.type = MSG_INT;
			msg.source = process->getID();
			msg.iteration = 0;
			msg.object = CLIENT_CONNECTED;
			process->getSharedMessageQueue()->send(&msg);
			connected = true;
			return true;
		}
		return false;
	}

	void* Client::alloc(const std::string &varname, bool blocking)
	{
		return alloc_block(varname,0,blocking);
	}

	void* Client::alloc_block(const std::string & varname, 
			int32_t block, bool blocking)
	{

		// check that the variable is known in the configuration
		Variable* variable = VariableManager::Search(varname);

		if(variable == NULL) {
			ERROR("Variable \""<< varname 
					<< "\" not defined in configuration");
			return NULL;
		}
		/*
		if((not variable->IsTimeVarying()) && iteration != 0) {
			WARN("Trying to write a non-time-varying variable at an iteration "
					<< "different from 0, the variable won't be allocated");
			return NULL;
		}
		*/

		// the variable is known, get its layout
		//Layout* layout = variable->GetLayout();

		// TODO : eventually all layouts should be of limited extents, so
		// this test will be useless and the IsUnlimited function will be
		// deprecated
		/*if(layout->IsUnlimited()) {
			ERROR("The layout as undefined extents or has a variable-sized type, "
					<< "use chunk-based allocation");
			return NULL;
		}

		// initialize the chunk descriptor
		ChunkDescriptor* cd = ChunkDescriptor::New(*layout);

		// try allocating the required memory
		size_t size = sizeof(ChunkHeader)+cd->GetDataMemoryLength(layout->GetType());			
		void* location = variable->Allocate(iteration, block);
			//process->getSharedMemorySegment()->Allocate(size);	

		if(location == NULL && not blocking) {
			ERROR("Could not allocate memory: not enough available memory");
			lost(iteration);
			ChunkDescriptor::Delete(cd);
			return NULL;
		} else if(location == NULL && blocking) {
			while(location == NULL) {
				clean(iteration);
				if(process->getSharedMemorySegment()->WaitAvailable(size)) {
					location = process->getSharedMemorySegment()->Allocate(size);
				} else {
					ERROR("Could not allocate memory: not enough total memory");
					ChunkDescriptor::Delete(cd);
					return NULL;
				}
			}
		}
		// create the chunk header in memory
		int source = process->getID();
		ChunkHeader* ch = new(location) ChunkHeader(cd,layout->GetType(),iteration,source);

		// create the ChunkImpl and attach it to the variable
		ChunkImpl* chunk = new ChunkImpl(process->getSharedMemorySegment(),ch);
		variable->AttachChunk(chunk);	

		ChunkDescriptor::Delete(cd);
		// return the pointer to data
		*/
		Chunk* chunk = variable->Allocate(block,blocking);
		if(chunk != NULL) {
			chunk->SetDataOwnership(true);
			return chunk->Data();
		} else {
			errorOccured = true;
			return NULL;
		}
	}

	int Client::commit(const std::string & varname, int32_t iteration)
	{
		// here we assume blocks are numbered from 0 to n-1,
		// maybe the assumption is too strong?
		Variable* v = VariableManager::Search(varname);
                if(v == NULL)
                        return -1;

		int n = v->CountTotalBlocks(iteration);
		for(int b=0; b < n; b++) {
			commit_block(varname,b,iteration);
		}
		return 0;
	}

	int Client::commit_block(const std::string & varname, int32_t block, int32_t iteration)
	{		
		Variable* v = VariableManager::Search(varname);
		if(v == NULL)
			return -1;

		ChunkImpl* chunk = NULL;

		if(iteration < 0)
			iteration = Environment::GetLastIteration();
		
		int source = Process::get()->getID();
		Chunk* c = v->GetChunk(source,iteration,block);

		if(c == NULL) {
			ERROR("Unknown block " << block << " for variable " << varname);
			errorOccured = true;
			return -2;
		}
		try {
			chunk = dynamic_cast<ChunkImpl*>(c);
		} catch(std::exception &e) {
			ERROR("When doing dynamic cast: " << e.what());
			errorOccured = true;
			return -3;
		}

		// create notification message
		Message message;
		message.source = process->getID();

		message.iteration = iteration;
		message.type = MSG_VAR;
		message.handle = chunk->GetHandle();
		message.object = v->GetID();
		// send message
		process->getSharedMessageQueue()->send(&message);
		// free message
		DBG("Variable \"" << varname << "\" has been commited");

		// we don't need to keep the chunk in the client now,
		// so we erase it from the variable, but we don't erase the data.
		chunk->SetDataOwnership(false);
		v->DetachChunk(chunk);

		return 0;
	}

	int Client::write(const std::string & varname, 
			const void* data, bool blocking)
	{
		return write_block(varname, 0, data, blocking);
	}

	int Client::write_block(const std::string &varname, int32_t block,
			const void* data, bool blocking)
	{
		/* check that the variable is know in the configuration */
		Variable* variable = VariableManager::Search(varname);

		if(variable == NULL) {
			ERROR("Unknown variable \"" << varname <<"\"");
			return -1;
		}

		/*
		if((not variable->IsTimeVarying()) && iteration != 0) {
			WARN("Trying to write a non-time-varying variable at an iteration "
					<< "different from 0, the variable won't be written");
			return -4;
		}

		Layout* layout = variable->getLayout();

		if(layout->isUnlimited()) {
			ERROR("Trying to write a variable" 
					<< " with an unlimited layout (use chunk_write instead)");
			return -3;
		}

		// initialize the chunk descriptor
		ChunkDescriptor* cd = ChunkDescriptor::New(*layout);

		// try allocating the required memory
		size_t size = sizeof(ChunkHeader)+cd->getDataMemoryLength(layout->getType());
		void* location = process->getSharedMemorySegment()->allocate(size);

		if(location == NULL && not blocking) {
			ERROR("Could not allocate memory: not enough available memory");
			lost(iteration);
			ChunkDescriptor::Delete(cd);
			return -2;
		} else if(location == NULL && blocking) {
			while(location == NULL) {
				clean(iteration);
				if(process->getSharedMemorySegment()->waitAvailable(size)) {
					location = process->getSharedMemorySegment()->allocate(size);
				} else {
					ERROR("Could not allocate memory: not enough total memory");
					ChunkDescriptor::Delete(cd);
					return -2;
				}
			}
		}

		// create the chunk header in memory
		int source = process->getID();
		ChunkHeader* ch = new(location) ChunkHeader(cd,layout->getType(),
								iteration,source, block);

		// create the ChunkImpl and attach it to the variable
		ChunkImpl chunk(process->getSharedMemorySegment(),ch);
*/
		Chunk* chunk = variable->Allocate(block,blocking);
		if(chunk == NULL) 
		{
			ERROR("Unable to allocate chunk for variable \"" << varname << "\"");
			errorOccured = true;
			return -1;
		}
		// copy data
		//size = cd->getDataMemoryLength(layout->getType());
		//memcpy(chunk.data(),data,size);
		int size = chunk->MemCopy(data);

		// create message
		Message message;

		message.source = chunk->GetSource();
		message.iteration = chunk->GetIteration();
		message.object = variable->GetID();
		message.type = MSG_VAR;
		message.handle = chunk->GetHandle();

		// send message
		process->getSharedMessageQueue()->send(&message);
		DBG("Variable \"" << varname << "\" has been written");

		//ChunkDescriptor::Delete(cd);
		variable->DetachChunk(chunk);

		return size;
	}


	// TODO : remove the following deprecated function
	int Client::chunk_write(chunk_h chunkh, const std::string & varname, 
			int32_t iteration, const void* data, bool blocking)
	{
		/* check that the variable is know in the configuration */
		Variable* variable = VariableManager::Search(varname);

		if(variable == NULL) {
			ERROR("Variable \""<< varname << "\" not defined in configuration");
			return -1;
		}

		if((not variable->IsTimeVarying()) && iteration != 0) {
			WARN("Trying to write a non-time-varying variable at an iteration "
					<< "different from 0, the variable won't be written");
			return -4;
		}

		ChunkDescriptor* cd = (ChunkDescriptor*)chunkh;
		// check if the chunk matches the layout boundaries
		Layout* layout = variable->GetLayout();
		if(not cd->Within(*layout)) {
			ERROR("Chunk boundaries do not match variable's layout");
			return -3;
		}

		// allocate memory
		size_t size = sizeof(ChunkHeader)+cd->GetDataMemoryLength(layout->GetType());
		void* location = process->getSharedMemorySegment()->Allocate(size);

		if(location == NULL && not blocking) {
			ERROR("Could not allocate memory: not enough available memory");
			//lost();
			errorOccured = true;
			return -2;
		} else if(location == NULL && blocking) {
			while(location == NULL) {
				clean();
				if(process->getSharedMemorySegment()->WaitAvailable(size)) {
					location = process->getSharedMemorySegment()->Allocate(size);
				} else {
					ERROR("Could not allocate memory: not enough total memory");
					return -2;
				}
			}
		}

		// create the ChunkHeader
		int source = process->getID();
		ChunkHeader* ch = new(location) ChunkHeader(cd,layout->GetType(),iteration,source);

		// create the ChunkImpl object		
		ChunkImpl chunk(process->getSharedMemorySegment(),ch);

		// copy data
		size = cd->GetDataMemoryLength(layout->GetType());
		memcpy(chunk.Data(),data,size);

		// create message
		Message message;

		message.source = source;
		message.iteration = iteration;
		message.object = variable->GetID();
		message.type = MSG_VAR;
		message.handle = chunk.GetHandle();

		// send message
		process->getSharedMessageQueue()->send(&message);
		DBG("Variable \"" << varname << "\" has been written");

		return size;
	}

	int Client::signal(const std::string & signal_name)
	{
		int32_t iteration = Environment::GetLastIteration();
		try {
			Action* action = ActionManager::Search(signal_name);
			if(action == NULL) {
				DBG("Undefined action \"" << signal_name << "\"");
				return -2;
			}

			Message sig;
			sig.source = process->getID();
			sig.iteration = iteration;
			sig.type = MSG_SIG;
			sig.handle = 0;
			sig.object = action->GetID();

			try {
				process->getSharedMessageQueue()->send(&sig);
			} catch(interprocess_exception &e) {
				ERROR("Error while sending event \"" << 
						signal_name << "\", " << e.what());
				return -1;
			}
			DBG("Event \""<< signal_name << "\" has been sent");
			return 0;
		} catch (std::exception &e) {
			ERROR(e.what());
			return -3;
		}
	}

	int Client::get_parameter(const std::string & paramName, void* buffer, unsigned int size)
	{
		Parameter* p = ParameterManager::Search(paramName);
		if(p != NULL) {
			p->ToBuffer(buffer, size);
			return 0;
		} else {
			return -1;
		}
	}

	int Client::set_parameter(const std::string & paramName, const void* buffer, 
			unsigned int size)
	{
		Parameter* p = ParameterManager::Search(paramName);
		if(p != NULL) {
			p->FromBuffer(buffer,size);
			return 0;
		} else {
			return -1;
		}
	}

	int Client::kill_server()
	{
		static int killed;
		if(!killed) {
			Message kill;
			kill.type = MSG_INT;
			kill.source = process->getID();
			kill.iteration = -1;
			kill.object = KILL_SERVER;
			process->getSharedMessageQueue()->send(&kill);
			return 0;
		} else {
			WARN("Trying to send kill signal multiple times to the server");
			return -1;
		}
	}

	int Client::clean()
	{
		Message msg;
		msg.type = MSG_INT;
		msg.source = process->getID();
		msg.iteration = Environment::GetLastIteration();
		msg.object = URGENT_CLEAN;
		process->getSharedMessageQueue()->send(&msg);
		return 0;
	}

	int Client::lost()
	{
		Message msg;
		msg.type = MSG_INT;
		msg.source = process->getID();
		msg.iteration = Environment::GetLastIteration();
		msg.object = LOST_DATA;
		process->getSharedMessageQueue()->send(&msg);
		return 0;	
	}

	int Client::end_iteration()
	{
		int iteration = Environment::GetLastIteration();
		Environment::StartNextIteration();
		Message msg;
		msg.type = MSG_INT;
		msg.source = process->getID();
		msg.iteration = iteration;
		if(not errorOccured)
			msg.object = END_ITERATION;
		else {
			msg.object = END_ITERATION_NO_UPDATE;
			errorOccured = false;
		}
		
		process->getSharedMessageQueue()->send(&msg);
		return 0;
	}

	chunk_h Client::chunk_set(unsigned int dimensions,
			const std::vector<int> & startIndices, 
			const std::vector<int> & endIndices)
	{
		ChunkDescriptor *c = ChunkDescriptor::New(dimensions,&startIndices[0],&endIndices[0]);
		return c;
	}

	void Client::chunk_free(chunk_h chunkh) 
	{
		ChunkDescriptor::Delete(chunkh);
	}

	MPI_Comm Client::mpi_get_client_comm()
	{
		return Environment::getEntityComm();
	}

	Client::~Client() 
	{
		Process::kill();
		DBG("Client destroyed successfuly");
	}

}

