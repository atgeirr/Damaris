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
 * \date February 2012 
 * \author Matthieu Dorier
 * \version 0.4
 */

#ifndef __DAMARIS_ENVIRONMENT_H
#define __DAMARIS_ENVIRONMENT_H

#include <list>
#include <iostream>
#include <string>
#include <mpi.h>

#include "xml/Model.hpp"
#include "core/Configurable.hpp"

namespace Damaris {

	/** 
	 * The class Environment holds all local informations
	 * such as the id of the enclosing process.
	 */
	class Environment : public Configurable<Model::Simulation> { 

	private:
		MPI_Comm entityComm; /*!< clients or servers communicator (depending on the entity). */
		MPI_Comm globalComm; /*!< global communicator. */
		MPI_Comm nodeComm;   /*!< communicator for the processors in the node. */
		
		std::list<int> knownClients;
		int lastIteration;
		bool isClient;
		std::string magicNumber;
		static Environment* env;

		/**
		 * \brief Constructor taking a base model. 
		 * \param[in] mdl : base model from the configuration file.
		 */
		Environment(const Model::Simulation& mdl);
	
		/**
		 * Destructor.
		 */
		~Environment();

	public:
		/**
		 * Initializes the environement given a model.
		 */
		static void Init(const Model::Simulation& mdl);

		/**
		 * Tells if the process is a client or not.
		 */
		static bool IsClient();

		/**
		 * Set if the process is a client of not.
		 */
		static void SetClient(bool b);

		/**
		 * \brief Get the name of the simulation.
		 */
		static std::string SimulationName();

		/**
		 * \brief Get the default language for the running simulation.
		 */
		static Model::Language DefaultLanguage();

		/**
		 * \brief Get the number of clients per node.
		 */
		static int ClientsPerNode();

		/**
		 * \brief Get the number of cores per node.
		 */
		static int CoresPerNode();


		/**
		 * Returns the number of dedicated cores.
		 */
		static int ServersPerNode();

		/**
		 * Returns the list of id of clients connected to the
		 * dedicated core. In standalone mode, will return a list
		 * with only the id of the calling client.
		 */
		static const std::list<int>& GetKnownLocalClients();

		/**
		 * Returns the total number of clients used by the simulation.
		 */
		static int CountTotalClients();

		/**
		 * Returns the total number of servers used by the simulation.
		 */
		static int CountTotalServers();

		/**
		 * Add the id of newly connected client.
		 */
		static void AddConnectedClient(int id);

		/**
		 * \brief Set the communicator gathering processes of the same kind (client or server.
		 */
		static void SetEntityComm(MPI_Comm comm);

		/**
		 * \brief gets the communicator gathering processes of the same kind.
		 */
		static MPI_Comm GetEntityComm();

		/**
		 * \brief Set the global communicator (usually MPI_COMM_WORLD).
		 */
		static void SetGlobalComm(MPI_Comm comm);

		/**
		 * \brief Gets the global communicator passed at start time.
		 */
		static MPI_Comm GetGlobalComm();

		/**
		 * \brief Set the communicator gathering processes of the same node.
		 */
		static void SetNodeComm(MPI_Comm comm);
		
		/**
		 * \brief Get the communicator gathering processes of the same node.
		 */
		static MPI_Comm GetNodeComm();

		/**
		 * \brief Tell if a dedicated core is present.
		 */
		static bool HasServer();

		/**
		 * Returns the number of domain that each client is supposed to handle.
		 */
		static unsigned int NumDomainsPerClient();

		/**
		 * Returns the last known terminated iteration.
		 */
		static int GetLastIteration();

		/**
		 * Sets the iteration that just terminated.
		 */
		static bool StartNextIteration();

		/**
		 * Gets the magic number for this simulation (supposed to
		 * uniquely identify a run of a simulation).
		 */
		static const std::string& GetMagicNumber();

		/**
		 * Sets the magic number for this simulation.
		 */
		static void SetMagicNumber(const std::string& mgcnbr);
	};

}

#endif
