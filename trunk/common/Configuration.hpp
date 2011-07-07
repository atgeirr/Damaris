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

#ifndef __DAMARIS_CONFIG_H
#define __DAMARIS_CONFIG_H

#include <map>
#include <list>
#include <string>

#include "common/Language.hpp"
#include "common/Parameter.hpp"
#include "common/Layout.hpp"

/* TODO: having #define statements is not good, we should have
   a child class of Configuration for the server, and another for
   the Client */
#ifdef __DAMARIS_SERVER
	#include "common/ActionsManager.hpp"
#endif

namespace Damaris {

	/** 
	 * The class Configuration holds all informations extrated from
	 * both the command lines (for the server) and the external configuration
	 * file. It is independant of the format of this configuration file
	 * (which is loaded through the ConfigHandler object).
	 */
	class Configuration {
		
	private:
		std::string* configFile; 	/*!< For information, we keep the name of the configuration file. */
		std::string* simulationName; 	/*!< Name of the simulation. */
		int coresPerNode;	 	/*!< Total number of cores per node. */
		std::string* segmentName; 	/*!< Name of the shared memory segment. */
		size_t segmentSize; 		/*!< Size of the shared memory segment. */
		std::string* msgQueueName; 	/*!< Name of the shared messages queue. */
		size_t msgQueueSize; 		/*!< Size (in number of messages) of the queue of messages. */
		language_e defaultLanguage;	/*!< Default language (C, Fortran or Unknown) */
		
		std::map<std::string,Parameter>* parameters; 	/*!< List of parameters. */
		std::map<std::string,Layout*>* layouts; 	/*!< List of layouts. Associate layout names to layouts. */
		/* TODO: right now this association just contains names,
		   it should further associate a variable name to a VariableInfo object
		   containing ALL informations related to a variable */
		std::map<std::string,std::string>* variableLayouts; /*!< Association from variable names to layout names. */
#ifdef __DAMARIS_SERVER
		ActionsManager* actionsManager; /*!< Container of actions. */
#endif
		bool checkConfiguration(); /*!< Check if the Configuration is correctly loaded. */
	public:
		/**
		 * \brief Constructor.
		 * \param[in] configName : name of the configuration file to load.
		 */
		Configuration(std::string* configName);
		
		/**
		 * \brief Destructor.
		 */
		~Configuration();

		/**
		 * \brief Get the name of the XML file.
		 * \return Name of the XML file. Do not delete this pointer.
		 */
		std::string* getFileName() { return configFile; }

#ifdef __DAMARIS_SERVER	
		/**
		 * \brief Get the ActionManager initialized within the configuration.
		 * Only available for Server.
		 * \return The ActionManager.
		 */
		ActionsManager* getActionsManager();
#endif
		
		/**
		 * \brief Get the name of the simulation.
		 */
		std::string* getSimulationName() { return simulationName; }
		/**
		 * \brief Set the name of the simulation.
		 */
		void setSimulationName(const char* name) { simulationName = new std::string(name); }

		/**
		 * \brief Get the default language for the running simulation.
		 */
		language_e getDefaultLanguage() { return defaultLanguage; }
		/**
		 * \brief Set the default language for the running simulation.
		 */
		void setDefaultLanguage(language_e l) { defaultLanguage = l; }	
	
		/**
		 * \brief Get the number of cores per node (including those used by Damaris).
		 */
		int getCoresPerNode() const { return coresPerNode; }
		/**
		 * \brief Set the number of cores per node (including those used by Damaris).
		 */
		void setCoresPerNode(int cpn) { coresPerNode = cpn; }

		/**
		 * \brief Get the name of the shared memory segment.
		 */
		std::string* getSegmentName() const { return segmentName; }
		/**
		 * \brief Set the name of the shared memory segment.
		 */
		void setSegmentName(char* name) { segmentName = new std::string(name); }

		/**
		 * \brief Get the size (in bytes) of the shared memory segment.
		 */
		size_t getSegmentSize() const { return segmentSize; }
		/**
		 * \brief Set the size (in bytes) of the shared memory segment.
		 */
		void setSegmentSize(int s) { segmentSize = (size_t)s; }
		
		/**
		 * \brief Get the name of the message queue.
		 */
		std::string* getMsgQueueName() const { return msgQueueName; }
		/**
		 * \brief Set the name of the message queue.
		 */
		void setMsgQueueName(char* name) { msgQueueName = new std::string(name); }

		/**
		 * \brief Get the size (in number of messages) of the message queue.
		 */
		size_t getMsgQueueSize() const { return msgQueueSize; }
		/**
		 * \brief Set the size (in number of messages) of the message queue.
		 */
		void setMsgQueueSize(int s) { msgQueueSize = s; }

		/**
		 * \brief Get the value associated to a parameter.
		 * This function doesn't work with string parameters.
		 * \param[in] name : Name of the parameter to retrieve.
		 * \param[out] value : Buffer to hold the value.
		 * \return 0 in case of success, -1 if the parameter is not found.
		 */
		int getParameterValue(const char* name, void* value);
		
		/**
		 * THIS FUNCTION IS NOT IMPLEMENTED YET...
		 */
		int getParameterString(const char* name, std::string* s);
		
		/**
		 * \brief Get the type of a parameter.
		 * \param[in] name : Name of the parameter to consider.
		 * \param[out] t : buffer of Types::basic_type_e to hold to result.
		 * \return 0 in case of success, -1 if the parameter is not found.
		 */
		int getParameterType(const char* name, Types::basic_type_e* t);		
		
		/**
		 * \brief Set a parameter (type and value).
		 * \param[in] name : Name of the parameter to set.
		 * \param[in] type : Type of the parameter.
		 * \param[in] value : value of the parameter.
		 */
		void setParameter(const char* name, const char* type, const char* value);
		
		/**
		 * \brief Associate a variable name to the name of its layout.
		 * \param[in] name : Name of the variable.
		 * \param[in] layoutName : Name of the layout.
		 */
		void setVariableInfo(const char* name, const char* layoutName);
		
		/**
		 * \brief Get the layout associated to a variable.
		 * \param[in] name : Name of the variable.
		 * \return A pointer to the associated Layout, NULL if the layout is not found.
		 * The returned pointer is hold by the Configuration object and doesn't have to be deleted.
		 */
		Layout* getVariableLayout(const char* name);
	
		/**
		 * \brief Set a layout by its name, type, list of dimensions and language.
		 * \param[in] name : Name of the layout.
		 * \param[in] type : Type of the data to be hold.
		 * \param[in] dims : List of the dimensions (see Layout).
		 * \param[in] l : Langage for the layout.
		 */
		void setLayout(const char* name, const char* type, const std::list<int>* dims, language_e l);
		/**
		 * \brief Get the layout by its name.
		 * \param[in] name : Name of the layout to retrieve.
		 * \return A pointer to the layout found, or NULL if not found. The returned pointer is
		 * hold by the Configuration and doesn't have to be deleted.
		 */
		Layout* getLayout(const char* name);
		
		/**
		 * \brief Set an event by its name, name of function and name of plugin's file.
		 * \param[in] name : Name of the event.
		 * \param[in] action : Name of the function to load.
		 * \param[in] plugin : Name of the dynamic library to load.
		 */	
		void setEvent(const char* name, const char* action, const char* plugin);	
};

}

#endif
