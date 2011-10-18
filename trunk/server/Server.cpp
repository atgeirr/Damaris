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
 * \date July 2011
 * \author Matthieu Dorier
 * \version 0.1
 *
 * Contains the definition of functions for the Server object.
 */
#include <iostream>
#include <list>
#include "common/Debug.hpp"
#include "common/Environment.hpp"
#include "common/SharedMemorySegment.hpp"
#include "common/Configuration.hpp"
#include "server/Server.hpp"
//#include "server/ServerConfiguration.hpp"
#include "common/Message.hpp"
#include "common/ChunkHeader.hpp"
#include "common/Chunk.hpp"
#include "common/Layout.hpp"
//#include "common/LayoutFactory.hpp"
#include "common/SharedMemory.hpp"

using namespace boost::interprocess;

Damaris::Server *server;

namespace Damaris {

	/* constructor for embedded mode */
	Server::Server(std::string* cf, int id)
	{
		//Model::simulation_mdl* mdl = new simulation_mdl(cf->c_str(),xml_schema::flags::dont_validate);
		std::auto_ptr<Model::simulation_mdl> mdl(Model::simulation(cf->c_str(),xml_schema::flags::dont_validate));

		Configuration::initialize(mdl,cf);
		Environment::initialize(mdl,id);

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
		try {
#ifdef __SYSV	
			SharedMessageQueue::remove(sysv_shmem,config->getMsgQueueName()->c_str());
			SharedMemorySegment::remove(sysv_shmem,config->getSegmentName()->c_str());	
			
			msgQueue = SharedMessageQueue::create(sysv_shmem,
						      config->getMsgQueueName()->c_str(),
						      (size_t)config->getMsgQueueSize(),
						      sizeof(Message));
			segment = SharedMemorySegment::create(sysv_shmem,
							config->getSegmentName()->c_str(),
							(size_t)config->getSegmentSize());

#else
			SharedMessageQueue::remove(posix_shmem,config->getMsgQueueName()->c_str());
			SharedMemorySegment::remove(posix_shmem,config->getSegmentName()->c_str());      
			
			msgQueue = SharedMessageQueue::create(posix_shmem,
							config->getMsgQueueName()->c_str(),
							(size_t)config->getMsgQueueSize(),
							sizeof(Message));
                        
			segment = SharedMemorySegment::create(posix_shmem,
							config->getSegmentName()->c_str(),
							(size_t)config->getSegmentSize());
#endif
		}
		catch(interprocess_exception &ex) {
			ERROR("Error when initializing the server: " << ex.what());
			exit(-1);
		}

		metadataManager = config->getMetadataManager();
		actionsManager = config->getActionsManager();

		INFO("Server successfully initialized with configuration " << config->getFileName()->c_str());
	}
	
	/* destructor */
	Server::~Server()
	{
#ifdef __SYSV
		SharedMessageQueue::remove(sysv_shmem,config->getMsgQueueName()->c_str());
		SharedMemorySegment::remove(sysv_shmem,config->getSegmentName()->c_str());
#else
		SharedMessageQueue::remove(posix_shmem,config->getMsgQueueName()->c_str());
		SharedMemorySegment::remove(posix_shmem,config->getSegmentName()->c_str());
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
				processMessage(msg);
			}
		}
		//INFO("out of main loop");
		
		delete msg;
		return 0;
	}
	
	/* process a incoming message */
	void Server::processMessage(Message* msg) 
	{
			
		//std::string name(msg->content);
		int32_t iteration = msg->iteration;
		int32_t source = msg->source;
		char* data = NULL;
		
		if(msg->type == MSG_VAR)
		{
			//DBG("Received notification for variable " << name.c_str()); 
			data = (char*)segment->getAddressFromHandle(msg->handle);
			ChunkHeader *header = ChunkHeader::fromBuffer((void*)data);
			data = data + header.size();
			Chunk chunk(header,source,iteration,data);
			metadataManager->attachChunk(msg->object,chunk);
			//layout = LayoutFactory::unserialize(msg->layoutInfo);
			//Variable v(name,iteration,sourceID,layout,data);
			//metadataManager->put(v);
			return;
		}
		
		if(msg->type == MSG_SIG) 
		{
			//DBG("Received event " << msg->content);
		//	if(msg->content[0] == '#') {
		//		if(name == "#kill") needStop -= 1;
		//	} else {
		//		actionsManager->reactToUserSignal(&name,iteration,sourceID,metadataManager);		
		//	}
		//	return;
			actionsManager->reactToUserSignal(msg->object,iteration,source,metadataManager);
		}
	}
	
	/* indicate that the server should stop */
	void Server::stop()
	{
		needStop = 0;
	}
	
}

