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
#include "common/Debug.hpp"
#include "common/Message.hpp"
#include "common/ShmChunk.hpp"
#include "common/Layout.hpp"
#include "server/Server.hpp"
#include "viz/VisItListener.hpp"

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
	needStop = process->getEnvironment()->getClientsPerNode();
	if(p->getModel()->visit().present()) {
		Viz::VisItListener::init(p->getEnvironment()->getEntityComm(),
		p->getModel()->visit().get(),
		p->getEnvironment()->getSimulationName());
	}
}


/* destructor */
Server::~Server()
{
	DBG("In ~Server()");
	Process::kill();
	DBG("After Process killed");
}
	
/* starts the server and enter the main loop */
int Server::run()
{
	DBG("Successfully entered in \"run\" mode");
	
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
		
		if((vizstt = Viz::VisItListener::connected()) > 0) {
			Viz::VisItListener::enterSyncSection(vizstt);
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
		Variable* v = process->getMetadataManager()->getVariable(object);
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
		process->getActionsManager()->reactToUserSignal(object,iteration,source);
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
	case KILL_SERVER:
		needStop--; // TODO: check that each client has sent the event instead of checking the number
		break;
	case URGENT_CLEAN:
		DBG("Received a \"clean\" message");
		process->getActionsManager()->reactToUserSignal("clean",iteration,source);
		break;
	case LOST_DATA:
		DBG("Received a \"lost data\" message");
		process->getActionsManager()->reactToUserSignal("lost",iteration,source);
		break;
	}
}

/* indicate that the server should stop */
void Server::stop()
{
	needStop = 0;
}

}
