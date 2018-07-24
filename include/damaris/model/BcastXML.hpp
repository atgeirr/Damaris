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

#include <string>
#include <mpi.h>

#include "damaris/util/Pointers.hpp"
#include "damaris/model/Model.hpp"

namespace damaris {
namespace model {


/**
 * Loads an XML file in the rank 0 of the given MPI communicator, broadcast
 * its content to other processes that will all parse it and return a model.
 *
 * \param[in] comm : MPI communicator gather all processes 
	that need to load the XML file.
 * \param[in] uri : path of the XML file.
 */
std::shared_ptr<Simulation> BcastXML(const MPI_Comm& comm, const std::string& uri);

/**
 * Loads the XML file without involving any other processes.
 *
 * \param[in] uri : path of the XML file.
 */
std::shared_ptr<Simulation> LoadXML(const std::string& uri);

}
}
