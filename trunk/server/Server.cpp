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
#include "data/ChunkImpl.hpp"
#include "data/Layout.hpp"
#include "core/ActionManager.hpp"
#include "core/VariableManager.hpp"
#include "server/Server.hpp"
#include "comm/MPILayer.hpp"
#include "comm/RPC.hpp"
#ifdef __ENABLE_VISIT
#include "visit/VisItListener.hpp"
#endif

Damaris::Server *__server = NULL;

namespace Damaris {

Server* Server::New(std::auto_ptr<Model::Simulation> mdl, int32_t id)
{
	Process::Init(mdl,id);
	Process* p = Process::Get();
	p->createSharedStructures();
	Environment::SetClient(false);
	return new Server(p);
}

/* constructor for embedded mode */
Server::Server(Process* p)
{
	process = p;
	commLayer = NULL;
	rpcLayer = NULL;
	needStop = Environment::ClientsPerNode();
}


/* destructor */
Server::~Server()
{
	Process::Kill();
	DBG("Process killed successfuly");
	MPILayer<int>::Delete(commLayer);
	DBG("VisIt MPI layer deleted successfuly");
}
	
/* starts the server and enter the main loop */
int Server::run()
{
	DBG("Successfully entered in \"run\" mode");
	commLayer = MPILayer<int>::New(Environment::GetEntityComm());
	rpcLayer  = CollectiveRPC<void (*)(void)>::New(commLayer);

#ifdef __ENABLE_VISIT

	void (*f)(void) = &Viz::VisItListener::EnterSyncSection;
	rpcLayer->RegisterMulti(f,(int)RPC_VISIT_CONNECTED);

	void (*g)(void) = &Viz::VisItListener::Update;
	rpcLayer->RegisterCollective(g,(int)RPC_VISIT_UPDATE);

	// initializing environment
	if(process->getModel()->visit().present()) {
		Viz::VisItListener::Init(Environment::GetEntityComm(),
			process->getModel()->visit(),
		Environment::SimulationName());
	}
#endif
	
	Message msg;
	bool received;

	while(needStop > 0) {
		// try receiving from the shared message queue
		received = process->getSharedMessageQueue()->TryReceive(&msg,sizeof(Message));
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
				if(Viz::VisItListener::Connected()) {
					rpcLayer->Call(RPC_VISIT_CONNECTED);
				}
			}
		}
#endif
		// update the RPC layer
		rpcLayer->Update();
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
		Variable* v = VariableManager::Search(object);
		if(v != NULL) {
			DBG("Retrieving data for variable " << v->GetName() << " at iteration "
			<< iteration << " from source " << source);
			v->Retrieve(handle);
		} else {
			// the variable is unknown, we are f....
			ERROR("Server received data for an unknown variable entry."
			<< " This is a very uncommon error considering all the"
			<< " client-side checking. "
			<< " Be sure that from now on, your program will not"
			<< " behave as expected and"
			<< " will be subject to possibly huge memory leaks.");
		}
		} catch(std::exception &e) {
			ERROR(e.what());
		}
		return;
	}
	
	if(msg.type == MSG_SIG)
	{
		ActionManager::ReactToUserSignal(object,iteration,source);
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
		if(Environment::StartNextIteration()) {
#ifdef __ENABLE_VISIT
			rpcLayer->Call(RPC_VISIT_UPDATE);
#endif
		}
		break;
	case ITERATION_HAS_ERROR:
		if(Environment::StartNextIteration()) {
			ActionManager::ReactToUserSignal("#error",iteration,source);
		}
		break;
	case KILL_SERVER:
		needStop--; 
		// TODO: check that each client has sent 
		// the event instead of checking the number
		break;
	}
}

/* indicate that the server should stop */
void Server::stop()
{
	needStop = 0;
}

}
