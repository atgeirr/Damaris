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
 * \file Server.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#include <iostream>
#include <list>
#include "core/Debug.hpp"
#include "core/Message.hpp"
#include "data/ShmChunk.hpp"
#include "data/Layout.hpp"
#include "core/ActionManager.hpp"
#include "core/VariableManager.hpp"
#include "server/Server.hpp"
#include "visit/VisItListener.hpp"

Damaris::Server *server;

namespace Damaris {

Server* Server::New(const std::string& cfgfile, int32_t id)
{
	Process::initialize(cfgfile,id);
	Process* p = Process::get();
	p->createSharedStructures();
	return new Server(p);
}

/* constructor for embedded mode */
Server::Server(Process* p)
{
	process = p;
	needStop = Environment::getClientsPerNode();
}


/* destructor */
Server::~Server()
{
	Process::kill();
}
	
/* starts the server and enter the main loop */
int Server::run()
{
	DBG("Successfully entered in \"run\" mode");

	if(process->getModel()->visit().present()) {
		Viz::VisItListener::Init(Environment::getEntityComm(),
			process->getModel()->visit(),
			Environment::getSimulationName());
	}
	
	Message msg;
	bool received;
	int vizstt;

	while(needStop > 0) {
		received = process->getSharedMessageQueue()->tryReceive(&msg,sizeof(Message));
		if(received) {
			DBG("Received a message of type " << msg.type
				<< " iteration is "<<msg.iteration
				<< " source is " <<msg.source);
			processMessage(msg);
		}
		
		if((vizstt = Viz::VisItListener::Connected()) > 0) {
			Viz::VisItListener::EnterSyncSection(vizstt);
		}
	}
	
	return 0;
}

/* process a incoming message */
void Server::processMessage(const Message& msg) 
{
	const int32_t& iteration 	= msg.iteration;
	const int32_t& source 	= msg.source;
	const int32_t& object 	= msg.object;
	const handle_t& handle	= msg.handle;
	
	if(msg.type == MSG_VAR)
	{
		try {
		ShmChunk* chunk = new ShmChunk(process->getSharedMemorySegment(),handle);
		Variable* v = VariableManager::Search(object);
		if(v != NULL) {
			v->attachChunk(chunk);
		} else {
			// the variable is unknown, discarde it
			ERROR("Server received a chunk " 
				<< "for an unknown variable, discarding");
			chunk->remove();
			delete chunk;
		}
		} catch(std::exception &e) {
			ERROR(e.what());
		}
		return;
	}
	
	if(msg.type == MSG_SIG)
	{
		ActionManager::reactToUserSignal(object,iteration,source);
		return;
	}

	if(msg.type == MSG_INT)
	{
		processInternalSignal(object,iteration,source);
	}
}

void Server::processInternalSignal(int32_t object, int iteration, int source)
{
	switch(object) {
	case CLIENT_CONNECTED:
		Environment::AddConnectedClient(source);
		break;
	case END_ITERATION:
		Environment::SetLastIteration(iteration);
		break;
	case KILL_SERVER:
		needStop--; // TODO: check that each client has sent the event instead of checking the number
		break;
	case URGENT_CLEAN:
		DBG("Received a \"clean\" message");
		ActionManager::reactToUserSignal("clean",iteration,source);
		break;
	case LOST_DATA:
		DBG("Received a \"lost data\" message");
		ActionManager::reactToUserSignal("lost",iteration,source);
		break;
	}
}

/* indicate that the server should stop */
void Server::stop()
{
	needStop = 0;
}

}
