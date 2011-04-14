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
