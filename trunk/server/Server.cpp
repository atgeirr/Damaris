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

#include <iostream>
#include <list>

#include "common/Debug.hpp"
#include "common/Configuration.hpp"
#include "common/Message.hpp"
#include "common/Layout.hpp"
#include "common/LayoutFactory.hpp"
#include "server/Server.hpp"

using namespace boost::interprocess;

namespace Damaris {

	Server::Server(std::string* cf, int id)
	{
		config = new Configuration(cf,id);
		init();
	}
	
	Server::Server(Configuration* c)
	{
		config = c;
		init();
	}

	void Server::init() 
	{
		needStop = false;
		
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
			
			metadataManager = new MetadataManager(segment);
			actionsManager = new ActionsManager(metadataManager);
		}
		catch(interprocess_exception &ex) {
			ERROR("Error when initializing the server: " << ex.what());
			exit(-1);
		}
		INFO("Server successfully started with configuration " << config->getFileName()->c_str());
	}
	
	Server::~Server()
	{
		shared_memory_object::remove(config->getMsgQueueName()->c_str());
		delete msgQueue;
		
		shared_memory_object::remove(config->getSegmentName()->c_str());
		delete segment;
		
		delete actionsManager;
		delete metadataManager;
		delete config;
	}
	
	void Server::run()
	{
		INFO("Successfully entered in \"run\" mode");
		
		Message *msg = new Message();
		unsigned int priority;
		size_t  recvSize;
		bool received;
		
		while(!needStop) {
			received = msgQueue->try_receive(msg,sizeof(Message), recvSize, priority);
			if(received) {
				processMessage(msg);
			}
		}
		
		delete msg;
	}
	
	void Server::processMessage(Message* msg) 
	{
		
		std::string name(msg->content);
		int32_t iteration = msg->iteration;
		int32_t sourceID = msg->sourceID;
		Layout* layout = NULL;
		void* data = NULL;
		
		if(msg->type == MSG_VAR)
		{ 
			//LOGF("receving variable \"%s\" step is %ld\n",msg->content,(long int)iteration)
			data = segment->get_address_from_handle(msg->handle);
			layout = LayoutFactory::unserialize(msg->layoutInfo);
			metadataManager->put(&name,iteration,sourceID,layout,data);
			return;
		}
		
		if(msg->type == MSG_SIG) 
		{
			//database->pretty_print();
			actionsManager->reactToSignal(&name,iteration,sourceID);		
			return;
		}
	}
	
	void Server::stop()
	{
		needStop = true;
	}
	
}
