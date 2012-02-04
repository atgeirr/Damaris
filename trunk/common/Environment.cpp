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
#include "common/Debug.hpp"
#include "common/Environment.hpp"

namespace Damaris {

	Environment::Environment(Model::SimulationModel* mdl)
	: Configurable<Environment,Model::SimulationModel>(mdl)
	{
		init();
	}

	Environment::~Environment() 
	{ }

	void Environment::init()
	{
		entityComm = MPI_COMM_NULL;
		globalComm = MPI_COMM_NULL;
		nodeComm   = MPI_COMM_NULL;
	}

	int Environment::getCoresPerNode() const
	{
		return model->architecture().cores().count();
	}

	int Environment::getClientsPerNode() const
	{
		return model->architecture().cores().clients().count();
	}

	const std::string & Environment::getSimulationName() const
	{
		return model->name();
	}

	Language::language_e Environment::getDefaultLanguage() const
	{
		return Language::getLanguageFromString(&(model->language()));
	}
}
