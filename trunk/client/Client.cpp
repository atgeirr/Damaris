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
 * \date November 2012
 * \author Matthieu Dorier
 * \version 0.7
 * \see Client.hpp
 */
#include <string.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <exception>

#include "core/Debug.hpp"
#include "memory/Message.hpp"
#include "client/Client.hpp"

#include "core/ParameterManager.hpp"
#include "core/VariableManager.hpp"
#include "core/ActionManager.hpp"

Damaris::Client* __client = NULL;

namespace Damaris {

	Client* Client::New(std::auto_ptr<Model::Simulation> mdl, MPI_Comm clients)
	{
		int32_t id;
		MPI_Comm_rank(clients,&id);
		Process::Init(mdl,id);
		Process* p = Process::Get();
		p->openSharedStructures();
		Environment::SetEntityComm(clients);
		Environment::SetGlobalComm(clients);
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
			DBG("connecting client " << Process::get()->getID());
			Message msg;
			msg.type = MSG_INT;
			msg.source = process->getID();
			msg.iteration = 0;
			msg.object = CLIENT_CONNECTED;
			process->getSharedMessageQueue()->Send(&msg);
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
		if(errorOccured) return NULL;

		if(block < (int32_t)0 
			|| block >= (int32_t)Environment::NumDomainsPerClient())
		{
			ERROR("Invalid block ID");
			return NULL;
		}
	
		// check that the variable is known in the configuration
		Variable* variable = VariableManager::Search(varname);

		if(variable == NULL) {
			ERROR("Variable \""<< varname 
					<< "\" not defined in configuration");
			return NULL;
		}
		
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

		Chunk* chunk = NULL;

		if(iteration < 0)
			iteration = Environment::GetLastIteration();
		
		int source = Process::Get()->getID();
		chunk = v->GetChunk(source,iteration,block);

		if(chunk == NULL) {
			ERROR("Unknown block " << block << " for variable " << varname);
			errorOccured = true;
			return -2;
		}
		/*
		try {
			chunk = dynamic_cast<ChunkImpl*>(c);
		} catch(std::exception &e) {
			ERROR("When doing dynamic cast: " << e.what());
			errorOccured = true;
			return -3;
		}*/

		// create notification message
		Message message;
		message.source = process->getID();

		message.iteration = iteration;
		message.type = MSG_VAR;
		message.handle = chunk->GetHandle();
		message.object = v->GetID();
		// send message
		process->getSharedMessageQueue()->Send(&message);
		// free message
		DBG("Variable \"" << varname << "\" has been commited");

		// we don't need to keep the chunk in the client now,
		// so we erase it from the variable, but we don't erase the data.
		chunk->SetDataOwnership(false);
		v->DetachChunk(chunk);

		return 0;
	}

	int Client::set_position(const std::string& varname, 
		const int* position)
	{
		return set_block_position(varname,position,0);
	}

	int Client::set_block_position(const std::string& varname, 
		const int* position, int32_t block_id)
	{
		Variable* v = VariableManager::Search(varname);
		if(v == NULL) return -1;
		
		Position* p = v->GetBlockPosition(block_id);
		int dim = v->GetLayout()->GetDimensions();
		for(int i = 0; i < dim; i++) {
			p->SetOffset(i,position[i]);
		}
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
		if(errorOccured) return -4;

		if(block < (int32_t)0 || block >= (int32_t)Environment::NumDomainsPerClient())
		{
			ERROR("Invalid block ID");
			return -3;
		}

		/* check that the variable is know in the configuration */
		Variable* variable = VariableManager::Search(varname);

		if(variable == NULL) {
			ERROR("Unknown variable \"" << varname <<"\"");
			return -1;
		}

		Chunk* chunk = variable->Allocate(block,blocking);
		if(chunk == NULL) 
		{
			DBG("Unable to allocate chunk for variable \"" << varname << "\"");
			errorOccured = true;
			return -2;
		}
		int size = chunk->GetDataSpace()->MemCopy(data);

		// create message
		Message message;

		message.source = chunk->GetSource();
		message.iteration = chunk->GetIteration();
		message.object = variable->GetID();
		message.type = MSG_VAR;
		message.handle = chunk->GetHandle();

		// send message
		process->getSharedMessageQueue()->Send(&message);
		DBG("Variable \"" << varname << "\" has been written");
		
		chunk->SetDataOwnership(false);
		variable->DetachChunk(chunk);
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
				process->getSharedMessageQueue()->Send(&sig);
			} catch(interprocess_exception &e) {
				ERROR("Error while sending event \"" << 
						signal_name << "\", " << e.what());
				return -1;
			}
			DBG("Event \""<< signal_name << "\" has been sent");
			return 0;
		} catch (std::exception &e) {
			ERROR(e.what());
			return -1;
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
			process->getSharedMessageQueue()->Send(&kill);
			return 0;
		} else {
			WARN("Trying to send kill signal multiple times to the server");
			return -1;
		}
	}

	int Client::end_iteration()
	{
		int iteration = Environment::GetLastIteration();
		Environment::StartNextIteration();
		Message msg;
		msg.type = MSG_INT;
		msg.source = process->getID();
		msg.iteration = iteration;
	
		// do a reduction to know if any process had an error
		int has_error;
		MPI_Allreduce (&errorOccured, &has_error,1, 
				MPI_INT, MPI_BOR, Environment::GetEntityComm());
		if(not has_error)
			msg.object = END_ITERATION;
		else {
			msg.object = ITERATION_HAS_ERROR;
			errorOccured = false;
		}
		
		process->getSharedMessageQueue()->Send(&msg);
		return 0;
	}

	MPI_Comm Client::mpi_get_client_comm()
	{
		return Environment::GetEntityComm();
	}

	Client::~Client() 
	{
		Process::Kill();
		DBG("Client destroyed successfuly");
	}

}

