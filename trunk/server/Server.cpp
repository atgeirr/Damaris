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

Server* Server::New(const std::string& cfgfile, int32_t id)
{
	Process* p = new Process(cfgfile,id);
	p->createSharedStructures();
	return new Server(p);
}

/* constructor for embedded mode */
Server::Server(Process* p)
{
	process = p;
	needStop = process->getEnvironment()->getClientsPerNode();
/*
	try {
		std::auto_ptr<Model::SimulationModel> 
			mdl(Model::simulation(cf.c_str(),xml_schema::flags::dont_validate));

		DBG("Model build successfuly from configuration file");		

		config = Configuration::getInstance();
		config->initialize(mdl,cf);
		DBG("Configuration initialized successfuly");

		env = config->getEnvironment();
		env->setID(id);
		DBG("Environment initialized successfuly");
	
		init();

	} catch(xml_schema::exception &e) {
                ERROR(e.what());
                exit(-1);
        }
*/
}

/* constructor for standalone mode */
/*Server::Server(Configuration* c)
{
	config = c;
	env    = config->getEnvironment();
	init();
}*/

/* initialization */
/*void Server::init() 
{
	needStop = environment->getClientsPerNode();
	// creating shared structures 
	DBG("Starting creation of shared structures...");
	try {
#ifdef __SYSV	
		SharedMessageQueue::remove(sysv_shmem,config->getMsgQueueName()->c_str());
		SharedMemorySegment::remove(sysv_shmem,config->getSegmentName()->c_str());	
		
		msgQueue = SharedMessageQueue::create(sysv_shmem,
					      env->getMsgQueueName().c_str(),
					      (size_t)env->getMsgQueueSize(),
					      sizeof(Message));
		segment = SharedMemorySegment::create(sysv_shmem,
						env->getSegmentName().c_str(),
						(size_t)entv->getSegmentSize());

#else
		SharedMessageQueue::remove(posix_shmem,env->getMsgQueueName().c_str());
		SharedMemorySegment::remove(posix_shmem,env->getSegmentName().c_str());      
		
		msgQueue = SharedMessageQueue::create(posix_shmem,
						env->getMsgQueueName().c_str(),
						(size_t)env->getMsgQueueSize(),
						sizeof(Message));
		DBG("Shared message queue created");
                       
		segment = SharedMemorySegment::create(posix_shmem,
						env->getSegmentName().c_str(),
						(size_t)env->getSegmentSize());
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
*/

/* destructor */
Server::~Server()
{
	delete process;
/*
#ifdef __SYSV
	SharedMessageQueue::remove(sysv_shmem,env->getMsgQueueName().c_str());
	SharedMemorySegment::remove(sysv_shmem,env->getSegmentName().c_str());
#else
	SharedMessageQueue::remove(posix_shmem,env->getMsgQueueName().c_str());
	SharedMemorySegment::remove(posix_shmem,env->getSegmentName().c_str());
#endif
*/
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

/**

void* Server::alloc(const std::string & varname, int32_t iteration)
	{
		// this is basically the same code than in Client, we copy it here
		//  to be sure that any modification on Client won't affect it. 

		// check that the variable is known in the configuration 
		Variable* variable = metadataManager->getVariable(varname);

        	if(variable == NULL) {
			ERROR("Variable \""<< varname 
				<< "\" not defined in configuration");
			return NULL;
        	}

		// the variable is known, get its layout
		Layout* layout = variable->getLayout();

		// prepare variable to initialize a chunk
		std::vector<int> si(layout->getDimensions()),ei(layout->getDimensions());
		for(unsigned int i=0; i < layout->getDimensions(); i++)	{
			ei[i] = layout->getExtentAlongDimension(i)-1;
			si[i] = 0;
		}

		// try initializing the chunk in shared memory
		try {
			ShmChunk* chunk = 
				new ShmChunk(segment,layout->getType(),
						layout->getDimensions(),si,ei);
			chunk->setSource(env->getID());
			chunk->setIteration(iteration);
			variable->attachChunk(chunk);
			// chunk initialized, returns the data! 
			return chunk->data();

		} catch (...) {
			ERROR("While allocating \"" << varname 
				<< "\", allocation failed");
		}
		// on failure, returns NULL 
		return NULL;
	}
	
	int Server::commit(const std::string & varname, int32_t iteration)
	{		
		Variable* v = metadataManager->getVariable(varname);
		if(v == NULL)
			return -1;

		ShmChunk* chunk = NULL;
		// get the pointer to the allocated chunk
		ChunkIndexByIteration::iterator end;
		ChunkIndexByIteration::iterator it = v->getChunksByIteration(iteration,end);

		if(it == end)
			return -2;
		try {
			chunk = dynamic_cast<ShmChunk*>(it->get());
		} catch(std::exception &e) {
			ERROR("When doing dynamic cast: " << e.what());
			return -3;
		}
	
		// nothing to do actually, the server already knows the variable
			
                // free message
		DBG("Variable \"" << varname << "\" has been commited");

		return 0;
	}
	
	int Server::write(const std::string & varname, int32_t iteration, const void* data)
	{
		// check that the variable is known in the configuration 
		Variable* variable = metadataManager->getVariable(varname);

        	if(variable == NULL) {
			ERROR("Variable \""<< varname 
				<< "\" not defined in configuration");
			return -1;
        	}

		Layout* layout = variable->getLayout();

		std::vector<int> si(layout->getDimensions()),ei(layout->getDimensions());
                for(unsigned int i=0; i < layout->getDimensions(); i++) {
                        ei[i] = layout->getExtentAlongDimension(i)-1;
                        si[i] = 0;
                }

		ShmChunk* chunk = NULL;
                try {
                        chunk = new ShmChunk(segment,layout->getType(),
						layout->getDimensions(),si,ei);
                        chunk->setSource(env->getID());
                        chunk->setIteration(iteration);
                } catch (...) {
                        ERROR("While writing \"" << varname << "\", allocation failed");
                	return -2;
		}

		// copy data
		size_t size = chunk->getDataMemoryLength();
		memcpy(chunk->data(),data,size);
	
		variable->attachChunk(chunk);	
		
		// send message
		DBG("Variable \"" << varname << "\" has been written");
	
		return size;
	}

	int Server::chunk_write(chunk_h chunkh, const std::string & varname, 
			int32_t iteration, const void* data)
	{
		// check that the variable is know in the configuration 
		Variable* variable = metadataManager->getVariable(varname);

        	if(variable == NULL) {
			ERROR("Variable \""<< varname << "\" not defined in configuration");
			return -1;
        	}

		ChunkHandle* chunkHandle = (ChunkHandle*)chunkh;

		// check if the chunk matches the layout boundaries 
		Layout* layout = variable->getLayout();
		if(not chunkHandle->within(layout)) {
			ERROR("Chunk boundaries do not match variable's layout");
			return -3;
		}

		ShmChunk* chunk = NULL;
                try {
			Types::basic_type_e t = layout->getType();
			unsigned int d = chunkHandle->getDimensions();
			std::vector<int> si(d);
			std::vector<int> ei(d);

			for(unsigned int i=0;i<d; i++) {
				si[i] = chunkHandle->getStartIndex(i);
				ei[i] = chunkHandle->getEndIndex(i);
			}

                        chunk = new ShmChunk(segment,t,d,si,ei);
                        chunk->setSource(env->getID());
                        chunk->setIteration(iteration);
                } catch (...) {
                        ERROR("While writing \"" << varname << "\", allocation failed");
                	return -2;
		}

		// copy data
		size_t size = chunk->getDataMemoryLength();
		memcpy(chunk->data(),data,size);
		
		variable->attachChunk(chunk);	
		DBG("Variable \"" << varname << "\" has been written");
	
		return size;
	}

	int Server::signal(const std::string & signal_name, int32_t iteration)
	{
		Action* action = actionsManager->getAction(signal_name);
		if(action == NULL) {
			DBG("Undefined action \"" << signal_name << "\"");
			return -2;
		}

		action->call(iteration,env->getID());

		DBG("Event \""<< signal_name << "\" has been sent");
		return 0;
	}

	int Server::kill_server()
	{
		WARN("Synchronous server cannot be killed (you own your process, man!)");
		return -1;
	}
*/

#ifdef __ENABLE_MPI
Client* start_mpi_entity(const std::string& configFile, MPI_Comm globalcomm)
{
	/* Global rank and size in the passed communicator */
	int size, rank;
	MPI_Comm_size(globalcomm,&size);
	MPI_Comm_rank(globalcomm,&rank);

/*
	std::auto_ptr<Model::SimulationModel> mdl;
	try { 
		mdl(Model::simulation(configFile.c_str(),xml_schema::flags::dont_validate));
	} catch (xml_schema::exception &e) {
		ERROR(e.what());
		MPI_Abort(MPI_COMM_WORLD,-1);
	}
	int clpn = mdl->architecture().cores().clients().count();
        int copn = mdl->architecture().cores().count();
*/
	Process* p = new Process(configFile,rank);
	
	Environment* env = p->getEnvironment();
	env->setGlobalComm(globalcomm);
	int clpn = env->getClientsPerNode();
	int copn = env->getCoresPerNode();

	/* The name of the processor is used to compute communicators */
	char procname[MPI_MAX_PROCESSOR_NAME];
	int len;
	MPI_Get_processor_name(procname,&len);

	/* Compute the node identifier from the name */
	uint64_t nhash = (uint64_t)(14695981039346656037ULL);
	uint64_t fnv =  ((uint64_t)1 << 40) + (1 << 8) + 0xb3;
	for(int i=0; i < len; i++) {
		uint64_t c = (uint64_t)(procname[i]);
		nhash = nhash xor c;
		nhash *= fnv;
	}

	/* Create a new communicator gathering processes of the same node */
	int color = ((int)nhash >= 0) ? (int)nhash : - ((int)nhash);
	MPI_Comm nodecomm;
	MPI_Comm_split(globalcomm,color,rank,&nodecomm);
	env->setNodeComm(nodecomm);
	
	/* Get the size and rank in the node */
	int rankInNode;
	int sizeOfNode;
	MPI_Comm_rank(nodecomm,&rankInNode);
	MPI_Comm_size(nodecomm,&sizeOfNode);
	
	if(copn - clpn > 1) {
		ERROR("The number of dedicated cores per node"
			<< " must be either 0 or 1 in this version. Aborting...");
		MPI_Abort(MPI_COMM_WORLD,-1);
	}

	/* Check that the values match */
	if(sizeOfNode != copn) {
		ERROR("The number of cores detected in node does not match the number" 
			<< " provided in configuration."
			<< " This may be due to a configuration error or a (unprobable)"
			<< " hash colision in the algorithm. Aborting...");
		MPI_Abort(MPI_COMM_WORLD,-1);
	}

	/* Compute the communcator for clients and servers */
	int is_client = (rankInNode >= clpn) ? 0 : 1;
	MPI_Comm entitycomm;
	MPI_Comm_split(globalcomm,is_client,rank,&entitycomm);
	env->setEntityComm(entitycomm);
	
	/* Get rank and size in the entity communicator */
	int rankInEnComm, sizeOfEnComm;
	MPI_Comm_rank(entitycomm,&rankInEnComm);
	MPI_Comm_size(entitycomm,&sizeOfEnComm);

	if(not (copn == clpn)) {
		// dedicated core mode : the number of servers to create is strictly positive
		if(is_client) {
			DBG("Client starting, rank = " << rank);
			// the following barrier ensures that the client
			// won't be created before the servers are started.
			MPI_Barrier(globalcomm);
			p->openSharedStructures();
			return new Client(p);
		} else {
			DBG("Server starting, rank = " << rank);
			p->createSharedStructures();
			Server server(p);
			MPI_Barrier(globalcomm);
			server.run();
			return NULL;
		}
	} else {
		// synchronous mode : the servers are attached to each client
/*
		if(rankInNode != 0) {
			MPI_Barrier(globalcomm);
			return new SecondaryServer(p);
		} else {
			Server* s = new Server(p);
			MPI_Barrier(globalcomm);
			return s;
		}
*/
		return NULL;
	}
}	
#endif

}
