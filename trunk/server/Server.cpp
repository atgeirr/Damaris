#include <iostream>
#include <list>

//#include <boost/interprocess/ipc/message_queue.hpp>
//#include <boost/interprocess/managed_shared_memory.hpp>

#include "common/Util.hpp"
#include "common/Configuration.hpp"
#include "common/Message.hpp"
#include "common/Layout.hpp"
#include "common/LayoutFactory.hpp"
#include "server/Server.hpp"

using namespace boost::interprocess;

namespace Damaris {
	
	Server::Server(std::string* configfile) 
	{
		config = new Configuration();
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
			behaviorManager = new BehaviorManager(metadataManager);
		}
		catch(interprocess_exception &ex) {
			LOGF("Error when initializing the server: %s\n",ex.what())
			exit(-1);
		}
		LOGF("Server successfully started with configuration %s\n",configfile->c_str())
	}
	
	Server::~Server()
	{
		shared_memory_object::remove(config->getMsgQueueName()->c_str());
		delete msgQueue;
		
		shared_memory_object::remove(config->getSegmentName()->c_str());
		delete segment;
		
		delete behaviorManager;
		delete metadataManager;
		
		delete config;
	}
	
	void Server::run()
	{
		LOG("Successfully entered in \"run\" mode\n")
		
		Message *msg = new Message();
		unsigned int priority;
		size_t  recvSize;
		bool c;
		
		while(!needStop) {
			c = msgQueue->try_receive(msg,sizeof(Message), recvSize, priority);
			if(c) {
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
			LOGF("receving variable \"%s\" step is %ld\n",msg->content,(long int)iteration)
			data = segment->get_address_from_handle(msg->handle);
			layout = LayoutFactory::unserialize(msg->layoutInfo);
			metadataManager->put(&name,iteration,sourceID,layout,data);
			return;
		}
		
		if(msg->type == MSG_POKE) 
		{
			//database->pretty_print();
			behaviorManager->reactToPoke(&name,iteration,sourceID);		
			return;
		}
	}
	
	void Server::stop()
	{
		needStop = true;
	}
	
}
