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
 * \file Configuration.hpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#ifndef __DAMARIS_CONFIG_H
#define __DAMARIS_CONFIG_H

#include <map>
#include <list>
#include <string>
#include <iostream>

#include "xml/Model.hpp"
#include "common/ParameterSet.hpp"
#include "common/MetadataManager.hpp"
#include "common/ActionsManager.hpp"
#include "common/Language.hpp"
#include "common/Layout.hpp"
#include "common/Calc.hpp"

namespace Damaris {

	/** 
	 * The class Configuration holds all informations extrated from
	 * both the command lines (for the server) and the external configuration
	 * file. It is independant of the format of this configuration file
	 * (which is loaded through the Damaris::Model::simulation_mdl object).
	 */
	class Configuration {
				
	private:
		static Configuration* m_instance; /*!< Pointer to a unique Configuration 
						    object (singleton). */

		std::auto_ptr<Model::simulation_mdl> baseModel; /*!< Model extracted from 
								  the XML file */
		std::string* configFile; /*!< For information, we keep the name of the 
					   configuration file. */
		
		ParameterSet parameters; 		/*!< List of parameters. */
		ActionsManager* actionsManager;		/*!< Pointer to the ActionsManager. */
		MetadataManager* metadataManager; 	/*!< Pointer to the MetadataManager. */
		
		Calc<std::string::const_iterator,ParameterSet>* layoutInterp; /*!< Parser for 
										layout interpretation. */
		
		void fillParameterSet(); 	/*!< Fills the set of parameters 
						  from the base model. */
		void fillMetadataManager(); 	/*!< Fills the metadata manager 
						  with variables from the base model. */
		void readVariablesInSubGroup(const Model::group_mdl* g, 
				const std::string &name); 
		void fillActionsManager(); 	/*!< Fills the ActionsManager with 
						  Actions from the base model. */

	protected:
		
		/**
		 * \brief Constructor.
		 * \param[in] mdl : simulation base model from XML file.
		 * \param[in] configName : name of the configuration file that has been loaded.
		 */
		Configuration(std::auto_ptr<Model::simulation_mdl> mdl, const std::string& configName);

		/**
		 * \brief Destructor.
		 */
		~Configuration();

	public:
		/**
		 * \brief Retrieve an instance of Configuration (singleton design pattern).
		 * \return NULL if Configuration::initialize has never been called before, 
		 * a valid pointer otherwise.
		 */
		static Configuration* getInstance();

		/**
		 * \brief Initializes Configuration with a given model and configuration file.
		 * \param[in] mdl : simulation base model.
		 * \param[in] configName : name of the configuration file to load.
		 */
		static void initialize(std::auto_ptr<Model::simulation_mdl> mdl, 
				const std::string & configName);
		
		/**
		 * \brief Finalize (free resources) Configuration.
		 * If Configuration::getInstance() is called after finalize, NULL is returned.
		 */
		static void finalize();
		
		/**
		 * \brief Get the name of the XML file.
		 * \return Name of the XML file. Do not delete this pointer.
		 */
		const std::string& getFileName() { return *configFile; }

		/**
		 * \brief Get the name of the simulation.
		 */
		const std::string & getSimulationName() const;

		/**
		 * \brief Get the default language for the running simulation.
		 */
		Language::language_e getDefaultLanguage() const;

		/**
		 * \brief Get the number of clients per node.
		 */
		int getClientsPerNode() const;

		/**
		 * \brief Get the number of cores per node.
		 */
		int getCoresPerNode() const;

		/**
		 * \brief Get the name of the shared memory segment.
		 */
		const std::string& getSegmentName() const;

		/**
		 * \brief Get the size (in bytes) of the shared memory segment.
		 */
		size_t getSegmentSize() const;
		
		/**
		 * \brief Get the name of the message queue.
		 */
		const std::string & getMsgQueueName() const;

		/**
		 * \brief Get the size (in number of messages) of the message queue.
		 */
		size_t getMsgQueueSize() const;

		/**
		 * \brief Get the parameters set.
		 */
		ParameterSet* getParameterSet();
		
		/**
		 * \brief Get the ActionsManager.
		 */
		ActionsManager* getActionsManager();

		/**
		 * \brief Get the MetadataManager.
		 */
		MetadataManager* getMetadataManager();
};

}
#endif
