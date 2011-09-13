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
#include "server/Server.hpp"
#include "server/ServerConfiguration.hpp"
#include "common/Message.hpp"
#include "common/Layout.hpp"
#include "common/LayoutFactory.hpp"
#include "common/SharedMemory.hpp"

using namespace boost::interprocess;

Damaris::Server *server;

namespace Damaris {

	/* constructor for embedded mode */
	Server::Server(std::string* cf, int id)
	{
		ServerConfiguration::initialize(cf);
		config = ServerConfiguration::getInstance();
		//env = new Environment();
		//env->setID(id);
		config->setServerID(id);
		init();
	}
	
	/* constructor for standalone mode */
	Server::Server(ServerConfiguration* c, Environment *e)
	{
		config = c;
		env = e;
		config->setServerID(env->getID());
		init();
	}

	/* initialization */
	void Server::init() 
	{
		needStop = config->getClientsPerNode();
		/* creating shared structures */
		try {
			if(config->getSharedMemoryType() == "sysv") {
			
				SharedMessageQueue::remove(sysv_shmem,config->getMsgQueueName()->c_str());
				SharedMemorySegment::remove(sysv_shmem,config->getSegmentName()->c_str());	
				
				msgQueue = SharedMessageQueue::create(sysv_shmem,
							      config->getMsgQueueName()->c_str(),
							      (size_t)config->getMsgQueueSize(),
							      sizeof(Message));
				segment = SharedMemorySegment::create(sysv_shmem,
								config->getSegmentName()->c_str(),
								(size_t)config->getSegmentSize());

			} else {
			
				SharedMessageQueue::remove(posix_shmem,config->getMsgQueueName()->c_str());
				SharedMemorySegment::remove(posix_shmem,config->getSegmentName()->c_str());      
                        
				msgQueue = SharedMessageQueue::create(posix_shmem,
								config->getMsgQueueName()->c_str(),
								(size_t)config->getMsgQueueSize(),
								sizeof(Message));
                        
				segment = SharedMemorySegment::create(posix_shmem,
								config->getSegmentName()->c_str(),
								(size_t)config->getSegmentSize());
			}
		}
		catch(interprocess_exception &ex) {
			ERROR("Error when initializing the server: " << ex.what());
			exit(-1);
		}

		metadataManager = new MetadataManager(segment);
		actionsManager = config->getActionsManager();
		INFO("Server successfully started with configuration " << config->getFileName()->c_str());
	}
	
	/* destructor */
	Server::~Server()
	{
		if(config->getSharedMemoryType() == "sysv") {
			SharedMessageQueue::remove(sysv_shmem,config->getMsgQueueName()->c_str());
			SharedMemorySegment::remove(sysv_shmem,config->getSegmentName()->c_str());
		} else {
			SharedMessageQueue::remove(posix_shmem,config->getMsgQueueName()->c_str());
			SharedMemorySegment::remove(posix_shmem,config->getSegmentName()->c_str());
		}
		delete msgQueue;
		delete segment;
		delete metadataManager;
		ServerConfiguration::finalize();
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
			
		std::string name(msg->content);
		int32_t iteration = msg->iteration;
		int32_t sourceID = msg->sourceID;
		Layout* layout = NULL;
		void* data = NULL;
		
		if(msg->type == MSG_VAR)
		{
			DBG("Received notification for variable " << name.c_str()); 
			data = segment->getAddressFromHandle(msg->handle);
			layout = LayoutFactory::unserialize(msg->layoutInfo);
			Variable v(name,iteration,sourceID,layout,data);
			metadataManager->put(v);
			return;
		}
		
		if(msg->type == MSG_SIG) 
		{
			DBG("Received event " << msg->content);
			if(msg->content[0] == '#') {
				if(name == "#kill") needStop -= 1;
			} else {
				actionsManager->reactToUserSignal(&name,iteration,sourceID,metadataManager);		
			}
			return;
		}
	}
	
	/* indicate that the server should stop */
	void Server::stop()
	{
		needStop = 0;
	}
	
}

