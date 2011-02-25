#ifndef __DAMARIS_SERVER_H
#define __DAMARIS_SERVER_H

#include <string>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>

#include "common/Configuration.hpp"
#include "common/Message.hpp"
#include "common/BehaviorManager.hpp"
#include "common/MetadataManager.hpp"

using namespace boost::interprocess;

namespace Damaris {
	
	class Server {
	public:
		Server(std::string *config);
		~Server();
		void run();
		void stop();
	private:
		bool needStop;
		Configuration *config;
		message_queue *msgQueue;
		managed_shared_memory *segment;
		MetadataManager *metadataManager;
		BehaviorManager *behaviorManager;
		
		void processMessage(Message* msg);
	}; // class Server
	
	
} // namespace Damaris

#endif
