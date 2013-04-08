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
 * \date November 2012
 * \author Matthieu Dorier
 * \version 0.7
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
			Viz::VisItListener::Init(Environment::GetEntityComm(),
					p->getModel()->visit(),
					Environment::SimulationName());
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

	int StdAloneClient::commit_block(const std::string & varname, int32_t block, 
			int32_t iteration)
	{		
		Variable* v = VariableManager::Search(varname);
		if(v == NULL)
			return -2;

		int source = Process::Get()->getID();
		Chunk* chunk = v->GetChunk(source,iteration,block);

		if(chunk == NULL)
			return -1;
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

	int StdAloneClient::write(const std::string & varname, 
		const void* data, bool blocking)
	{
		return write_block(varname,0,data,blocking);
	}

	int StdAloneClient::write_block(const std::string &varname,
			int32_t block, const void* data,
			bool blocking)
	{
		if(block < (int32_t)0 || block >= (int32_t)Environment::NumDomainsPerClient())
		{
			ERROR("Invalid block ID");
			return -3;
		}

		/* check that the variable is know in the configuration */
		Variable* variable = VariableManager::Search(varname);

		if(variable == NULL) {
			return -1;
		}

		Chunk* chunk = variable->Allocate(block,blocking);
		if(chunk == NULL)
		{
			ERROR("Unable to allocate chunk for variable \"" << varname << "\"");
			return -2;
		}
		chunk->SetDataOwnership(true);
		int size = chunk->MemCopy(data);
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
		if(process->getModel()->visit().present()) {
			Viz::VisItListener::Update();
			// try receiving from VisIt (only for rank 0)
			if(process->getID() == 0) {
				Viz::VisItListener::Connected();
			}		
			// try receiving from the VisIt callback communication layer
			Viz::VisItListener::EnterSyncSection();
		}
#endif
		return 0;
	}
}
