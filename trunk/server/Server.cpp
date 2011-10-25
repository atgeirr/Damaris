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
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#include <iostream>
#include <list>
#include "common/Debug.hpp"
#include "common/Environment.hpp"
#include "common/SharedMemorySegment.hpp"
#include "common/Configuration.hpp"
#include "server/Server.hpp"
#include "common/Message.hpp"
#include "common/ShmChunk.hpp"
#include "common/Layout.hpp"
#include "common/SharedMemory.hpp"

Damaris::Server *server;

namespace Damaris {

	/* constructor for embedded mode */
Server::Server(const std::string &cf, int id)
{
	std::auto_ptr<Model::simulation_mdl> 
		mdl(Model::simulation(cf.c_str(),xml_schema::flags::dont_validate));
	DBG("Model build successfuly from configuration file");		

	Configuration::initialize(mdl,cf);
	DBG("Configuration initialized successfuly");

	Environment::initialize(mdl,id);
	DBG("Environment initialized successfuly");

	config = Configuration::getInstance();
	env    = Environment::getInstance();

	init();
}

/* constructor for standalone mode */
Server::Server(Configuration* c, Environment *e)
{
	config = c;
	env    = e;
	
	init();
}

/* initialization */
void Server::init() 
{
	needStop = config->getClientsPerNode();
	/* creating shared structures */
	DBG("Starting creation of shared structures...");
	try {
#ifdef __SYSV	
		SharedMessageQueue::remove(sysv_shmem,config->getMsgQueueName()->c_str());
		SharedMemorySegment::remove(sysv_shmem,config->getSegmentName()->c_str());	
		
		msgQueue = SharedMessageQueue::create(sysv_shmem,
					      config->getMsgQueueName().c_str(),
					      (size_t)config->getMsgQueueSize(),
					      sizeof(Message));
		segment = SharedMemorySegment::create(sysv_shmem,
						config->getSegmentName().c_str(),
						(size_t)config->getSegmentSize());

#else
		SharedMessageQueue::remove(posix_shmem,config->getMsgQueueName().c_str());
		SharedMemorySegment::remove(posix_shmem,config->getSegmentName().c_str());      
		
		msgQueue = SharedMessageQueue::create(posix_shmem,
						config->getMsgQueueName().c_str(),
						(size_t)config->getMsgQueueSize(),
						sizeof(Message));
		DBG("Shared message queue created");
                       
		segment = SharedMemorySegment::create(posix_shmem,
						config->getSegmentName().c_str(),
						(size_t)config->getSegmentSize());
		DBG("Shared Segment created");
#endif
	}
	catch(interprocess_exception &ex) {
		ERROR("Error when initializing the server: " << ex.what());
		exit(-1);
	}

	metadataManager = config->getMetadataManager();
	ASSERT(metadataManager != NULL);
	DBG("Metadata manager created successfuly");

	actionsManager = config->getActionsManager();
	ASSERT(actionsManager != NULL);

	DBG("Actions manager created successfuly");

	INFO("Server successfully initialized with configuration " 
			<< config->getFileName());
}

/* destructor */
Server::~Server()
{
#ifdef __SYSV
	SharedMessageQueue::remove(sysv_shmem,config->getMsgQueueName().c_str());
	SharedMemorySegment::remove(sysv_shmem,config->getSegmentName().c_str());
#else
	SharedMessageQueue::remove(posix_shmem,config->getMsgQueueName().c_str());
	SharedMemorySegment::remove(posix_shmem,config->getSegmentName().c_str());
#endif
	delete msgQueue;
	delete segment;
		
	Configuration::finalize();
	Environment::finalize();
}
	
/* starts the server and enter the main loop */
int Server::run()
{
	INFO("Successfully entered in \"run\" mode");
	
	Message *msg = new Message();
	unsigned int priority;
	size_t  recvSize;
	bool received;
	
	while(needStop > 0) {
		received = msgQueue->tryReceive(msg,sizeof(Message), recvSize, priority);
		if(received) {
			DBG("Received a	message of type " << msg->type);
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
		ShmChunk* chunk = new ShmChunk(segment,handle); 
		Variable* v = metadataManager->getVariable(object);
		if(v != NULL) v->attachChunk(chunk);
		else {
			// the variable is unknown, discarde it
			ERROR("Server received a chunk " 
				<< "for an unknown variable, discarding");
			chunk->remove();
			delete chunk;
		}
		return;
	}
	
	if(msg->type == MSG_SIG) 
	{
		actionsManager->reactToUserSignal(object,
				iteration,source,metadataManager);
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
		needStop--; break;
	}
}

/* indicate that the server should stop */
void Server::stop()
{
	needStop = 0;
}


Client* start_mpi_entity(const std::string& configFile, MPI_Comm* newcomm, int* newrank, int* newsize)
{
	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	std::auto_ptr<Model::simulation_mdl> 
		mdl(Model::simulation(configFile.c_str(),xml_schema::flags::dont_validate));

	Configuration::initialize(mdl,configFile);
	Configuration* config = Configuration::getInstance();

	int clpn = config->getClientsPerNode();
	int copn = config->getCoresPerNode();

	DBG("Starting MPI instances for " << clpn << " clients per core");
	int is_server = (rank % copn) < (copn - clpn);
	int is_client = not is_server;

	MPI_Comm_split(MPI_COMM_WORLD,is_client,rank,newcomm);
	MPI_Comm_rank(*newcomm,newrank);
	MPI_Comm_size(*newcomm,newsize);

	Environment::initialize(mdl,rank);
	Environment* env = Environment::getInstance();

	if(is_client) {
		DBG("Client starting, rank = " << rank);
		MPI_Barrier(MPI_COMM_WORLD);
		return new Client(config,env);
	} else {
		DBG("Server starting, rank = " << rank);
		Server server(config,env);
		MPI_Barrier(MPI_COMM_WORLD);
		server.run();
		return NULL;
	}
}	

}
