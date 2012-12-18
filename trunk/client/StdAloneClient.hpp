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
 * \date November 2012
 * \author Matthieu Dorier
 * \version 0.7
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
		 * \see Client::write
		 */
		virtual int write(const std::string & varname, 
				const void* data, bool blocking = false);

		/**
		 * \see Client::write_block
		 */
		virtual int write_block(const std::string &varname,
				int32_t block, const void* data,
				bool blocking = false);

		/**
		 * \see Client::signal
		 */
		virtual int signal(const std::string & signame);

		/** 
		 * \see Client::commit
		 */
		virtual int commit(const std::string & varname, int32_t iteration = -1);

		/**
		 * \see Client::commit_block
		 */
		virtual int commit_block(const std::string & varname, int32_t block,
				int32_t iteration = -1);

		/**
		 * Overwrite the normal kill_server() function from Client,
		 * since a StdAloneClient should ne be killed, but should terminate
		 * itself normally.
		 */
		virtual int kill_server();

		/**
		 * \see Client::clean
		 */
		virtual int clean();

		/**
		 * Indicates that the iteration has terminated, this will potentially
		 * update connected backends such as VisIt.
		 */
		virtual int end_iteration();

		/**
		 * \brief Destructor.
		 * To be called at the end of the before stopping the client program.
		 */
		virtual ~StdAloneClient();
	}; // class StdAloneClient

} // namespace Damaris

#endif
