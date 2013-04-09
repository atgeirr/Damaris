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
 * \date November 2012
 * \author Matthieu Dorier
 * \version 0.7
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

		bool errorOccured; /*!< Indicates wether an error occured during the last iteration. */
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
		 * \param[in] mdl : model loaded from an XML file.
		 * \param[in] comm : communicator gathering clients.
		 */
		static Client* New(std::auto_ptr<Model::Simulation> mdl, MPI_Comm clients);


		/**
		 * Notifies a potential server that the client has connected.
		 */
		virtual int connect();

		/**
		 * \brief Writes a full variable.
		 * Writes a variable into shared memory and sends a message 
		 * notifying the write to the dedicated core running on the same node.
		 * If the memory segment is full, this function will fail.
		 * If the message queue is full, this function will block.
		 * 
		 * \param[in] varname : name of the variable to write. This variable should 
		 *                      be defined in the XML configuration.
		 * \param[in] data : pointer to the data to be copied to the shared memory buffer.
		 *
		 * \param[in] blocking : if true, waits for enough memory to be available.
		 *  
		 * \return the amount of bytes written in case of success,
		 *         -1 if the variable has not been defined,
		 *         -2 if the allocation of memory failed.
		 */
		virtual int write(const std::string & varname, 
				const void* data, bool blocking = false);

		/**
		 * Same as Client::write, but write a given block of data
		 * refered by its block ID.
		 * Returns -3 if the block ID is invalid.
		 */
		virtual int write_block(const std::string &varname,
				int32_t block, const void* data,
				bool blocking = false);

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
		 *         -2 in case the event is not defined.
		 */
		virtual int signal(const std::string & signame);

		/**
		 * \brief Allocate a buffer directly in shared memory for future writing.
		 * Allocates a buffer in shared memory so it can be written after by the simulation.
		 * Requires a call to commit to notify the dedicated core that the variable 
		 * has been written. 
		 *
		 * \param[in] varname : name of the variable to write 
		 *                      (must be defined in the configuration file).
		 * \param[in] blocking : blocks until enough memory is available, if true.
		 *
		 * \return a pointer to the allocated memory in case of success,
		 *         NULL in case of failure (variable not defined, allocation error).
		 */
		virtual void* alloc(const std::string & varname, 
				bool blocking = true);

		/**
		 * Same as Client::alloc, but with a block ID parameter.
		 * Returns NULL also if the block ID is invalid.
		 */
		virtual void* alloc_block(const std::string & varname,
				int32_t block, bool blocking = true);

		/** 
		 * \brief Commit a variable (i.e. all the blocks written for it).
		 * Notifies the dedicated core that the previously 
		 * allocated buffer has been written 
		 * Blocks if the message queue is full.
		 *
		 * \param[in] varname : name of the variable to notify 
		 *                      (must have been previously allocated).
		 * \param[in] iteration : iteration of the associated variable.
		 * 
		 * \return 0 in case of success,
		 *        -1 if the variable hasn't been allocated,
		 * 	  -2 if the variable is unknown,
		 * 	  -3 for an unknown error (dynamic cast error).
		 */
		virtual int commit(const std::string & varname, int32_t iteration = -1);

		/**
		 * Same as Client::commit but with an additional block ID parameter
		 * to commit only one single block.
		 */
		virtual int commit_block(const std::string & varname, 
				int32_t block, int32_t iteration = -1);

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
