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
 * The Client object represents a single core running the
 * simulation. It wraps a Processor object and exposes the set of
 * functions allowing a simulation to write data and send signals to
 * a dedicated core. This Client object is not supposed to work in
 * a standalone mode (when no core is dedicated). For this purpose, see
 * the StdAloneClient class.
 */
class Client {

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
		/**
		 * Constructor. This function creates a Client object. It must be used only
		 * when clients and servers are deployed separately. Moreover, this function
		 * sends a message to the dedicated core to notify that a new client is now
		 * connected.
		 * \param[in] file : name of the XML file to load.
		 * \param[in] id : id of the client. This id must be unique across all application clients.
		 */
		static Client* New(const std::string& file, int32_t id);


		/**
		 * Notifies a potential server that the client has connected.
		 */
		virtual int connect();

		/**
		 * \see Writer::write
		 */
		virtual int write(const std::string & varname, 
				  const void* data, bool blocking = false);
	
		/**
		 * \see Writer::write_block
		 */
		virtual int write_block(const std::string &varname,
				int32_t block, const void* data,
				bool blocking = false);
	
		/**
		 * \see Writer::chunk_write
		 * \deprecated
		 */
		virtual int chunk_write(chunk_h chunkh, const std::string & varname, 
			int32_t iteration, const void* data, bool blocking = false)
		__attribute__ ((deprecated));

		/**
		 * \see Writer::signal
		 */
		virtual int signal(const std::string & signame);

		/**
		 * \see Writer::alloc
		 */
		virtual void* alloc(const std::string & varname, 
				    bool blocking = true);

		/**
		 * \see Writer::alloc_block
		 */
		virtual void* alloc_block(const std::string & varname,
				int32_t block, bool blocking = true);

		/** 
		 * \see Writer::commit
		 */
		virtual int commit(const std::string & varname, int32_t iteration = -1);

		/**
		 * \see Writer::commit_block
		 */
		virtual int commit_block(const std::string & varname, 
				int32_t block, int32_t iteration = -1);

		/**
		 * \see Writer::chunk_set
		 * \deprecated
		 */
		virtual chunk_h chunk_set(unsigned int dimensions,
					const std::vector<int> & startIndices, 
					const std::vector<int> & endIndices)
		__attribute__ ((deprecated));

		/**
		 * \see Writer::chunk_free
		 * \deprecated
		 */
		virtual void chunk_free(chunk_h chunkh)
		__attribute__ ((deprecated));

		/** 
		 * \brief Retrieves a parameter's value. 
		 * 
		 * \param[in] paramName : name of the parameter to retrieve.
		 * \param[out] buffer : pointer to the memory where to copy the parameter's value.
		 *
		 * \return 0 in case of success, -1 if the parameter is not found.
		 */
		virtual int get_parameter(const std::string & paramName, void* buffer, 
				unsigned int size);

		/**
		 * Modify a parameter's value. Will cause all dependent layout to be
		 * rebuilt. The modification only has effect on the core that called this function.
		 * The user is responsible for coordinating processes if the modification has to be done
		 * globally. Note: this function will NOT define an inexisting parameter.
		 * 
		 * \param[in] paramName : name of the parameter to modify.
		 * \param[in] buffer : address of the value to set.
		 * \param[in] size : maximum size of the buffer (usually sizeof(type of the parameter)).
		 *
		 * \return 0 in case of success, -1 if the parameter does not exist.
		 */
		virtual int set_parameter(const std::string & paramName, const void* buffer, 
				unsigned int size);
		
		/**
		 * Sends a signal to the server to shut it down (all clients in node need
		 * to call this function before the server is actually killed.
		 * \return 0 in case of success, -1 in case of failure.
		 */
		virtual int kill_server();

		/**
		 * Sends a signal to the server to try free the shared memory.
		 */
		virtual int clean();

		/**
		 * Sends a signal to the server to notify that a client was
		 * unable to write some expected data.
		 */
		virtual int lost();

		/**
		 * Indicates that the iteration has terminated, this will potentially
		 * update connected backends such as VisIt.
		 */
		virtual int end_iteration();

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
