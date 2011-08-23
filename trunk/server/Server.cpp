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
#include "server/ServerConfiguration.hpp"
#include "common/Message.hpp"
#include "common/Layout.hpp"
#include "common/LayoutFactory.hpp"
#include "server/Server.hpp"

using namespace boost::interprocess;

Damaris::Server *server;

namespace Damaris {

	/* constructor for embedded mode */
	Server::Server(std::string* cf, int id)
	{
		ServerConfiguration::initialize(cf);
		config = ServerConfiguration::getInstance();
		env = new Environment();
		env->setID(id);
		init();
	}
	
	/* constructor for standalone mode */
	Server::Server(ServerConfiguration* c, Environment *e)
	{
		config = c;
		env = e;
		init();
	}

	/* initialization */
	void Server::init() 
	{
		needStop = config->getClientsPerNode();
		/* creating shared structures */
		try {
			
			shared_memory_object::remove(config->getMsgQueueName()->c_str());
			shared_memory_object::remove(config->getSegmentName()->c_str());	
			
			msgQueue = new message_queue(create_only,
							config->getMsgQueueName()->c_str(),
							config->getMsgQueueSize(),
							sizeof(Message));
			
			segment = new managed_shared_memory(create_only,
								config->getSegmentName()->c_str(),
								config->getSegmentSize());
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
		shared_memory_object::remove(config->getMsgQueueName()->c_str());
		delete msgQueue;
		
		shared_memory_object::remove(config->getSegmentName()->c_str());
		delete segment;
		
		delete actionsManager;
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
		
		while(needStop != 0) {
			received = msgQueue->try_receive(msg,sizeof(Message), recvSize, priority);
			if(received) {
				processMessage(msg);
			}
		}
		
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
			data = segment->get_address_from_handle(msg->handle);
			layout = LayoutFactory::unserialize(msg->layoutInfo);
			Variable v(name,iteration,sourceID,layout,data);
			metadataManager->put(v);
			return;
		}
		
		if(msg->type == MSG_SIG) 
		{
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

