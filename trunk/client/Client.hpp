#ifndef __DAMARIS_CLIENT_H
#define __DAMARIS_CLIENT_H

#include <string>
#include <stdint.h>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>

#include "common/Configuration.hpp"
#include "common/Layout.hpp"

using namespace boost::interprocess;

namespace Damaris {

	class Client {
	private:
		int id;
		Configuration *config;
		message_queue *msgQueue;
		managed_shared_memory *segment;
		
	public:
		Client(std::string* config,int32_t id);
		int write(std::string* varname, int32_t iteration, const void* data, const Layout* layout);
		int signal(std::string* signame, int32_t iteration);
		void* alloc(std::string* varname, int32_t iteration, const Layout* layout);
		int commit(std::string* varname, int32_t iteration);
		~Client();
	}; // class Client

} // namespace Damaris

#endif
