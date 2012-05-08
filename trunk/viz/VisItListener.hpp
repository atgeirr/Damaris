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
 * \file VisItListener.hpp
 * \date May 2012
 * \author Matthieu Dorier
 * \version 0.5
 */
#ifndef __DAMARIS_VISIT_LISTENER_H
#define __DAMARIS_VISIT_LISTENER_H

#include <mpi.h>

namespace Damaris {
namespace Viz {

/**
 * This class is the core of the VisIt-Damaris connection.
 */
class VisItListener {

	private:

		static MPI_Comm comm;

		static void broadcastSlaveCommand(int *command);
		static void slaveProcessCallback();
		static bool processVisItCommand();

	public:
		static void init(MPI_Comm c);

		static int connected();

		static int enterSyncSection(int visitstat);

};

}
}
#endif
