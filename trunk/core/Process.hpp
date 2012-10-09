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
 * \version 0.4
 */
#ifndef __DAMARIS_PROCESS_H
#define __DAMARIS_PROCESS_H

#include <string>
#include <vector>
#include <stdint.h>

#include "xml/Model.hpp"
#include "core/Environment.hpp"
#include "memory/SharedMessageQueue.hpp"
#include "memory/SharedMemorySegment.hpp"

namespace Damaris {

/**
 * \class Process
 * The Process object represents a single core either dedicated or running the
 * simulation. It is characterized by an ID and is initialized
 * with the name of an XML configuration file.
 * Process is a singleton object that has to be initialized using Process:initialize.
 */
class Process {

	private:
		static Process* _instance; /*!< The singleton instance of Process. */

		/**
		 * Helper function called by the constructor to initialize everything.
		 * \param[in] conf : name of the configuration file to load.
		 * \param[in] id : id of the process.
		 */
		void init(const std::string& conf, int32_t id = -1);

		bool sharedStructuresOwner; /*!< This variable indicates wether the process
						 has created the shared structures of not.
						 If it did, it will have to remove them at the end. */


		/** 
		 * \brief Constructor.
		 * Initializes the process given the name of a configuration file and an ID. 
		 * Damaris won't check if two process have the same ID so the user shoud be
		 * careful with that. This constructor is private: use Process::initialize.
		 *
		 * \param[in] config : name of an XML configuration file.
		 * \param[in] id : id of the process (should be unique).
		 */
		Process(const std::string & config, int32_t id = -1);

		/**
		 * Destructor. This function is private, use Process:kill instead.
		 */
		~Process();	

	protected:
		std::auto_ptr<Damaris::Model::Simulation> model; /*!< base model initialized from the configuration file. */
		SharedMessageQueue 	*msgQueue; /*!< pointer to the message queue. */
		SharedMemorySegment 	*segment; /*!< pointer to the shared memory segment. */
		int id;
	public:
		/**
		 * Get the singleton instance of the Process object. NULL is returned if
		 * Process has not been initialized, and an error message is printed.
		 */
		static Process* get();
	
		/**
		 * Initializes the singleton instance of Process.
		 * \param[in] config : name of the configuration file.
		 * \param[in] id : id of the process.
		 */	
		static void initialize(const std::string &config, int32_t id = -1);		

		/**
		 * Kill the singleton instance of Process.
		 */
		static bool kill();

		/**
		 * Opens the shared message queue and shared memory segment
		 * if they haven't been opened yet.
		 */
		void openSharedStructures();

		/**
		 * Creates the shared message queue and shared memory segment.
		 * Removes previously opened shared structures with the same name.
		 */
		void createSharedStructures();

		/**
		 * Get the SharedMessageQueue instance attached to the Process.
		 */
		SharedMessageQueue* getSharedMessageQueue()
		{
			return msgQueue;
		}

		/**
		 * Get the SharedMemorySegment instance attached to the Process.
		 */
		SharedMemorySegment* getSharedMemorySegment()
		{
			return segment;
		}

		/**
		 * Returns a pointer over the XML model.
		 */
		Model::Simulation* getModel()
		{
			return model.get();
		}

		/**
		 * Returns the rank of the process.
		 */
		int getID() const
		{
			return id;
		}

		/**
		 * Set the rank of the process.
		 */
		void setID(int i) 
		{
			id = i;
		}

}; // class Process 

} // namespace Damaris

#endif
