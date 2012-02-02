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
 * \file Client.hpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 * 
 * Client.hpp is the main file to be included in a C++ 
 * client in order to access the main client-side functions
 * of Damaris.
 */
#ifndef __DAMARIS_CLIENT_H
#define __DAMARIS_CLIENT_H

#include <string>
#include <vector>
#include <stdint.h>

#include "common/Writer.hpp"
#include "common/Process.hpp"
#include "common/Environment.hpp"
#include "common/Configuration.hpp"
#include "common/SharedMessageQueue.hpp"
#include "common/SharedMemorySegment.hpp"
#include "common/ActionsManager.hpp"
#include "common/MetadataManager.hpp"
#include "common/ChunkHandle.hpp"
#include "common/Layout.hpp"

/**
 * \namespace Damaris
 * All classes related to Damaris are defined under the Damaris namespace.
 */
namespace Damaris {

/**
 * \class Client
 * The Client object represents a single core running the
 * simulation. It is characterized by an ID and is initialized
 * with the name of an XML configuration file.
 */
class Client : public Writer {

	private:
		Process* process;

	public:
		/** 
		 * \brief Constructor.
		 * Initializes the client given the name of a configuration file and an ID. 
		 * Damaris won't check if two clients have the same ID so the user shoud be
		 * careful with that.
		 *
		 * \param[in] config : name of an XML configuration file.
		 * \param[in] id : id of the client (should be unique).
		 */
		Client(Process* p);

		static Client* New(const std::string& file, int32_t id);

		/**
		 * \see Writer::write
		 */
		virtual int write(const std::string & varname, int32_t iteration, const void* data);
		
		/**
		 * \see Writer::chunk_write
		 */
		virtual int chunk_write(chunk_h chunkh, const std::string & varname, 
			int32_t iteration, const void* data);		

		/**
		 * \see Writer::signal
		 */
		virtual int signal(const std::string & signame, int32_t iteration);

		/**
		 * \see Writer::alloc
		 */
		virtual void* alloc(const std::string & varname, int32_t iteration);

		/** 
		 * \see Writer::commit
		 */
		virtual int commit(const std::string & varname, int32_t iteration);

		/**
		 * \see Writer::chunk_set
		 */
		virtual chunk_h chunk_set(unsigned int dimensions,
					const std::vector<int> & startIndices, 
					const std::vector<int> & endIndices);

		/**
		 * \see Writer::chunk_free
		 */
		virtual void chunk_free(chunk_h chunkh);

		/** 
		 * \brief Retrieves a parameter's value. Not implemented yet.
		 * 
		 * \param[in] paramName : name of the parameter to retrieve.
		 * \param[out] buffer : pointer to the memory where to copy the parameter's value.
		 *
		 * \return 0 in case of success, -1 if the parameter is not found.
		 */
		virtual int get_parameter(const std::string & paramName, void* buffer);
		
		/**
		 * Sends a signal to the server to shut it down (all clients in node need
		 * to call this function before the server is actually killed.
		 * \return 0 in case of success, -1 in case of failure.
		 */
		virtual int kill_server();

		/**
		 * Gets the communicator gathering clients.
		 */
		virtual MPI_Comm get_clients_communicator();
		
		/**
		 * \brief Destructor.
		 * To be called at the end of the before stopping the client program.
		 */
		virtual ~Client();
}; // class Client

} // namespace Damaris

#endif
