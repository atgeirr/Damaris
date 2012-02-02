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

/*
	const std::string & Environment::getMsgQueueName() const
	{
		return model->queue().name();
	}
*/
/*
	size_t Environment::getMsgQueueSize() const
	{
		return (size_t)(baseModel->architecture().queue().size());
	}
*/
/*
	const std::string & Environment::getSegmentName() const
	{
		return baseModel->architecture().buffer().name();
	}
*/
/*
	size_t Environment::getSegmentSize() const
	{
		return (size_t)(baseModel->architecture().buffer().size());
	}
*/
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
