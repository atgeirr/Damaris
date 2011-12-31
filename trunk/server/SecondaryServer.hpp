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
 * \file SecondaryServer.hpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#ifndef __DAMARIS_SECONDARY_SERVER_H
#define __DAMARIS_SECONDARY_SERVER_H

#include <string>
#include <mpi.h>

#include "client/Client.hpp"
#include "common/Message.hpp"

namespace Damaris {

	class SecondaryServer : public Server {
		public:
			/**
			 * \brief Constructor.
			 * Used when starting a server from an external program
			 * 
			 * \param[in] configFile : name of the configuration file.
			 * \param[in] id : identifier for this server.
			 */
			SecondaryServer(const std::string &configFile, int id);

			/**
			 * \brief Constructor. 
			 * Used by the standalone server program, which pre-initializes 
			 * a configuration through command line arguments, 
			 * and a pre-initialized environment 
			 *
			 * \param[in] config : a fully initialized Configuration object.
			 */
			SecondaryServer(Configuration* config);

			/**
			 * \brief Destructor 
			 * Delete all related objects (configuration, environment, shared memory objects).
			 */
			~SecondaryServer();

		private:
			/** 
			 * Initializes everything (called by constructors).
			 */
			void init();
	}; // class SecondaryServer

} // namespace Damaris

#endif
