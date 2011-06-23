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

#ifdef __ENABLE_FORTRAN
        #include "common/FCMangle.h"
#endif
#include "common/Debug.hpp"
#include "common/Environment.hpp"
#include "common/Configuration.hpp"
#include "common/Message.hpp"
#include "common/Layout.hpp"
#include "common/LayoutFactory.hpp"
extern "C" {
#include "server/Server.h"
}
#include "server/Server.hpp"

using namespace boost::interprocess;

namespace Damaris {

	/** constructor for embedded mode */
	Server::Server(std::string* cf, int id)
	{
		config = new Configuration(cf);
		env = new Environment();
		env->setID(id);
		init();
	}
	
	/** constructor for standalone mode */
	Server::Server(Configuration* c, Environment *e)
	{
		config = c;
		env = e;
		init();
	}
	/** initialization */
	void Server::init() 
	{
		needStop = false;
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
		delete config;
	}
	/* starts the server and enter the main loop */
	int Server::run()
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
			data = segment->get_address_from_handle(msg->handle);
			layout = LayoutFactory::unserialize(msg->layoutInfo);
			Variable v(name,iteration,sourceID,layout,data);
			metadataManager->put(v);
			return;
		}
		
		if(msg->type == MSG_SIG) 
		{
			actionsManager->reactToSignal(&name,iteration,sourceID,metadataManager);		
			return;
		}
	}
	
	/* indicate that the server should stop */
	void Server::stop()
	{
		needStop = true;
	}
	
}

/* ====================================================================== 
 C Binding
 ====================================================================== */
extern "C" {

	Damaris::Server *server;
	/* Starts an embedded server inside the simulation,
	   the function will block until the server is asked to
	   stop. The server is initialized with a configuration
	   file and an id. */
	int DC_server(const char* configFile, int server_id)
	{
		std::string config_str(configFile);
		server = new Damaris::Server(&config_str,server_id);
		return server->run();
	}

#ifdef __ENABLE_FORTRAN
/* ====================================================================== 
 Fortran Binding
 ====================================================================== */
	void FC_FUNC_GLOBAL(df_server,DF_SERVER)
		(char* configFile_f, int32_t* server_id_f, int32_t* ierr_f, int32_t configFile_size)
	{
		std::string config_str(configFile_f,configFile_size);
		server = new Damaris::Server(&config_str,*server_id_f);
		*ierr_f = server->run();
	}
#endif
}
