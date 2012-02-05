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
#include "server/Server.hpp"
#include "common/Message.hpp"
#include "common/ShmChunk.hpp"
#include "common/Layout.hpp"

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
}


/* destructor */
Server::~Server()
{
	Process::kill();
}
	
/* starts the server and enter the main loop */
int Server::run()
{
	INFO("Successfully entered in \"run\" mode");
	
	Message *msg = new Message();
	bool received;
	
	while(needStop > 0) {
		received = true;
		process->getSharedMessageQueue()->receive(msg,sizeof(Message));
		if(received) {
			INFO("Received a message of type " << msg->type);
			processMessage(msg);
		}
	}
	
	delete msg;
	return 0;
}

/* process a incoming message */
void Server::processMessage(Message* msg) 
{
	int32_t& iteration 	= msg->iteration;
	int32_t& source 	= msg->source;
	int32_t& object 	= msg->object;
	handle_t& handle	= msg->handle;
	
	if(msg->type == MSG_VAR)
	{
		try {
		ShmChunk* chunk = new ShmChunk(process->getSharedMemorySegment(),handle);
		Variable* v = process->getMetadataManager()->getVariable(object);
		if(v != NULL) {
			INFO("A");
			v->attachChunk(chunk);
			INFO("B");
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
	
	if(msg->type == MSG_SIG)
	{
		process->getActionsManager()->reactToUserSignal(object,iteration,source);
		return;
	}

	if(msg->type == MSG_INT)
	{
		processInternalSignal(object);
	}
}

void Server::processInternalSignal(int32_t object)
{
	switch(object) {
	case KILL_SERVER:
		needStop--; 
		break;
	}
}

/* indicate that the server should stop */
void Server::stop()
{
	needStop = 0;
}

}
