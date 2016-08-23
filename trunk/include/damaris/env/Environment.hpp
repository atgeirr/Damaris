/**************************************************************************
This file is part of Damaris.

Damaris is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Damaris is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Damaris.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

#ifndef __DAMARIS_ENVIRONMENT_H
#define __DAMARIS_ENVIRONMENT_H

#include <list>
#include <iostream>
#include <string>
#include <mpi.h>

#include "damaris/util/Pointers.hpp"
#include "damaris/model/Model.hpp"
#include "damaris/buffer/Buffer.hpp"
#include "damaris/client/Client.hpp"
#include "damaris/server/Server.hpp"
#include "damaris/log/EventLogger.hpp"

namespace damaris {
	
USING_POINTERS;

/** 
 * The class Environment holds all local informations related to this process,
 * as well as the root model used for initializing Damaris.
 */
class Environment { 

private:
	
	static bool _initialized_;
	static shared_ptr<model::Simulation> _baseModel_;

	/* Main communicators */
	static MPI_Comm _entityComm_; /*!< clients or servers communicator 
					(depending on the entity). */
	static MPI_Comm _globalComm_; /*!< global communicator. */
	static MPI_Comm _nodeComm_;   /*!< communicator for the 
					processors in the node. */
	
	// meaningful only on servers: this list stores the known
	// clients connected to this server.
	static std::list<int> _knownClients_;
	// id representing a specific run of the simulation
	static std::string _magicNumber_;

	static int 	_lastIteration_; /*!< Current iteration. */
	static bool 	_isClient_; /*!< Whether or not this process is a client. */
	static bool 	_isDedicatedCore_; /*!< Whether or not this process is a dedicated core. */
	static bool	_isDedicatedNode_; /*!< Whether or not this process is a dedicated node. */
	static int 	_globalProcessID_; /*!< Global process ID. */
	static int 	_entityProcessID_; /*!< ID of the process among the
						processes of the same kind. */

	static int	_clientsPerNode_; /*!< number of clients per client nodes. */
	static int	_coresPerNode_;	/*!< number of cores per node. */
	static int _serversPerNode_; //orc:number of servers per node

	static shared_ptr<Client> _client_; /*!< Pointer to the Client object
		that serves for the API. not-null only if _isClient_ is true. */
	static shared_ptr<Server> _server_; /*!< Pointer to the Server object.
		Not-null only if _isDedicatedCore_ or _isDedicatedNode is true. */
	static shared_ptr<Buffer> _defaultBuffer_; /*!< Default buffer for
					variables to allocate into. */
	static shared_ptr<EventLogger> _eventLogger_; /*!< The single instance of the EventLogger
					object used for logging damaris events. */
	static bool _sharedStructuresOwner_; /*!< True if this process is
			Responsible for freeing the shared memory. */
	
	static bool OpenSharedStructures();
	static bool CreateSharedStructures();
	static bool CreateLocalStructures();
	static bool InitManagers();
	static bool InitDedicatedCores(MPI_Comm comm);
	static bool InitDedicatedNodes();
	static bool InitStandalone(MPI_Comm comm);

public:
	/**
	 * Initializes the environment given a model and a communicator.
	 * The configuration file must exist (Abort if the file does not exist).
	 * The hardware configuration (number of cores in each node) must
	 * match the configuration file (Abort otherwise). 
	 * This function will initialize all Managers.
	 *
	 * \param[in] confiFile : name of the XML file to read.
	 * \param[in] global : global MPI communicator.
	 */
	static bool Init(const std::string& configFile,
			MPI_Comm global);

	/**
	 * Finalize the environment, also deletes all the Managers and
	 * the object they store.
	 */
	static bool Finalize();

	/**
	 * Returns true if Damaris has been initialized.
	 */
	static bool Initialized() {
		return _initialized_;
	}
	
	/**
	 * Returns the model that was used to initialize the Enrvironment.
	 */
	static shared_ptr<model::Simulation> GetModel() {
		return _baseModel_;
	}

	/**
	 * Tells if the process is a client or not.
	 */
	static bool IsClient() {
		if(not _initialized_) return true;
		return _isClient_;
	}
	
	/**
	 * Tells if the process is a server or not.
	 */
	static bool IsServer() {
		if(not _initialized_) return false;
		return _isDedicatedCore_ || _isDedicatedNode_ 
			|| (not HasServer());
	}

	/**
	 * Tells if the process is a dedicated core or not.
	 */
	static bool IsDedicatedCore() {
		if(not _initialized_) return false;
		return _isDedicatedCore_;
	}

	/**
	 * Tells if the process is a dedicated node or not.
	 */
	static bool IsDedicatedNode() {
		if(not _initialized_) return false;
		return _isDedicatedNode_;
	}
	
	/**
	 * Returns the instance of Client.
	 */
	static shared_ptr<Client> GetClient() {
		return _client_;
	}
	
	/**
	 * Returns the instance of Server.
	 */
	static shared_ptr<Server> GetServer() {
		return _server_;
	}

	/**
	 * Get the name of the simulation.
	 */
	static std::string GetSimulationName() {
		if(not _initialized_) return std::string("");
		return _baseModel_->name();
	}

	/**
	 * Get the default language for the running simulation.
	 */
	static model::Language GetDefaultLanguage() {
		if(not _initialized_) return model::Language::unknown;
		return _baseModel_->language();
	}
	
	/**
	 * Returns the ID of the process in the global communicator.
	 */
	static int GetGlobalProcessID() {
		if(not _initialized_) return -1;
		return _globalProcessID_;
	}
	
	/**
	 * Returns the ID of the process in the communicator gathering
	 * processes of the same type (i.e. clients or servers).
	 */
	static int GetEntityProcessID() {
		if(not _initialized_) return -1;
		return _entityProcessID_;
	}

	/**
	 * Get the number of clients per node.
	 */
	static int ClientsPerNode() {
		if(not _initialized_) return -1;
		//return _baseModel_->architecture().cores().clients().count();
		return _clientsPerNode_;
	}

	/**
	 * Get the number of cores per node.
	 */
	static int CoresPerNode() {
		if(not _initialized_) return -1;
		//return _baseModel_->architecture().cores().count();
		return _coresPerNode_;
	}

	/**
	 * Returns the number of dedicated cores.
	 */
	static int ServersPerNode() {
		if(not _initialized_) return -1;
		if(HasServer())
			//return CoresPerNode() - ClientsPerNode();
			return _serversPerNode_;
		else
			return 1;
	}

	/**
	 * Returns the list of id of clients connected to the
	 * dedicated core. In standalone mode, will return a list
	 * with only the id of the calling client.
	 */
	static const std::list<int>& GetKnownLocalClients() {
		return _knownClients_;
	}

	/**
	 * Returns the total number of clients used by the simulation.
	 */
	static int CountTotalClients() {
		if(not _initialized_) return -1;
		int dcore = 0;
		MPI_Comm_size(_entityComm_,&dcore);
		/*if(HasServer()) {
			return dcore*ClientsPerNode();
		}
		return dcore;*/
		if(_isDedicatedCore_){
			return dcore*ClientsPerNode();
		}
		//since in dedicated node mode all cores are client
		//and clientspernode yields the total number of client nodes.
		else if(_isDedicatedNode_){
			return CoresPerNode()*ClientsPerNode();
		}
		return dcore;
	}

	/**
	 * Returns the total number of servers used by the simulation.
	 */
	static int CountTotalServers() {
		if(not _initialized_) return -1;
		int dcore = 0;
		MPI_Comm_size(_entityComm_,&dcore);
		return dcore;
	}

	/**
	 * Add the id of newly connected client.
	 */
	static void AddConnectedClient(int id);

	/**
	 * Gets the communicator gathering processes of the same kind
	 * (i.e. if this process is a client, it returns the communicator
	 * gathering all clients).
	 */
	static MPI_Comm GetEntityComm() {
		if(not _initialized_) return MPI_COMM_NULL;
		return _entityComm_;
	}

	/**
	 * Gets the global communicator passed at start time.
	 */
	static MPI_Comm GetGlobalComm() {
		if(not _initialized_) return MPI_COMM_NULL;
		return _globalComm_;
	}
	
	/**
	 * Get the communicator gathering processes of the same node.
	 */
	static MPI_Comm GetNodeComm() {
		if(not _initialized_) return MPI_COMM_NULL;
		return _nodeComm_;
	}

	/**
	 * Tell if a dedicated core/node is present.
	 */
	static bool HasServer() {
		if(not _initialized_) return false;
		//return (CoresPerNode() != ClientsPerNode());
		return (_serversPerNode_ != 0);
	}

	/**
	 * Returns the number of domain that each client is supposed to handle.
	 */
	static unsigned int NumDomainsPerClient() {
		if(not _initialized_) return -1;
	//	return _baseModel_->architecture().cores().clients().domains();
		return _baseModel_->architecture().domains().count();
	}

	/**
	 * Returns the last known terminated iteration.
	 */
	static int GetLastIteration() {
		if(not _initialized_) return -1;
		return _lastIteration_;
	}

	/**
	 * Increments the iteration counter. On servers this function should 
	 * be called as many times as the number of clients connected to this
	 * server in order for the iteration counter to be incremented.
	 * \return true only if the iteration counter has been changed.
	 */
	static bool StartNextIteration();

	/**
	 * Gets the magic number for this simulation (supposed to
	 * uniquely identify a run of a simulation).
	 */
	static const std::string& GetMagicNumber() {
		return _magicNumber_;
	}
	
	/**
	 * Returns the default buffer of the simulation.
	 */
	static shared_ptr<Buffer> GetDefaultBuffer() {
		return _defaultBuffer_;
	}

	/**
    * Returns the singleton event logger.
    */
	static shared_ptr<EventLogger> GetEventLogger() {
		return _eventLogger_;
	}
};

}

#endif
