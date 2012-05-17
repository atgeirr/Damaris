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
		entityComm = MPI_COMM_NULL;
		globalComm = MPI_COMM_NULL;
		nodeComm   = MPI_COMM_NULL;
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

	int Environment::getCoresPerNode() 
	{
		if(env == NULL) return -1;
		return env->model.architecture().cores().count();
	}

	int Environment::getClientsPerNode() 
	{
		if(env == NULL) return -1;
		return env->model.architecture().cores().clients().count();
	}

	std::string Environment::getSimulationName()
	{
		if(env == NULL) return "unknown";
		return env->model.name();
	}

	Model::Language Environment::getDefaultLanguage()
	{
		if(env == NULL) return Model::Language::unknown;
		return env->model.language();
	}

	void Environment::setEntityComm(MPI_Comm comm) 
	{ 
		if(env == NULL) return;
		env->entityComm = comm;
	}

	MPI_Comm Environment::getEntityComm() 
	{
		if(env == NULL) return MPI_COMM_NULL;
		return env->entityComm;
	}

	void Environment::setGlobalComm(MPI_Comm comm)
	{
		if(env == NULL) return;
		env->globalComm = comm;
	}

	MPI_Comm Environment::getGlobalComm()
	{
		if(env == NULL) return MPI_COMM_NULL;
		return env->globalComm;
	}

	void Environment::setNodeComm(MPI_Comm comm)
	{
		if(env == NULL) return;
		env->nodeComm = comm;
	}

	MPI_Comm Environment::getNodeComm()
	{
		if(env == NULL) return MPI_COMM_NULL;
		return env->nodeComm;
	}

	bool Environment::hasServer() 
	{
		return (getCoresPerNode() != getClientsPerNode());
	}
}
