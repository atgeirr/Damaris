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
 * \file VisItListener.cpp
 * \date May 2012
 * \author Matthieu Dorier
 * \version 0.5
 */
#include <mpi.h>
#include <VisItControlInterface_V2.h>
#include <boost/bind.hpp>

#include "common/Debug.hpp"
#include "viz/VisItListener.hpp"

namespace Damaris {
namespace Viz {

void VisItListener::init(MPI_Comm c)
{
	comm = c;
}

int VisItListener::connected()
{
	int visitstate = VisItDetectInput(0, -1);
	if(visitstate >= -5 && visitstate <= -1) {
		ERROR("uncaught error");
	}
	return visitstate;
}

int VisItListener::enterSyncSection(int visitstate)
{
	switch(visitstate) {
		case 1:
			if(VisItAttemptToCompleteConnection()) {
				INFO("VisIt connected");
				VisItSetSlaveProcessCallback(&VisItListener::slaveProcessCallback);
			}
			break;
		case 2:
			if(!processVisItCommand()) {
				VisItDisconnect();
			}
			break;
	}
	return 0;
}

void VisItListener::broadcastSlaveCommand(int *command)
{
	MPI_Bcast(command, 1, MPI_INT, 0, comm);
}


void VisItListener::slaveProcessCallback()
{
	int command = 0;
	broadcastSlaveCommand(&command);
}

bool VisItListener::processVisItCommand()
{
	return 1;
}
}
}
