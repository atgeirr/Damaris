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
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
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

#include "core/Writer.hpp"
#include "core/Process.hpp"
#include "data/Layout.hpp"

/**
 * \namespace Damaris
 * All classes related to Damaris are defined under the Damaris namespace.
 */
namespace Damaris {

class Initiator;
/**
 * \class Client
 * The Client object represents a single core running the
 * simulation. It is characterized by an ID and is initialized
 * with the name of an XML configuration file.
 */
class Client : public Writer {

	friend class Initiator;

	protected:
		Process* process; /*!< The Process object holding the ActionsManager,
				    MetadataManager, SharedMessageQueue, SharedMemorySegment */

		/** 
		 * \brief Constructor.
		 * Initializes the client given an already built Process object.
		 * This constructor is private, only the Initiator class can access it,
		 * as well as child classes.
		 *
		 * \param[in] p : pointer to a initialized Process object, required to be not-null.
		 */
		Client(Process* p);

	public:
		static Client* New(const std::string& file, int32_t id);

		/**
		 * \see Writer::write
		 */
		virtual int write(const std::string & varname, int32_t iteration, const void* data, bool blocking = false);
		
		/**
		 * \see Writer::chunk_write
		 */
		virtual int chunk_write(chunk_h chunkh, const std::string & varname, 
			int32_t iteration, const void* data, bool blocking = false);

		/**
		 * \see Writer::signal
		 */
		virtual int signal(const std::string & signame, int32_t iteration);

		/**
		 * \see Writer::alloc
		 */
		virtual void* alloc(const std::string & varname, int32_t iteration, bool blocking = false);

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
		 * Sends a signal to the server to try free the shared memory.
		 */
		virtual int clean(int iteration);

		/**
		 * Sends a signal to the server to notify that a client was
		 * unable to write some expected data.
		 */
		virtual int lost(int iteration);

		/**
		 * Gets the communicator gathering clients.
		 */
		virtual MPI_Comm mpi_get_client_comm();
		
		/**
		 * \brief Destructor.
		 * To be called at the end of the before stopping the client program.
		 */
		virtual ~Client();
}; // class Client

} // namespace Damaris

#endif
