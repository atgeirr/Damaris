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
#include "core/MPILayer.hpp"
#include "data/ChunkImpl.hpp"
#include "data/Layout.hpp"
#include "core/ActionManager.hpp"
#include "core/VariableManager.hpp"
#include "server/Server.hpp"
#ifdef __ENABLE_VISIT
#include "visit/VisItListener.hpp"
#endif

Damaris::Server *__server = NULL;

namespace Damaris {

Server* Server::New(const std::string& cfgfile, int32_t id)
{
	Process::initialize(cfgfile,id);
	Process* p = Process::get();
	p->createSharedStructures();
	Environment::SetClient(false);
	return new Server(p);
}

/* constructor for embedded mode */
Server::Server(Process* p)
{
	process = p;
#ifdef __ENABLE_VISIT
	visitMPIlayer = NULL;
#endif
	needStop = Environment::ClientsPerNode();
}


/* destructor */
Server::~Server()
{
	Process::kill();
	DBG("Process killed successfuly");
#ifdef __ENABLE_VISIT
	MPILayer<int>::Delete(visitMPIlayer);
	DBG("VisIt MPI layer deleted successfuly");
#endif
}
	
/* starts the server and enter the main loop */
int Server::run()
{
	DBG("Successfully entered in \"run\" mode");

#ifdef __ENABLE_VISIT
	int vizstt;
	if(process->getModel()->visit().present()) {
		Viz::VisItListener::Init(Environment::GetEntityComm(),
			process->getModel()->visit(),
		Environment::SimulationName());
		visitMPIlayer = MPILayer<int>::New(Environment::GetEntityComm());
	}
#endif
	
	Message msg;
	bool received;

	while(needStop > 0) {
		// try receiving from the shared message queue
		received = process->getSharedMessageQueue()->tryReceive(&msg,sizeof(Message));
		if(received) {
			DBG("Received a message of type " << msg.type
				<< " iteration is "<<msg.iteration
				<< " source is " <<msg.source);
			processMessage(msg);
		}

#ifdef __ENABLE_VISIT		
		if(process->getModel()->visit().present()) {
			// try receiving from VisIt (only for rank 0)
	
			if(process->getID() == 0) {
				if((vizstt = Viz::VisItListener::Connected()) > 0) {
					visitMPIlayer->bcast(&vizstt);
				}
			}

			// try receiving from the VisIt callback communication layer
			if(visitMPIlayer->deliver(&vizstt)) {
				Viz::VisItListener::EnterSyncSection(vizstt);
			}
		}
#endif
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
//		ChunkImpl* chunk = new ChunkImpl(process->getSharedMemorySegment(),handle);
//		chunk->SetDataOwnership(true);
		Variable* v = VariableManager::Search(object);
		if(v != NULL) {
			DBG("Retrieving data for variable " << v->GetName() << " at iteration "
			<< iteration << " from source " << source);
			v->Retrieve(handle);
		} else {
			// the variable is unknown, we are f....
			ERROR("Server received data for an unknown variable entry."
			<< " This is a very uncommon error considering all the client-side checking."
			<< " Be sure that from now on, your program will not behave as expected and"
			<< " will be subject to possibly huge memory leaks.");
			//delete chunk;
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

	static bool no_update = false;
	static bool global_no_update = false;

	switch(object) {
	case CLIENT_CONNECTED:
		Environment::AddConnectedClient(source);
		break;
	case END_ITERATION:
		if(Environment::StartNextIteration()) {
			MPI_Allreduce(&no_update,&global_no_update,1,
					MPI_BYTE,MPI_BOR, Environment::GetEntityComm());
			no_update = false;	
#ifdef __ENABLE_VISIT
			if(not global_no_update) Viz::VisItListener::Update();
#endif
		}
		break;
	case END_ITERATION_NO_UPDATE:
		no_update = true;
		if(Environment::StartNextIteration()) {	
			MPI_Allreduce(&no_update,&global_no_update,1,
					MPI_BYTE,MPI_BOR, Environment::GetEntityComm());
			no_update = false;
		}
		break;
	case KILL_SERVER:
		needStop--; // TODO: check that each client has sent the event instead of checking the number
		break;
	case URGENT_CLEAN:
		DBG("Received a \"clean\" message");
		ActionManager::reactToUserSignal("#clean",iteration,source);
		break;
	case LOST_DATA:
		DBG("Received a \"lost data\" message");
		ActionManager::reactToUserSignal("#lost",iteration,source);
		break;
	}
}

/* indicate that the server should stop */
void Server::stop()
{
	needStop = 0;
}

}
