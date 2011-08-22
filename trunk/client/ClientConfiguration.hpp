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

#ifndef __DAMARIS_CLIENT_CONFIG_H
#define __DAMARIS_CLIENT_CONFIG_H

#include <map>
#include <list>
#include <string>

#include "common/Configuration.hpp"

namespace Damaris {

	/** 
	 * The class ClientConfiguration is a child class of Configuration,
	 * implementing configuration features that are specific to the Client.
	 */
	class ClientConfiguration : public Configuration {
		
	private:
		static ClientConfiguration* m_instance;
		
		/**
		 * \brief Constructor.
		 * \param[in] configName : name of the configuration file to load.
		 */
		ClientConfiguration(std::string* configName);

		/**
		 * \brief Destructor.
		 */
		~ClientConfiguration();

	public:
		/**
		 * \brief Retrieve an instance of ClientConfiguration (singleton design pattern).
		 * \return NULL if ClientConfiguration::initialize has never been called before, 
		 *         a valid pointer otherwise.
		 */
		static ClientConfiguration* getInstance();

		/**
		 * \brief Initializes ClientConfiguration with a given configuration file.
		 * \param[in] configName : name of the configuration file to load.
		 */
		static void initialize(std::string* configName);
		
		/**
		 * \brief Finalize (free resources) ClientConfiguration.
		 * If ClientConfiguration::getInstance() is called after finalize, NULL is returned.
		 */
		static void finalize();
		
		/**
		 * For ClientConfiguration, this function is empty since the client doesn't
		 * have to play with plugins loading.
		 */ 
		void setEvent(const char* name, const char* action, const char* plugin);
};

}

#endif
