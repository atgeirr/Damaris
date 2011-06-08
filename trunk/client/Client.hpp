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
/**
 * The Client object represent a single core running the
 * simulation. It is characterized by a uniq ID and is initialized
 * with a configuration file.
 */
class Client {
	private:
		int id; /* this is the ID of the client */
		Configuration *config; /* configuration object */
		message_queue *msgQueue; /* pointer to the message queue */
		managed_shared_memory *segment; /* pointer to the shared memory segment */
		
	public:
		/* constructor, initializes the client given the name
		   of a configuration file and and ID. No one will check
		   if two clients have the same ID so the user shoud be
		   careful with that. */
		Client(std::string* config,int32_t id);
		/* writes a variable into shared memory and sends a message notifying the write */
		int write(std::string* varname, int32_t iteration, const void* data);//, const Layout* layout);
		/* sends en event to the dedicated core */
		int signal(std::string* signame, int32_t iteration);
		/* allocate a buffer in shared memory so it can be written after */
		void* alloc(std::string* varname, int32_t iteration);//, const Layout* layout);
		/* notify the dedicated core that the previously allocated buffer has been written */
		int commit(std::string* varname, int32_t iteration);
		/* destructor */
		~Client();
}; // class Client

} // namespace Damaris

#endif
