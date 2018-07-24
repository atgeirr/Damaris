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
#ifndef __DAMARIS_SERVER_H
#define __DAMARIS_SERVER_H

#include <map>
#include <string>
#include <mpi.h>

#include "Damaris.h"

#include "damaris/util/Pointers.hpp"
#include "damaris/util/Deleter.hpp"
#include "damaris/util/Bind.hpp"
#include "damaris/msg/Signals.hpp"
#include "damaris/comm/Channel.hpp"
#include "damaris/comm/Reactor.hpp"

namespace damaris {


class Server {
	
friend class Deleter<Server>;

	private:
		
	std::shared_ptr<Reactor> reactor_; /*!< Reactor gathering servers. */
	std::map<int,std::shared_ptr<Channel> > clients_; /*!< Channels to 
						communicate with clients. */
	
	std::map<int,HeaderMessage> headerMsg_; /*!< Headers sent from clients. */
	
	bool needStop_; /*!< Whether the server should stop running. */
	bool firstRun_; /*!< Whether this is the first time running the server.*/
	
#ifdef HAVE_VISIT_ENABLED
	int visitIteration_; /*!< Iteration to use for VisIt. */
#endif
	
	/**
	 * Constructor.
	 */
	Server() : needStop_(false), firstRun_(true) {}
	
	/**
	 * Destructor.
	 * Deletes all related objects (configuration, environment, 
	 * shared memory objects).
	 */
	virtual ~Server() {
		Stop();
	}
		
	// Callbacks associated to events sent by the clients
	void OnHeader(int,int,const void*, int);
	void OnConnect(int source);
	void OnStop(int source);
	void OnNextIteration(int source);
	void OnNextIterationWithErrors(int source);
	void OnCallAction(const std::shared_ptr<Channel>& ch, int source);
	void OnBcastAction(const std::shared_ptr<Channel>& ch, int source);
	void OnWrite(const std::shared_ptr<Channel>& ch, int source);
	void OnRemoteWrite(const std::shared_ptr<Channel>& ch, int source);
	void OnCommit(const std::shared_ptr<Channel>& ch, int source);
	void OnClear(const std::shared_ptr<Channel>& ch, int source);

	// Callbacks associated to interactions with other servers
	static void BcastEventCallback(
			int tag, int source, const void* buf, int count);
	void EndOfIterationCallback(
			int tag, int source, const void* buf, int count);

	public:
		
	/**
	 * Returns the reactor connecting all servers.
	 */
	std::shared_ptr<Reactor> GetReactor() const {
		return reactor_;
	}
		
	/**
	 * Adds a Channel associated to a client, binds tags to
	 * callbacks and starts listening to it asynchronously.
	 *
	 * \param[in] ch : Channel to add.
	 */
	bool AddChannelToClient(const std::shared_ptr<Channel>& ch);

	/** 
	 * Enters in the server main loop. Won't stop until Stop() is called.
	 */
	void Run();

	/** 
	 * Forces the server to stop after the next iteration of
	 * the main loop. Does not remove the shared memory 
	 * structures (the server can run again if necessary).
	 * This function will usually be called by the OnStop callback.
	 */
	void Stop() {
		needStop_ = true;
		reactor_->Stop();
	}
		
	/**
	 * This static function is the normal way of retrieving
	 * an instance over a Server object. It creates the server from an MPI
	 * communicator gathering all servers.
	 *
	 * \param[in] comm : MPI communicator gathering all servers.
	 */
	static std::shared_ptr<Server> New(MPI_Comm comm) {
		std::shared_ptr<Server> s(new Server(), Deleter<Server>());
		s->reactor_ = Reactor::New(comm);
		return s;
	}

}; // class Server

} // namespace damaris

#endif
