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
#include "core/Debug.hpp"
#include "core/Environment.hpp"

namespace Damaris {

	Environment* Environment::env = NULL;

	Environment::Environment(const Model::Simulation& mdl)
	: Configurable<Model::Simulation>(mdl)
	{
		entityComm = MPI_COMM_WORLD;
		globalComm = MPI_COMM_NULL;
		nodeComm   = MPI_COMM_NULL;
		lastIteration = 0;
	}

	Environment::~Environment() 
	{ }

	void Environment::Init(const Model::Simulation& mdl)
	{
		if(env != NULL) {
			WARN("Called twice Environment::Init");
			return;
		}
		env = new Environment(mdl);
	}

	bool Environment::IsClient()
	{
		if(env == NULL) return true;
		return env->isClient;
	}

	void Environment::SetClient(bool b)
	{
		if(env != NULL)
			env->isClient = b;
	}

	int Environment::CoresPerNode() 
	{
		if(env == NULL) return -1;
		return env->model.architecture().cores().count();
	}

	int Environment::ClientsPerNode() 
	{
		if(env == NULL) return -1;
		return env->model.architecture().cores().clients().count();
	}

	int Environment::ServersPerNode()
	{
		if(env == NULL) return -1;
		if(HasServer())
			return CoresPerNode() - ClientsPerNode();
		else
			return 1;
	}

	const std::list<int>& Environment::GetKnownLocalClients()
	{
		return env->knownClients;
	}

	void Environment::AddConnectedClient(int id)
	{
		std::list<int>::iterator it = env->knownClients.begin();
		while(it != env->knownClients.end() && (*it) != id) {
			it++;
		}
		if(it == env->knownClients.end()) {
			env->knownClients.push_back(id);
		}
	}

	int Environment::CountTotalClients()
	{
		int dcore = 0;
		MPI_Comm_size(env->entityComm,&dcore);
		if(HasServer()) {
			return dcore*ClientsPerNode();
		} 
		return dcore;
	}

	int Environment::CountTotalServers()
	{
		int dcore = 0;
		MPI_Comm_size(env->entityComm,&dcore);
		return dcore;
	}

	std::string Environment::SimulationName()
	{
		if(env == NULL) return "unknown";
		return env->model.name();
	}

	Model::Language Environment::DefaultLanguage()
	{
		if(env == NULL) return Model::Language::unknown;
		return env->model.language();
	}

	void Environment::SetEntityComm(MPI_Comm comm) 
	{ 
		if(env == NULL) return;
		env->entityComm = comm;
	}

	MPI_Comm Environment::GetEntityComm() 
	{
		if(env == NULL) return MPI_COMM_NULL;
		return env->entityComm;
	}

	void Environment::SetGlobalComm(MPI_Comm comm)
	{
		if(env == NULL) return;
		env->globalComm = comm;
	}

	MPI_Comm Environment::GetGlobalComm()
	{
		if(env == NULL) return MPI_COMM_NULL;
		return env->globalComm;
	}

	void Environment::SetNodeComm(MPI_Comm comm)
	{
		if(env == NULL) return;
		env->nodeComm = comm;
	}

	MPI_Comm Environment::GetNodeComm()
	{
		if(env == NULL) return MPI_COMM_NULL;
		return env->nodeComm;
	}

	bool Environment::HasServer() 
	{
		return (CoresPerNode() != ClientsPerNode());
	}

	unsigned int Environment::NumDomainsPerClient()
	{
		if(env == NULL) return 0;
		return env->model.architecture().cores().clients().domains();
	}

	int Environment::GetLastIteration()
	{
		return env->lastIteration;
	}

	bool Environment::StartNextIteration()
	{
		// for clients
		if((not HasServer()) || (IsClient())) {
			env->lastIteration++;
			return true;
		}

		// for servers
		static int locks = 0;
		locks += 1;
		if(locks == ClientsPerNode()) { 
			DBG("Iteration " << env->lastIteration << " terminated");
			env->lastIteration++;
			locks = 0;
			return true;
		}
		return false;
	}
}
