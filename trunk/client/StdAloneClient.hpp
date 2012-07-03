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
 * \file StdAloneClient.hpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.5
 */
#ifndef __DAMARIS_STDALONE_CLIENT_H
#define __DAMARIS_STDALONE_CLIENT_H

#include <string>
#include <vector>
#include <stdint.h>

#include "client/Client.hpp"

namespace Damaris {

class Initiator;
/**
 * The StdAloneClient object represents a single core running the
 * simulation when no dedicated core is present. In this case the
 * client runs the plugins by itself, synchronously.
 */
class StdAloneClient : public Client {

	friend class Initiator;

		/** 
		 * \brief Constructor.
		 * Initializes the client given an already built Process object.
		 * This constructor is private, only the Initiator class can access it.
		 *
		 * \param[in] p : pointer to a initialized Process object, required to be not-null.
		 */
		StdAloneClient(Process* p);

	public:

		/**
		 * \see Client::connect
		 */
		virtual int connect();

		/**
		 * \see Writer::write
		 */
		virtual int write(const std::string & varname, int32_t iteration, 
						const void* data, bool blocking = true);
		
		/**
		 * \see Writer::chunk_write
		 */
		virtual int chunk_write(chunk_h chunkh, const std::string & varname, 
						int32_t iteration, const void* data, bool blocking = true);		

		/**
		 * \see Writer::signal
		 */
		virtual int signal(const std::string & signame, int32_t iteration);

		/**
		 * \see Writer::alloc
		 */
		virtual void* alloc(const std::string & varname, int32_t iteration, bool blocking = true);

		/** 
		 * \see Writer::commit
		 */
		virtual int commit(const std::string & varname, int32_t iteration);

		/**
		 * Sends a signal to the server to shut it down (all clients in node need
		 * to call this function before the server is actually killed.
		 * \return 0 in case of success, -1 in case of failure.
		 */
		virtual int kill_server();

		/**
		 * \see Client::clean
		 */
		virtual int clean(int iteration);

		/**
		 * Indicates that the iteration has terminated, this will potentially
		 * update connected backends such as VisIt.
		 */
		virtual int end_iteration(int iteration);

		/**
		 * \brief Destructor.
		 * To be called at the end of the before stopping the client program.
		 */
		virtual ~StdAloneClient();
}; // class StdAloneClient

} // namespace Damaris

#endif
