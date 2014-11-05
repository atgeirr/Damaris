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
#ifndef __DAMARIS_CLIENT_H
#define __DAMARIS_CLIENT_H

#include <string>
#include <vector>
#include <stdint.h>

#include "damaris/util/Deleter.hpp"
#include "damaris/util/Pointers.hpp"
#include "damaris/comm/Channel.hpp"
#include "damaris/comm/Reactor.hpp"

namespace damaris {

USING_POINTERS;
	
/**
 * The Client object represents a single core running the simulation. It
 * exposes functions allowing a simulation to write data and send signals to
 * a dedicated core. This Client object is not supposed to work in
 * a standalone mode (when no core is dedicated). For this purpose, see
 * the StandaloneClient class.
 */
class Client {

	friend class Deleter<Client>;
	
	protected:
		
	int errorOccured_; /*!< Indicates wether an error 
			occured during the last iteration. */
	shared_ptr<Reactor> reactor_;   /*!< Reactor to make channels work. */
	shared_ptr<Channel> ch2server_; /*!< Channel to the server. */
	
	/** 
	 * Constructor.
	 */
	Client() : errorOccured_(0) {}
	
	/**
	 * Destructor.
	 */
	virtual ~Client() {}

	public:
	
	/**
	 * Constructor. Creates a Client object given a communicator gathering 
	 * all clients.
	 */
	static shared_ptr<Client> New(MPI_Comm comm) {
		shared_ptr<Client> c(new Client(),Deleter<Client>());
		c->reactor_ = Reactor::New(comm);
		return c;
	}

	/**
	 * Returns the Reactor gathering clients.
	 */
	shared_ptr<Reactor> GetReactor() const {
		return reactor_;
	}

		virtual void SetReactor(shared_ptr<Client> c , MPI_Comm comm) {
		c->reactor_ = Reactor::New(comm);
	}
	/**
	 * Sets the channel that has to be used to communicate with the server.
	 */
	virtual bool SetChannelToServer(const shared_ptr<Channel>& ch)
	{
		if(ch2server_) return false;
		ch2server_ = ch;
		return true;
	}
	
	/**
	 * Sends a message to the dedicated core to make this core known to it.
	 * Should be called before any other functions interacting with the
	 * dedicated cores.
	 */
	virtual int Connect();

	/**
	 * Writes a particular block of a variable into shared memory.
	 *
	 * \param[in] varname : name of the variable.
	 * \param[in] block : block id.
	 * \param[in] data : pointer to the data.
	 * \param[in] blocking : wether or not to block when the memory is full.
	 */
	virtual int Write(const std::string &varname,
			int32_t block, const void* data,
			bool blocking = false);
	
	/**
	 * Sends a signal to the dedicated core.
	 * 
	 * \param[in] signame : name of the signal to send.
	 */
	virtual int Signal(const std::string & signame);

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
	
	/**
	 * Clear a variable, i.e. sets it back to read/write and removes it
	 * from the index of the client. This does NOT remove it from memory.
	 * The dedicated core is responsible for freeing the memory.
	 *
	 * \param[in] varname : name of the variable.
	 * \param[in] block : id of the block.
	 * \param[in] iteration : iteration number of the block to clear.
	 */
	virtual int Clear(const std::string & varname, 
			int32_t block, int32_t iteration = -1);

	/**
	 * Set the position of a variable within a global space.
	 *
	 * \param[in] varname : name of the variable.
	 * \param[in] block : id of the block.
	 * \param[in] position : an array of offsets (should contain as many 
	 * items as the variable has dimensions).
	 */
	virtual int SetPosition(const std::string& varname, int32_t block,
			const int64_t* position);

	/**
	 * Sends a signal to the server to shut it down (all clients in node 
	 * need to call this function before the server is actually stopped.
	 */
	virtual int StopServer();

	/**
	 * Indicates that the iteration has terminated, this will potentially
	 * update connected backends such as VisIt.
	 */
	virtual int EndIteration();

};

}

#endif
