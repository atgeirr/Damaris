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
#include "common/Group.hpp"
#include "common/Calc.hpp"

namespace Damaris {

	/** 
	 * The class Configuration holds all informations extrated from
	 * both the command lines (for the server) and the external configuration
	 * file. It is independant of the format of this configuration file
	 * (which is loaded through the ConfigHandler object).
	 */
	class Configuration {
				
	private:
		static Configuration* m_instance; /*!< Pointer to a unique Configuration object (singleton). */

		std::string* configFile; 	/*!< For information, we keep the name of the configuration file. */
		std::string* simulationName; 	/*!< Name of the simulation. */
		int coresPerNode;	 	/*!< Total number of cores per node. */
		int clientsPerNode;		/*!< Total number of clients per node. */
		std::string* segmentName; 	/*!< Name of the shared memory segment. */
		size_t segmentSize; 		/*!< Size of the shared memory segment. */
		std::string* msgQueueName; 	/*!< Name of the shared messages queue. */
		size_t msgQueueSize; 		/*!< Size (in number of messages) of the queue of messages. */
		language_e defaultLanguage;	/*!< Default language (C, Fortran or Unknown) */
		std::string sharedMemoryModel;	/*!< True is shared memory model is XSI */
		
		std::map<std::string,Parameter>* parameters;	 	/*!< List of parameters. */
		std::map<std::string,int> intparams;		/*!< Subset of parameters that are integers */
		// TODO : use boost::any to handle parameters properly
		std::map<std::string,Layout*>* layouts; 	/*!< List of layouts. Associate layout names to layouts. */
		std::map<std::string,std::string>* variableLayouts; /*!< Association from variable names to layout names. */
		
		Group* dataHierarchy; /*!< Parent Group for the data (parameters, variables, layouts) hierarchy. */
		Calc<std::string::const_iterator,std::map<std::string,int> >* layoutInterp; /*!< Parser for layout interpretation */
		
	protected:
		bool checkConfiguration(); /*!< Check if the Configuration is correctly loaded. */

		/**
		 * \brief Constructor.
		 * \param[in] configName : name of the configuration file to load.
		 */
		Configuration(std::string* configName);

		/**
		 * \brief Destructor.
		 */
		~Configuration();

	public:
		/**
		 * \brief Retrieve an instance of Configuration (singleton design pattern).
		 * \return NULL if Configuration::initialize has never been called before, a valid pointer otherwise.
		 */
		static Configuration* getInstance();

		/**
		 * \brief Initializes Configuration with a given configuration file.
		 * \param[in] configName : name of the configuration file to load.
		 */
		static void initialize(std::string* configName);
		
		/**
		 * \brief Finalize (free resources) Configuration.
		 * If Configuration::getInstance() is called after finalize, NULL is returned.
		 */
		static void finalize();
		
		/**
		 * \brief Get the name of the XML file.
		 * \return Name of the XML file. Do not delete this pointer.
		 */
		std::string* getFileName() { return configFile; }

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
		 * \brief Get the number of clients per node.
		 */
		int getClientsPerNode() const { return clientsPerNode; }
		/**
		 * \brief Set the number of clients per node.
		 */
		void setClientsPerNode(int cpn) { clientsPerNode = cpn; }

		/**
		 * \brief Get the number of cores per node.
		 */
		int getCoresPerNode() const { return coresPerNode; }
		/**
		 * \brief Set the number of cores per node.
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
		 * \return true in case of success, false if the parameter is not found.
		 */
		bool getParameterValue(const char* name, void* value);
		
		/**
		 * THIS FUNCTION IS NOT IMPLEMENTED YET...
		 */
		int getParameterString(const char* name, std::string* s);
		
		/**
		 * \brief Get the type of a parameter.
		 * \param[in] name : Name of the parameter to consider.
		 * \param[out] t : buffer of Types::basic_type_e to hold to result.
		 * \return true in case of success, false if the parameter is not found.
		 */
		bool getParameterType(const char* name, Types::basic_type_e* t);		
		
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
		 * \param[in] descriptor : string descriptor for the list of dimensions.
		 * \param[in] l : Langage for the layout.
		 */
		void setLayout(const char* name, const char* type, const char* descriptor, language_e l);
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
		virtual void setEvent(const char* name, const char* action, const char* plugin) = 0;	

		/**
		 * \brief Set the parent group for the data hierarchy.
		 * \param[in] g : new parent Group.
		 */
		void setDataHierarchy(Group* g);

		/**
		 * \brief Set the shared memory functions to use (posix = shm_open, sysv = shmget).
		 * \param[in] model : string representing the model to use.
		 */
		void setSharedMemoryType(const char* str);

		/**
		 * \brief Get the shared memory functions to use.
		 * \return a string representing the functions to use.
		 */
		std::string getSharedMemoryType();
};

}

#endif
