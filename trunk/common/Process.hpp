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
 * \file Process.hpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.3
 * 
 * Client.hpp is the main file to be included in a C++ 
 * client in order to access the main client-side functions
 * of Damaris.
 */
#ifndef __DAMARIS_PROCESS_H
#define __DAMARIS_PROCESS_H

#include <string>
#include <vector>
#include <stdint.h>

#include "xml/Model.hpp"
#include "common/Environment.hpp"
#include "common/SharedMessageQueue.hpp"
#include "common/SharedMemorySegment.hpp"
#include "common/ActionsManager.hpp"
#include "common/MetadataManager.hpp"

/**
 * \namespace Damaris
 * All classes related to Damaris are defined under the Damaris namespace.
 */
namespace Damaris {

/**
 * \class Process
 * The Process object represents a single core either dedicated or running the
 * simulation. It is characterized by an ID and is initialized
 * with the name of an XML configuration file.
 */
class Process {
	private:
		static Process* _instance;
		static bool _instanciated;

//		void init(std::auto_ptr<Damaris::Model::SimulationModel> mdl);
		void init(const std::string& conf, int32_t id);
//		void init();
		bool sharedStructuresOwner;
	
	public:
		void openSharedStructures();
		void createSharedStructures();

	protected:
		std::auto_ptr<Damaris::Model::SimulationModel> model;
		Environment 	*environment; /*!< environment object. */
		MetadataManager *metadataManager; /*! metadata manager object. */
		ActionsManager 	*actionsManager; /*! keeps actions. */
		SharedMessageQueue 	*msgQueue; /*!< pointer to the message queue. */
		SharedMemorySegment 	*segment; /*!< pointer to the shared memory segment. */

	public:
		/** 
		 * \brief Constructor.
		 * Initializes the client given the name of a configuration file and an ID. 
		 * Damaris won't check if two clients have the same ID so the user shoud be
		 * careful with that.
		 *
		 * \param[in] config : name of an XML configuration file.
		 * \param[in] id : id of the client (should be unique).
		 * \param[in] tag : "open" tag, will only open existing shared structures.
		 */
		Process(const std::string & config, int32_t id);//, struct open);

//		Process(std::auto_ptr<Damaris::Model::SimulationModel> mdl, int32_t id, struct open);

		/** 
		 * \brief Constructor.
		 * Initializes the client given the name of a configuration file and an ID. 
		 * Damaris won't check if two clients have the same ID so the user shoud be
		 * careful with that.
		 *
		 * \param[in] config : name of an XML configuration file.
		 * \param[in] id : id of the client (should be unique).
		 * \param[in] tag : "create" tag, will create shared structures.
		 */
//		Process(const std::string & config, int32_t id, struct create);

//		Process(std::auto_ptr<Damaris::Model::SimulationModel> mdl, int32_t id, struct create);

		Environment* getEnvironment() 
		{
			return environment;
		}

		MetadataManager* getMetadataManager()
		{
			return metadataManager;
		}

		ActionsManager* getActionsManager()
		{
			return actionsManager;
		}

		SharedMessageQueue* getSharedMessageQueue()
		{
			return msgQueue;
		}

		SharedMemorySegment* getSharedMemorySegment()
		{
			return segment;
		}

		static Process* getInstance()
		{
			return _instance;
		} 

		~Process();
}; // class Process 

} // namespace Damaris

#endif
