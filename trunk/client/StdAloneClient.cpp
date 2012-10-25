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
#include "core/Debug.hpp"
#include "memory/Message.hpp"
#include "data/ChunkImpl.hpp"
#include "data/Layout.hpp"
#include "core/VariableManager.hpp"
#include "core/ActionManager.hpp"
#include "client/StdAloneClient.hpp"
#ifdef __ENABLE_VISIT
#include "visit/VisItListener.hpp"
#endif

namespace Damaris {

	StdAloneClient::StdAloneClient(Process* p)
		: Client(p)
	{
		Environment::SetClient(true);
#ifdef __ENABLE_VISIT
		if(process->getModel()->visit().present()) {
			Viz::VisItListener::Init(Environment::getEntityComm(),
					p->getModel()->visit(),
					Environment::getSimulationName());
		}
#endif
	}

	int StdAloneClient::connect()
	{
		static bool connected = false;
		if(not connected) {
			Environment::AddConnectedClient(process->getID());
			connected = true;
			return true;
		}
		return false;
	}

	/* destructor */
	StdAloneClient::~StdAloneClient()
	{
	}

/*
	void* StdAloneClient::alloc(const std::string & varname, bool blocking)
	{
		// check that the variable is known in the configuration
		Variable* variable = VariableManager::Search(varname);

		if(variable == NULL) {
			ERROR("Variable \""<< varname
					<< "\" not defined in configuration");
			return NULL;
		}

		if((not variable->IsTimeVarying()) && 
			Environment::GetLastIteration() != 0) {
			WARN("Trying to write a non-time-varying variable at an iteration "
			<< "different from 0, there is a chance to generate inconsistencies");
		}

		// the variable is known, get its layout
		Layout* layout = variable->GetLayout();

		if(layout->IsUnlimited()) {
			ERROR("Trying to allocate memory for an unlimited layout");
			return NULL;
		}

		// initialize the chunk descriptor
		ChunkDescriptor* cd = ChunkDescriptor::New(*layout);

		// try allocating the required memory
		size_t size = sizeof(ChunkHeader)+cd->GetDataMemoryLength(layout->GetType());
		void* location = process->getSharedMemorySegment()->Allocate(size);

		// This piece of code changes from Client.cpp: we don't want to block
		// if there is no way to get more memory.
		if(location == NULL && blocking) {
			clean(iteration);
			location = process->getSharedMemorySegment()->Allocate(size);
		}
		if(location == NULL) {
			ERROR("Could not allocate memory: not enough available memory");
			ChunkDescriptor::Delete(cd);
			return NULL;
		}

		// create the chunk header in memory
		int source = process->getID();
		ChunkHeader* ch = new(location) ChunkHeader(cd,layout->GetType(),iteration,source);

		// create the ChunkImpl and attach it to the variable
		ChunkImpl* chunk = new ChunkImpl(process->getSharedMemorySegment(),ch);
		chunk->SetDataOwnership(true);
		variable->AttachChunk(chunk);

		ChunkDescriptor::Delete(cd);

		// return the pointer to data
		return chunk->Data();
	}
*/
	int StdAloneClient::commit_block(const std::string & varname, int32_t block, int32_t iteration)
	{		
		Variable* v = VariableManager::Search(varname);
		if(v == NULL)
			return -1;

		// get the pointer to the allocated chunk
		//ChunkIndexByIteration::iterator end;
		//ChunkIndexByIteration::iterator it = v->getChunksByIteration(iteration,end);
		int source = Process::get()->getID();
		Chunk* chunk = v->GetChunk(source,iteration,block);

		if(chunk == NULL)
			return -2;
		try {
			dynamic_cast<ChunkImpl*>(chunk);
		} catch(std::exception &e) {
			ERROR("When doing dynamic cast: " << e.what());
			return -3;
		}

		// nothing to do actually, the server already knows the variable

		DBG("Variable \"" << varname << "\" has been commited");

		return 0;
	}

	int StdAloneClient::commit(const std::string &varname, int32_t iteration)
	{
		return commit_block(varname,0,iteration);
	}

	int StdAloneClient::write(const std::string & varname, 
		const void* data, bool blocking)
	{
		return write_block(varname,0,data,blocking);
	}

	int StdAloneClient::write_block(const std::string &varname,
			int32_t block, const void* data,
			bool blocking)
	{
		/* check that the variable is know in the configuration */
		Variable* variable = VariableManager::Search(varname);

		if(variable == NULL) {
			return -1;
		}

		/*
		if((not variable->IsTimeVarying()) && iteration != 0) {
			WARN("Trying to write a non-time-varying variable at an iteration "
					<< "different from 0, the variable won't be written");
			return -4;
		}

		Layout* layout = variable->GetLayout();

		if(layout->IsUnlimited()) {
			ERROR("Trying to write a variable"
					<< " with an unlimited layout (use chunk_write instead)");
			return -3;
		}

		// initialize the chunk descriptor
		ChunkDescriptor* cd = ChunkDescriptor::New(*layout);

		// try allocating the required memory
		size_t size = sizeof(ChunkHeader)+cd->GetDataMemoryLength(layout->GetType());
		void* location = process->getSharedMemorySegment()->Allocate(size);

		// This piece of code changes from Client.cpp: we don't want to block
		// if there is no way to get more memory.
		if(location == NULL && blocking) {
			clean(iteration);
			location = process->getSharedMemorySegment()->Allocate(size);
		}
		if(location == NULL) {
			ERROR("Could not allocate memory: not enough available memory");
			ChunkDescriptor::Delete(cd);
			return -2;
		}
		// create the chunk header in memory
		int source = process->getID();
		ChunkHeader* ch = new(location) ChunkHeader(cd,layout->GetType(),
							iteration,source,block);

		// create the ChunkImpl and attach it to the variable
		ChunkImpl* chunk = new ChunkImpl(process->getSharedMemorySegment(),ch);
		chunk->SetDataOwnership(true);
		// copy data
		size = cd->GetDataMemoryLength(layout->GetType());
		memcpy(chunk->Data(),data,size);

		variable->AttachChunk(chunk);	

		DBG("Variable \"" << varname << "\" has been written");

		ChunkDescriptor::Delete(cd);
		*/

		Chunk* chunk = variable->Allocate(block,blocking);
		if(chunk == NULL)
		{
			ERROR("Unable to allocate chunk for variable \"" << varname << "\"");
			return -1;
		}
	
		int size = chunk->MemCopy(data);
		return size;
	}

	int StdAloneClient::chunk_write(chunk_h chunkh, const std::string & varname, 
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

		// This piece of code changes from Client.cpp: we don't want to block
		// if there is no way to get more memory.
		if(location == NULL && blocking) {
			clean();
			location = process->getSharedMemorySegment()->Allocate(size);
		}
		if(location == NULL) {
			ERROR("Could not allocate memory: not enough available memory");
			return -2;
		}

		// create the ChunkHeader
		int source = process->getID();
		ChunkHeader* ch = new(location) ChunkHeader(cd,layout->GetType(),iteration,source);

		// create the ChunkImpl object       
		ChunkImpl* chunk = new ChunkImpl(process->getSharedMemorySegment(),ch);
		chunk->SetDataOwnership(true);

		// copy data
		size = cd->GetDataMemoryLength(layout->GetType());
		memcpy(chunk->Data(),data,size);

		variable->AttachChunk(chunk);	
		DBG("Variable \"" << varname << "\" has been written");

		return size;
	}

	int StdAloneClient::signal(const std::string & signal_name)
	{
		Action* action = ActionManager::Search(signal_name);
		if(action == NULL) {
			DBG("Undefined action \"" << signal_name << "\"");
			return -2;
		}

		int iteration = Environment::GetLastIteration();
		action->Call(iteration,process->getID());

		DBG("Event \""<< signal_name << "\" has been sent");
		return 0;
	}

	int StdAloneClient::kill_server()
	{
		WARN("Synchronous server cannot be killed (you own your process, man!)");
		return -1;
	}

	int StdAloneClient::end_iteration()
	{
		Environment::StartNextIteration();
#ifdef __ENABLE_VISIT
		int vizstt;
		if(process->getModel()->visit().present()) {
			Viz::VisItListener::Update();
			// try receiving from VisIt (only for rank 0)

			if(process->getID() == 0) {
				vizstt = Viz::VisItListener::Connected();
				DBG("In end_iteration for process 0, stat is " << vizstt);
			}		
			MPI_Bcast(&vizstt,1,MPI_INT,0, Environment::getEntityComm());
			// try receiving from the VisIt callback communication layer
			if(vizstt > 0) {
				Viz::VisItListener::EnterSyncSection(vizstt);
			}
		}
#endif
		return 0;
	}

	int StdAloneClient::clean()
	{
		return signal("clean");
	}
}
