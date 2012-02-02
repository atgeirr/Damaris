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

typedef ChunkHandle* chunk_h;
/**
 * \class Client
 * The Client object represents a single core running the
 * simulation. It is characterized by an ID and is initialized
 * with the name of an XML configuration file.
 */
class Client  {
	protected:
//		Environment *env; /*!< environment object. */
//		Configuration *config; /*!< configuration object. */
//		MetadataManager *metadataManager; /*! metadata manager object. */
//		ActionsManager *actionsManager; /*! keeps actions. */
//		SharedMessageQueue *msgQueue; /*!< pointer to the message queue. */
//		SharedMemorySegment* segment; /*!< pointer to the shared memory segment. */

//		Client();

	private:
		Process* process;
		/**
		 * \brief private function called by constructors.
		 */
//		void init(Configuration* config);

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
		 * \brief Constructor.
		 * This constructor is used to start a client using
		 * Damaris::start_mpi_entity.
		 * 
		 * \param[in] config : Configuration object (initialized).
		 * \param[in] env : Environment object (initialized).
		 */
//		Client(Configuration* config);

		/**
		 * \brief Writes a full variable.
		 * Writes a variable into shared memory and sends a message 
		 * notifying the write to the dedicated core running on the same node.
		 * If the memory segment is full, this function will fail.
		 * If the message queue is full, this function will block.
		 * 
		 * \param[in] varname : name of the variable to write. This variable should 
		 *                      be defined in the XML configuration.
		 * \param[in] iteration : iteration number for this write.
		 * \param[in] data : pointer to the data to be copied to the shared memory buffer.
		 * 
		 * \return the amount of bytes written in case of success,
		 *         -1 if the variable has not been defined,
		 *         -2 if the allocation of memory failed.
		 */
		virtual int write(const std::string & varname, int32_t iteration, const void* data);
		
		/**
		 * \brief Writes a chunk of a variable.
		 * Checks that the chunk is within the bound of the variable's layout first.
		 * 
		 * \param[in] varname : name of the variable to write in. This variable should
		 *                      be defined in the XML configuration.
		 * \param[in] iteration : iteration number for this write.
		 * \param[in] chunkh : handle of a pre-defined chunk.
		 * \param[in] data : pointer to the data to be written in the shared memory buffer.
		 * \return the amount of bytes written in case of success,
		 *         -1 if the variable has not been defined,
		 *         -2 if the allocation of memory failed,
		 *         -3 if the chunk has an inapropriate shape.
		 */
		virtual int chunk_write(chunk_h chunkh, const std::string & varname, 
			int32_t iteration, const void* data);		

		/**
		 * \brief Sends an event.
		 * Sends en event to the dedicated core running on the dedicated core.
		 * If the message queue is full, this function will block 
		 * until it can send the event.
		 * If the event is not explicitely defined in the configuration file,
		 * it will be ignored.
		 * 
		 * \param[in] signam : name of the event to send.
		 * \param[in] iteration : iteration associated to the event.
		 * 
		 * \return 0 in case of success,
		 *         -1 in case of failure when sending the message,
		 *	   -2 in case the event is not defined.
		 */
		virtual int signal(const std::string & signame, int32_t iteration);

		/**
		 * \brief Allocate a buffer directly in shared memory for future writing.
		 * Allocates a buffer in shared memory so it can be written after by the simulation.
		 * Requires a call to commit to notify the dedicated core that the variable 
		 * has been written. 
		 *
		 * \param[in] varname : name of the variable to write 
		 *		        (must be defined in the configuration file).
		 * \param[in] iteration : iteration at which the variable is written.
		 *
		 * \return a pointer to the allocated memory in case of success,
		 *         NULL in case of failure (variable not defined, allocation error).
		 */
		virtual void* alloc(const std::string & varname, int32_t iteration);

		/** 
		 * \brief Commit a variable.
		 * Notifies the dedicated core that the previously 
		 * allocated buffer has been written 
		 * Blocks if the message queue is full.
		 *
		 * \param[in] varname : name of the variable to notify 
		 *			(must have been previously allocated).
		 * \param[in] iteration : iteration of the associated variable.
		 * 
		 * \return 0 in case of success,
                 *        -1 if the variable hasn't been allocated.
		 */
		virtual int commit(const std::string & varname, int32_t iteration);

		/**
		 * \brief Defines a chunk.
		 * The representation of the chunk is as in Fortran, given boundaries.
		 * \param[in] type : type of the chunk.
		 * \param[in] dimensions : number of dimensions.
		 * \param[in] startIndices : start indices.
		 * \param[in] endIndices : end indices.
		 * \return A chunk handle. Call chunk_free to free the chunk handle.
		 */
		virtual chunk_h chunk_set(unsigned int dimensions,
					const std::vector<int> & startIndices, 
					const std::vector<int> & endIndices);

		/**
		 * \brief Free a chunk handle.
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
