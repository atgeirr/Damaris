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

#ifndef __DAMARIS_REACTOR_H
#define __DAMARIS_REACTOR_H

#include <map>
#include <vector>
#include <mpi.h>

#include "Damaris.h"

#include "damaris/util/Pointers.hpp"
#include "damaris/util/Deleter.hpp"
#include "damaris/util/ForwardDcl.hpp"

#include "damaris/comm/Callback.hpp"

namespace damaris
{

USING_POINTERS;
	
class Reactor : public ENABLE_SHARED_FROM_THIS(Reactor) {
	
	friend class Deleter<Reactor>;
	friend class Channel;
	
	protected:
	
	enum Message {
		BCAST, 		// tag used for broadcast messages
		DISPATCH,	// tag used for dispatch messages
		SYNC,		// tag used for sync messages
		SYNC_WAVE	// second tag used for sync messages
	};
	
	typedef struct {
		int root;
		int tag;
		int count;
	} MsgInfo;
	
	// -------------------------------------------------------------- //
	// attributes necessary for serving channel's requests
	// -------------------------------------------------------------- //
	bool running_;	// running or not
	int rank_;	// rank in the communicator
	int size_;	// size of the communicator
	MPI_Comm comm_;	// communicator of the reactor
	std::map<int,shared_ptr<Callback> > int_callbacks_;
	
	// -------------------------------------------------------------- //
	// attributes necessary for serving channel's requests
	// -------------------------------------------------------------- //
	std::vector<MPI_Request> 		requests_;
	std::vector<shared_ptr<Callback> > 	ext_callbacks_;
	std::vector<char*>	 		buffers_;
	
	// -------------------------------------------------------------- //
	// attributes necessary for the dispatch	
	// -------------------------------------------------------------- //
	shared_ptr<Channel> everybody_;
	MsgInfo	dispatch_info_;
	
	/**
	 * Constructor. Not supposed to be called anywhere else than from
	 * the static New function.
	 */
	Reactor() {}
	
	/**
	 * Called by New to initialize the brodcast tree.
	 */
	bool Init(MPI_Comm comm);
	
	/**
	 * Destructor. Will cancel all requests associated to async operations.
	 */
	virtual ~Reactor() {
		MPI_Request* r = &(requests_[0]);
		for(int i=0; i < (int)requests_.size(); i++) {
			MPI_Cancel(r+i);
		}
		requests_.resize(0);
	}
	
	/**
	 * Called by a Channel or by the Reactor itself to associate a 
	 * request with a callback to execute.
	 * 
	 * \param[in] req : Request object returned by an async MPI call.
	 * \param[in] cb : callback to call when the request completes.
	 * \param[in] buffer : buffer associated to the request (send or
	 * recv buffer).
	 */
	int BindRequest(MPI_Request req, const shared_ptr<Callback>& cb, 
			const void* buffer)
	{
		requests_.push_back(req);
		ext_callbacks_.push_back(cb);
		buffers_.push_back((char*)buffer);
	
		return DAMARIS_OK;
	}
	
	/**
	 * Removes a completed request from the reactor. This is an internal
	 * function taking the index of the request in the array that stores it.
	 * 
	 * \param[in] index : index of the request to delete.
	 */
	bool RemoveRequest(unsigned int index);
	
	/**
	 * This function is used as a callback associated to the tag DISPATCH.
	 * 
	 * \param[in] tag : tag to dispatch.
	 * \param[in] source : source that sent the message.
	 * \param[in] buf : buffer sent.
	 * \param[in] count : number of bytes sent.
	 */
	void DispatchCallback(int tag, int source, const void* buf, int count);
	
public:
	
	/**
	 * Sends a message to a specific destination in the communicator
	 * provided at creation time. The syntax is similar to MPI_Send,
	 * except for the communicator that has been provided when creating
	 * the Reactor. When received by the reactor on the remote process,
	 * the tag will be examined to determine the right callback to
	 * call (this callback has been provided through the Bind function).
	 *
	 * \param[in] tag : tag to be matched.
	 * \param[in] dest : destination in the communicator.
	 * \param[in] buf : buffer to send.
	 * \param[in] count : number of bytes to send.
	 */
	virtual bool Dispatch(int tag, int dest, const void* buf, int count);

	/**
	 * When all processes in the communicator have called this
	 * function with the same tag, the callback associated with this tag
	 * will be called.
	 * The callback will be called at each process with the parameters
	 * that they gave (parameters may not be the same in each process).
	 * This function follows an Atomic Broadcast semantics, i.e. if
	 * the processes use Sync twice with a tag 1 and tag 2, potentially
	 * in a different order in each process, it is ensured that all
	 * processes will choose a unique order to call the callbacks.
	 *
	 * \param[in] tag : tag to be matched by all processes sync'ing.
	 */
	virtual bool Sync(int tag) = 0;

	/**
	 * Broadcast a message to everybody in the communicator.
	 * The tag provided must be associated to a callback in each process.
	 * This primitive is a uniform broadcast: if two processes A and B
	 * broadcast a message, it is ensured that all the processes will
	 * call the associated callbacks in the same order.
	 * Hence, this function may return before the callback associated
	 * to the tag is effectively called.
	 */
	virtual bool Broadcast(int tag, const void* buf, int count) = 0;
	
	/**
	 * Execute ready handlers until no more handlers are ready.
	 * Returns the number of handlers executed.
	 */
	virtual int Poll();

	/**
	 * Execute one handler if ready.
	 * Returns 1 if the handler has been executed, 0 otherwise.*/
	virtual int PollOne();

	/**
	 * Run the Reactor's processing loop.
	 */
	virtual void Run();

	/**
	 * Runs exactly one handler.
	 */
	virtual void RunOne();
	
	/**
	 * Executes a maximum of n handlers.
	 *
	 * \param[in] n : maximum number of handlers to run.
	 */
	virtual void PollSome(int n) {
		for(int i=0; i<n; i++) 
			if(PollOne() == 0) break;
	}
	
	/**
	 * Stops the loop initiated by Poll or Run.
	 */
	virtual void Stop();

	/**
	 * Binds a callback f to a tag.
	 */
	template<typename T>
	bool Bind(int tag, T f) {
		int_callbacks_[tag] = Callback::New<T>(f);
		return true;
	}
	
	/**
	 * Creates a new Reactor from an MPI_Comm; this call is collective
	 * accross the processes of the communicator, it will duplicate the
	 * communicator.
	 */
	static shared_ptr<Reactor> New(MPI_Comm comm, int radix=2);
};

}

#endif
