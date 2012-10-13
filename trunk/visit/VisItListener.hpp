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

#include "xml/Model.hpp"

namespace Damaris {
namespace Viz {

/**
 * This class is the core of the VisIt-Damaris connection.
 */
class VisItListener {

	private:

		/**
		 * SimData is a structure providing a few information
		 * about the current status of the simulation. One single instance
		 * exists and is passed to VisIt's callback functions.
		 */
		struct SimData {
			int iteration;
		};

		static SimData sim; /*!< Single instance of SimData passed to VisIt's callbacks. */
		static MPI_Comm comm; /*< Main communicator to use when working in parallel. */


		/**
		 * This function is called at every time-step,
		 * it contains the collective communications required to
		 * exchange data reauired by VisIt.
		 */
		static int TimeStepCallback(void* cbdata);

		/**
		 * VisIt command callback.
		 */
		static void BroadcastSlaveCommand(int *command);
		/**
		 * VisIt command callback.
		 */
		static void SlaveProcessCallback();
		/**
		 * VisIt command callback.
		 */
		static bool ProcessVisItCommand();
		
		/**
		 * Helper function to broadcast integers.
		 */
		static int BroadcastIntCallback(int *value, int sender);
	
		/**
		 * Helper function to broadcast strings.
		 */
		static int BroadcastStringCallback(char *str, int len, int sender);

		/**
		 * VisIt command callback.
		 */
		static void ControlCommandCallback(const char *cmd, const char *args, void *cbdata);

		/**
		 * VisIt metadata callback.
		 */
		static visit_handle GetMetaData(void *cbdata);
		/**
		 * VisIt Mesh data callback.
		 */
		static visit_handle GetMesh(int domain, const char *name, void *cbdata);
		/**
		 * VisIt Variable data callback.
		 */
		static visit_handle GetVariable(int domain, const char *name, void *cbdata);
		/**
		 * VisIt Domain data callback.
		 */
		static visit_handle GetDomainList(const char* name, void* cbdata);

	public:
		/**
		 * Initializes the VisItListener part.
		 * \param[in] c : Communicator used for the communications between VisIt engines.
		 * \param[in] mdl : Model from the XML file.
		 * \param[in] simname : name of the simulation.
		 */
		static void Init(MPI_Comm c, const Model::Simulation::visit_optional& mdl, 
						const std::string& simname);

		/**
		 * This function has to be called only by process rank 0. It tests if Visit
		 * tried to connect to the simulation. If yes, it returns a positive integer.
		 * This integer should be passed to EnterSyncSection after having synchronized
		 * all the concerned processes.
		 */
		static int Connected();

		/**
		 * This function must be called by all the VisIt engines at the same time,
		 * so a synchronization is required prior to calling it. It should be given
		 * a visitstat, which is the value returned by Connected.
		 */
		static int EnterSyncSection(int visitstat);

		/**
		 * This function must be called when the iteration has changed. It updates
		 * the plots on the viewer side.
		 */
		static int Update();
};

}
}
#endif
