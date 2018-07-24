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

#ifndef __DAMARIS_CHANNEL_H
#define __DAMARIS_CHANNEL_H

#include <string.h>
#include <mpi.h>

#include "Damaris.h"

#include "damaris/util/Unused.hpp"
#include "damaris/util/ForwardDcl.hpp"
#include "damaris/util/Pointers.hpp"
#include "damaris/util/Deleter.hpp"
#include "damaris/util/Bind.hpp"
#include "damaris/util/Debug.hpp"

#include "damaris/comm/Callback.hpp"
#include "damaris/comm/Reactor.hpp"

namespace damaris 
{


/**
 * The Channel class is a wrapper to MPI send/recv functions to be used with
 * a Reactor object. The Reactor passed when creating the channel will take
 * care of checking for asynchronous send and receive, and call the right
 * callbacks.
 */
class Channel : public ENABLE_SHARED_FROM_THIS(Channel) {
	
	friend class Deleter<Channel>;

	private:
	MPI_Comm comm_; /*!< Communicator linking the 
			two entities of the channel. */
	int endpoint_;	/*!< Rank of the destination in the communicator. */
	std::weak_ptr<Reactor> reactor_; /*!< Pointer to the reactor that handles the
			asynchronous callbacks. Uses a weak_ptr to avoid cyclic
			references. */
	int max_async_; /*< Maximum number of pending async send. */
	int async_count_; /*< Current number of pending async send. */

	/**
	 * Constructor. Does nother and is private. The static New method should
	 * be used to create a new Channel.
	 */
	Channel() {}
	
	/**
	 * Destructor.
	 */
	virtual ~Channel() {}
		
	/**
	 * Blocking send. A matching Recv or AsyncRecv on the other side with
	 * the same tag will receive the message. This is basically just an
	 * MPI_Send.
	 *
	 * \param[in] tag : tag to send. The matching Receive should use the 
	 * same tag.
	 * \param[in] buf : pointer to the data to send.
	 * \param[in] count : number of bytes to send.
	 */
	int SimpleSend(int tag, const void* buf, int count) {
		if(endpoint_ == MPI_ANY_SOURCE) return DAMARIS_CHANNEL_ERROR;
		int err = MPI_Send((void*)buf,count,MPI_BYTE,endpoint_,tag,comm_);
		if(err == MPI_SUCCESS) return DAMARIS_OK;
		else return DAMARIS_MPI_ERROR;
	}
	
	/**
	 * Blocking receive. Basically just an MPI_Recv.
	 * 
	 * \param[in] tag : tag that must match to allow the receive.
	 * \param[in] buf : buffer in which to put the data.
	 * \param[in] count : number of bytes to receive.
	 */
	int SimpleRecv(int tag, void* buf, int count) {
		MPI_Status status;
		int err = MPI_Recv((void*)buf,count,MPI_BYTE,endpoint_,
				tag,comm_,&status);
		return (err == MPI_SUCCESS) ? DAMARIS_OK : DAMARIS_MPI_ERROR;
	}
	
	/**
	 * Non-blocking send. The callback should have the form
	 * f(int dest, const void* buffer, int size)
	 * It will be invoked with the initial buffer and count passed as
	 * parameters when calling this function.
	 * The buffer is not supposed to be modified or deleted before the
	 * callback is called.
	 *
	 * \param[in] tag : tag to match.
	 * \param[in] buf : pointer to the buffer to send.
	 * \param[in] count : number of bytes.
	 * \param[in] f : callback called when the send has completed.
	 */
	template<typename F>
	int SimpleAsyncSend(int tag, const void* buf, int count, F f) {
		if(max_async_ == 0) return DAMARIS_CHANNEL_ERROR;		

		std::shared_ptr<Reactor> r = reactor_.lock();
		if(not r) return DAMARIS_REACTOR_NOT_FOUND;
		
		// if queue is full, run the reactor until there is some space
		r->PollOne();
		while(async_count_ >= max_async_) {
			r->PollOne(); 
		}
		
		if(endpoint_ == MPI_ANY_SOURCE) return DAMARIS_CHANNEL_ERROR;
		std::shared_ptr<Callback> cb = Callback::New(f);
		MPI_Request req;
		int err = MPI_Isend((void*)buf,count,MPI_BYTE,endpoint_,
				tag,comm_,&req);
		if(err != MPI_SUCCESS) return DAMARIS_MPI_ERROR;
		
		int completed;
		MPI_Status status;
		err = MPI_Test(&req,&completed,&status);

		if(completed) {
			(*cb)(tag,endpoint_,buf,count);
			return DAMARIS_OK;
		} else {
			return r->BindRequest(req,cb,buf);
		}
	}
	
	private:
	
	void DefaultAsyncSendCallback(int UNUSED(tag), 
					int UNUSED(source), 
					const void* buf, int count) 
	{
		char* b = (char*)buf;
		if(buf != NULL && count != 0)
			delete[] b;
		async_count_ -= 1;
	}
	
	public:
	/**
	 * Second version of non-blocking send which doesn't take any callback.
	 * (We don't necessarily need to know that the asynchronous call
	 * completed). We still need periodic call to the Reactor's event
	 * processing loop in order not to overflow completed requests.
	 * This asynchronous send will actually make a copy of the buffer since
	 * there is no way to know when the orginal can be reused.
	 *
	 * \param[in] tag : tag to match.
	 * \param[in] buf : pointer to the buffer to send.
	 * \param[in] count : number of bytes.
	 */
	int SimpleAsyncSend(int tag, const void* buf, int count) {
		char* copy = new char[count];
		memcpy(copy,buf,count);
		async_count_ += 1;
		return SimpleAsyncSend(tag,copy,count,
			BIND(&Channel::DefaultAsyncSendCallback,
				this,_1,_2,_3,_4));
	}
	
	/**
	 * Non-blocking receive for a message with a specific tag.
	 * The callback should be of the form 
	 * f(int source, const void* buffer, int size)
	 *
	 * \param[in] tag : tag to match.
	 * \param[out] buf : buffer in which to receive the data.
	 * \param[in] count : maximum number of bytes to receive.
	 * \param[in] f : callback to call when the data is received.
	 */
	template<typename F>
	int SimpleAsyncRecv(int tag, void* buf, int count, F f) {
		std::shared_ptr<Reactor> r = reactor_.lock();
		if(not r) return DAMARIS_REACTOR_NOT_FOUND;
		std::shared_ptr<Callback> cb = Callback::New(f);
		MPI_Request req;
		int err = MPI_Irecv(buf,count,MPI_BYTE,endpoint_,
				tag,comm_,&req);
		if(err != MPI_SUCCESS) return DAMARIS_MPI_ERROR;
		return r->BindRequest(req,cb,buf);
	}
	
	public:
	
	/**
	 * Returns the rank of the destination in the inner communicator.
	 */
	int GetEndPoint() const {
		return endpoint_;
	}
	
	/**
	 * Returns the communicator used by the channel.
	 */
	MPI_Comm GetComm() const {
		return comm_;
	}
	
	/**
	 * Returns the Reactor used to create the Channel.
	 */
    std::shared_ptr<Reactor> GetReactor() const {
		return reactor_.lock();
	}
	
	/**
	 * Blocking send. A matching Recv or AsyncRecv on the other side with
	 * the same tag will receive the message. This is basically just an
	 * MPI_Send.
	 *
	 * \param[in] tag : tag to match the receive.
	 * \param[in] buf : buffer to send.
	 * \param[in] count : number of bytes to send.
	 */
	int Send(int tag, const void* buf, int count) {
		return SimpleSend(tag,buf,count);
	}
	
	/**
	 * Blocking send without data (only the tag matters).
	 *
	 * \param[in] tag : tag to be matched by the receiver.
	 */
	int Send(int tag) {
		return SimpleSend(tag,NULL,0);
	}
	
	/**
	 * Blocking receive. Basically just an MPI_Recv.
	 * 
	 * \param[in] tag : tag to match by the sender.
	 * \param[out] buf : buffer to receive the data.
	 * \param[in] count : number of bytes to receive.
	 */
	int Recv(int tag, void* buf, int count) {
		return SimpleRecv(tag,buf,count);
	}
	
	/**
	 * Blocking receive without data.
	 * 
	 * \param[in] tag : tag to match the send.
	 */
	int Recv(int tag) {
		return SimpleRecv(tag,NULL,0);
	}
	
	/**
	 * Non-blocking send. The callback should have the form
	 * f(int dest, const void* buffer, int size)
	 * It will be invoked with the initial buffer and count passed as
	 * parameters when calling this function.
	 * The buffer is not supposed to be modified or deleted before the
	 * callback is called.
	 * 
	 * \param[in] tag : tag to be matched by the receiver.
	 * \param[in] buf : buffer to send.
	 * \param[in] count : number of bytes to send.
	 * \param[in] f : callback to call when the send completes.
	 */
	template<typename F>
	int AsyncSend(int tag, const void* buf, int count, F f) {
		return SimpleAsyncSend<F>(tag,buf,count,f);
	}
	
	/**
	 * Non-blocking send without attached data (only the tag matters).
	 * 
	 * \param[in] tag : tag to be matched by the receiver.
	 * \param[in] f : callback to call when the send completes.
	 */
	template<typename F>
	int AsyncSend(int tag, F f) {
		return SimpleAsyncSend(tag,NULL,0,f);
	}
	
	/**
	 * Second version of non-blocking send which doesn't take any callback.
	 * (We don't necessarily need to know that the asynchronous call
	 * completed). We still need periodic call to the Reactor's event
	 * processing loop in order not to overflow completed requests.
	 *
	 * \param[in] tag : tag to be matched by the receiver.
	 * \param[in] buf : buffer to send.
	 * \param[in] count : number of bytes to send.
	 */
	int AsyncSend(int tag, const void* buf, int count) {
		return SimpleAsyncSend(tag,buf,count);
	}
	
	/**
	 * Non-blocking send without data and without callback.
	 * 
	 * \param[in] tag : tag to be matched by the receiver.
	 */
	int AsyncSend(int tag) {
		return SimpleAsyncSend(tag,NULL,0);
	}
	
	/**
	 * Non-blocking receive for a message with a specific tag.
	 * The callback should be of the form 
	 * f(int source, const void* buffer, int size)
	 *
	 * \param[in] tag : tag to be matched by the sender.
	 * \param[out] buf : buffer in which to store the result.
	 * \param[in] count : number of bytes to receive.
	 * \param[in] f : callback to be called when done receiving.
	 */
	template<typename F>
	int AsyncRecv(int tag, void* buf, int count, F f) {
		return SimpleAsyncRecv<F>(tag,buf,count,f);
	}
	
	/**
	 * Non-blocking receive for a message with a specific tag, but
	 * without data.
	 *
	 * \param[in] tag : tag to be matched by the sender.
	 * \param[in] f : callback to be called when done receiving.
	 */
	template<typename F>
	int AsyncRecv(int tag, F f) {
		return SimpleAsyncRecv(tag,NULL,0,f);
	}
	
	/**
	 * Creates a new Channel, given a Reactor, a communicator and
	 * a remote endpoint. The endpoint is a rank within the given
	 * communicator. This rank can be the rank of the process itself.
	 *
	 * \param[in] reactor : valid reactor instance.
	 * \param[in] c : communicator to use for communications.
	 * \param[in] endpoint : remote endpoint.
	 */
	 static std::shared_ptr<Channel> New(const std::shared_ptr<Reactor>& reactor,
					MPI_Comm c, int endpoint, int max_async)
	{
        if(not reactor) return std::shared_ptr<Channel>();
		int size;
		MPI_Comm_size(c,&size);
		if(((endpoint >= size) || (endpoint < 0)) 
			&& (not (endpoint == MPI_ANY_SOURCE))) {
	 	 return std::shared_ptr<Channel>();
		}
		
        std::shared_ptr<Channel> p(new Channel(), Deleter<Channel>());
		p->comm_ = c;
		p->endpoint_ = endpoint;
		p->reactor_ = reactor;
		p->max_async_ = max_async;
		p->async_count_ = 0;
		return p;
	}
};
	
}

#endif
