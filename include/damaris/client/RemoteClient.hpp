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
#ifndef __DAMARIS_REMOTE_CLIENT_H
#define __DAMARIS_REMOTE_CLIENT_H

#include <string>
#include <vector>
#include <stdint.h>

#include "damaris/util/Deleter.hpp"
#include "damaris/util/Pointers.hpp"
#include "damaris/comm/Channel.hpp"
#include "damaris/comm/Reactor.hpp"

namespace damaris {


/**
 * The RemoteClient object represents a single core running the simulation which
 * belongs to the computation node. It exposes functions allowing a simulation to
 * write data and send signals to a dedicated node.
 */
class RemoteClient : public Client {

	friend class Deleter<RemoteClient>;

	//protected:

	/**
	 * Constructor.
	 */
	RemoteClient() : Client()  {}

	/**
	 * Destructor.
	 */
	virtual ~RemoteClient() {}

	public:

	/**
	 * Constructor. Creates a RemoteClient object given a communicator gathering
	 * all clients.
	 */
	static std::shared_ptr<Client> New(MPI_Comm comm) {
		std::shared_ptr<Client> c(new RemoteClient(),Deleter<RemoteClient>());
		//c->reactor_ = Reactor::New(comm);
                c->SetReactor(c,comm);
		return c;
	}

	/**
	 * Overwrites the Write function of the Client class.
	 * First, it copies the data associated to the variable
	 * into shared memory, then sends it to the dedicated node.
	 *
	 * \param[in] varname : name of the variable.
	 * \param[in] block : block id.
	 * \param[in] data : pointer to the data.
	 * \param[in] blocking : whether or not to block when the memory is full.
	 */
	virtual int Write(const std::string &varname,
			int32_t block, const void* data,
			bool blocking = false);

	/**
	 * Allocates a buffer in shared memory so it can be written after by
	 * the simulation. Requires a call to Commit to notify the dedicated
	 * core that the variable has been written.
	 *
	 * \param[in] varname : name of the variable to write.
	 * \param[in] block : id of the block.
	 * \param[in] buffer : pointer to the allocated memory.
	 * \param[in] blocking : wether to block when the memory is full.
	 */
	virtual int Alloc(const std::string & varname,
			int32_t block, void** buffer, bool blocking = true);

	/**
	 * Commit a variable, i.e. sets it as read-only for the simulation and
	 * send a pointer to the dedicated core.
	 *
	 * \param[in] varname : name of the variable.
	 * \param[in] block : block id.
	 * \param[in] iteration : iteration number of the block to commit.
	 */
	virtual int Commit(const std::string & varname,
			int32_t block, int32_t iteration = -1);


};

}

#endif
