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
 * \file Environment.hpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */

#ifndef __DAMARIS_ENVIRONMENT_H
#define __DAMARIS_ENVIRONMENT_H

#include <iostream>
#include <mpi.h>

#include "xml/Model.hpp"
#include "common/Language.hpp"
#include "common/Singleton.hpp"

namespace Damaris {

	/** 
	 * The class Environment holds all local informations
	 * such as the id of the enclosing process.
	 */
	class Environment : public Singleton<Environment> {
		friend class Singleton<Environment>;
	private:
		bool initialized;

		Model::simulation_mdl* baseModel; /*! Pointer to the base model. */
		int id; /*!< ID of the process. */
	
		MPI_Comm entityComm; /*!< clients or servers communicator (depending on the entity). */
		MPI_Comm globalComm; /*!< global communicator. */
		MPI_Comm nodeComm;   /*!< communicator for the processors in the node. */
		/**
		 * \brief Constructor taking a base model and an ID. 
		 * \param[in] mdl : base model from the configuration file.
		 * \param[in] i : id of the process.
		 */
		Environment();
		/**
		 * Destructor.
		 */
		~Environment();

	public:
		/**
		 * \brief Initialize the Environment singleton object.
		 * \param[in] mdl : base model from the configuration file.
		 * \param[in] i : id of the process.
		 */
		void initialize(Model::simulation_mdl* mdl);

		/**
		 * \brief Get the ID of the server.
		 * \return ID of the server.
		 */
		int getID() { return id; }

		/**
		 * \brief Set the ID of the server.
		 * \param[in] i : new ID.
		 */
		void setID(int i) { id = i; }

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
		 * \brief Set the communicator gathering processes of the same kind (client or server.
		 */
		void setEntityComm(MPI_Comm comm) { entityComm = comm; }

		/**
		 * \brief gets the communicator gathering processes of the same kind.
		 */
		MPI_Comm getEntityComm() { return entityComm; }

		/**
		 * \brief Set the global communicator (usually MPI_COMM_WORLD).
		 */
		void setGlobalComm(MPI_Comm comm) { globalComm = comm; }

		/**
		 * \brief Gets the global communicator passed at start time.
		 */
		MPI_Comm getGlobalComm() { return globalComm; }

		/**
		 * \brief Set the communicator gathering processes of the same node.
		 */
		void setNodeComm(MPI_Comm comm) { nodeComm = comm; }
		
		/**
		 * \brief Get the communicator gathering processes of the same node.
		 */
		MPI_Comm getNodeComm() { return nodeComm; }
	};

}

#endif
