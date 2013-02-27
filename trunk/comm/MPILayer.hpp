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
 * \file MPILayer.hpp
 * \date February 2013
 * \author Matthieu Dorier
 * \version 0.7.1
 */

#ifndef __DAMARIS_MPI_LAYER_H
#define __DAMARIS_MPI_LAYER_H

#include <mpi.h>
#include <list>
#include <set>
#include <boost/shared_ptr.hpp>
#include "comm/Communication.hpp"
#include "core/Debug.hpp"

namespace Damaris {

/**
 * This class represents an asynchronous communication layer using MPI.
 * In particular it provides some functions that are missing from the
 * MPI-2 standard, such as non-blocking broadcast.
 */
template<typename MSG>
class MPILayer : public Communication<MSG> {

	private:
		enum tag { TAG_SEND    = 0, /*!< the message is sent. */
			   TAG_BCAST   = 1, /*!< the message is broadcasted. */
			   TAG_FWBCAST = 2, /*!< the message is forwarded to rank 0 for broadcast. */ 
			   TAG_SYNC    = 3};/*!< the message is a synchronization message. */ 

		/**
		 * This structure is used to store messages received by other processes
		 * in the Sync algorithm.
		 */
		struct sync_msg {
			MSG msg; /*!< the message to synchronize. */
			int lc; /*!< number of sync requests sent by the left child in the tree. */
			int rc; /*!< number of sync requests sent by the right child in the tree. */
			int me; /*!< number of sync requested by this process. */

			/**
			 * Constructor from a MSG.
			 */
			sync_msg(const MSG& m) 
			: msg(m), lc(0), rc(0), me(0) {}

			/**
			 * Copy constructor.
			 */
			sync_msg(const sync_msg& sm)
			: msg(sm.msg), lc(sm.lc), rc(sm.rc), me(sm.me) {}

			/**
			 * Implementation of == so that only messages are compared.
			 */
			bool operator==(const sync_msg& sm) const
			{
				return msg == sm.msg;
			}

			/**
			 * Implementation of < so that only messages are compared.
			 */
			bool operator<(const sync_msg& sm) const
			{
				return msg < sm.msg;
			}
	
			/**
			 * Return true is the synchronization is ready to be
			 * forwarded to the parent in the tree.
			 */
			bool ready() const {
				return (lc > 0) && (rc > 0) && (me > 0);
			}

			/**
			 * Decrease the synchronization requests. Should be called
			 * when the request is ready.
			 */
			void decrease() {
				lc--; rc--, me--;
			}
	
			/**
			 * A request is dead if nobody requested to sync, in which case
			 * it can be deleted from the set of current sync requests.
			 */
			bool dead() const {
				return (lc == 0) && (rc == 0) && (me == 0);
			}
		};

		MPI_Comm comm; /*!< Communicator through which sending messages. */
		std::list<MSG> toDeliver; /*!< List of messages ready to be delivered. */
		std::list<boost::shared_ptr<MPI_Request> > 
			pendingSendReq; /*!< List of requests associated to message sent. */
		std::set<sync_msg> 
			pendingBarriers; /*!< Messages associated to current non-bocking barriers. */

		int rank; /*!< Rank of the process in the communicator. */
		int size; /*!< Size of the communicator. */

		/**
		 * Constructor. The constructor is private, use New to create an instance.
		 */
		MPILayer(const MPI_Comm &c);
		
		/**
		 * Destructor (private)
		 */
		virtual ~MPILayer();

	public:
		/**
		 * Creates a new MPILayer object with a given communicator.
		 */
		static MPILayer* New(const MPI_Comm &c);
		
		/**
		 * Deletes the given MPILayer instance.
		 */
		static void Delete(MPILayer* l);

		/**
		 * \see Communication::Update
		 */
		void Update(unsigned int n = 1);

		/**
		 * \see Communication::Send
		 */
		void Send(int receiver, MSG m);

		/**
		 * \see Communication::Deliver
		 */
		bool Deliver(MSG* m);

		/**
		 * \see Communication::Bcast
		 */
		void Bcast(MSG m);
	
		/**
		 * \see Communication::Sync
		 */
		void Sync(MSG m);
};


template<typename MSG>
MPILayer<MSG>* MPILayer<MSG>::New(const MPI_Comm &c)
{
	return new MPILayer<MSG>(c);
}

template<typename MSG>
void MPILayer<MSG>::Delete(MPILayer *l)
{
	if(l != NULL) {
		delete l;
	}
}

template<typename MSG>
MPILayer<MSG>::MPILayer(const MPI_Comm &c)
{
	MPI_Comm_dup(c,&comm);
	MPI_Comm_rank(comm,&rank);
	MPI_Comm_size(comm,&size);
}

template<typename MSG>
MPILayer<MSG>::~MPILayer()
{
	MPI_Comm_free(&comm);
}

template<typename MSG>
void MPILayer<MSG>::Update(unsigned int n)
{
	static MSG m;
	static MPI_Request request;
	static MPI_Status status;
	static bool listening = false;

	// start by deleting one request of a previously sent message
	// if the request is obsolete, otherwise put it back in the queue
	if(!pendingSendReq.empty()) {
		boost::shared_ptr<MPI_Request> pending = pendingSendReq.front();
		pendingSendReq.pop_front();
		int done;
		MPI_Status s;
		MPI_Test(pending.get(),&done,&s);
		if(!done) {
			pendingSendReq.push_back(pending);
		}
	}

	// if we haven't started an MPI_IRecv yet, we do it now
	if(!listening) {
		MPI_Irecv(&m, sizeof(MSG), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, comm, &request);
		listening = true;
	}

	// we check the status of the last MPI_Irecv request
	int done;
	MPI_Test(&request,&done,&status);
	if(done) {
		// if the tag is a TAG_BCAST or TAG_FWBCAST, forward to child processes
		if(status.MPI_TAG == TAG_BCAST || status.MPI_TAG == TAG_FWBCAST) {
			int c1 = rank*2 + 1;
			int c2 = rank*2 + 2;
			if(c1 < size) {
				MPI_Request *pending = new MPI_Request;
				MPI_Isend(&m,sizeof(MSG),MPI_BYTE, c1, TAG_BCAST, comm, pending);
				pendingSendReq.push_back(boost::shared_ptr<MPI_Request>(pending));
			}
			if(c2 < size) {
				MPI_Request *pending = new MPI_Request;
				MPI_Isend(&m,sizeof(MSG),MPI_BYTE, c2, TAG_BCAST, comm, pending);
				pendingSendReq.push_back(boost::shared_ptr<MPI_Request>(pending));
			}
			toDeliver.push_back(m);
		}

		// if the tag is TAG_SEND
		if(status.MPI_TAG == TAG_SEND) {
			toDeliver.push_back(m);
		}

		// if the tag is TAG_SYNC
		if(status.MPI_TAG == TAG_SYNC) {
			int parent = (rank-1)/2;
			// node process, search for a valid entry in pending barriers
			typename std::set<sync_msg>::iterator it = pendingBarriers.find(sync_msg(m));
			if(it == pendingBarriers.end()) {
				sync_msg sm(m);
				if(2*(rank+1) >= size) sm.rc++;
				pendingBarriers.insert(sm);
			}
			// here we are sure a sync_msg is present in the set for this barrier
			it = pendingBarriers.find(sync_msg(m));
			sync_msg sm = *it;
			pendingBarriers.erase(it);
			// increment the number of sync calls from child process
			if(status.MPI_SOURCE == 2*(rank+1)) sm.rc++;
			if(status.MPI_SOURCE == 2*rank+1) sm.lc++;
			// check if the sync barrier is ready on this process
			if(sm.ready()) {
				sm.decrease();
				// if we have reached rank 0, broadcast the message
				if(rank == 0) {
					Bcast(sm.msg);
				} else {
					// otherwise, send to parent
					MPI_Request* pending = new MPI_Request;
					MPI_Isend(&m,sizeof(MSG),MPI_BYTE,parent, TAG_SYNC, comm, pending);
					pendingSendReq.push_back(boost::shared_ptr<MPI_Request>(pending));
				}
				// if the barrier is not dead,  put it back in the set
				if(!sm.dead()) {
					pendingBarriers.insert(sm);
				}
			} else {
				pendingBarriers.insert(sm);
			}
		}
		listening = false;

		if(n > 1) Update(n-1);
	}
}

template<typename MSG>
void MPILayer<MSG>::Send(int recvid, MSG m)
{
	if(recvid == rank) {
		toDeliver.push_back(m);
	} else {
		MPI_Request* pending = new MPI_Request;
		MPI_Isend(&m,sizeof(MSG),MPI_BYTE, recvid, TAG_SEND, comm, pending);
		pendingSendReq.push_back(boost::shared_ptr<MPI_Request>(pending));
	}
}

template<typename MSG>
bool MPILayer<MSG>::Deliver(MSG* m)
{
	Update();
	if(toDeliver.empty()) {
		return false;
	} else {
		DBG("message present, delivering");
		*m = *(toDeliver.begin());
		toDeliver.pop_front();
	}
	return true;
}

template<typename MSG>
void MPILayer<MSG>::Bcast(MSG m)
{
	DBG("entering bcast");

	// a non-0 rank will send its message to be broadcasted by rank 0
	if(rank != 0) {
		MPI_Request* pending = new MPI_Request;
		MPI_Isend(&m,sizeof(MSG),MPI_BYTE, 0, TAG_FWBCAST, comm, pending);
		pendingSendReq.push_back(boost::shared_ptr<MPI_Request>(pending));
		return;
	}

	// only rank 0 executes this
	int c1 = rank*2 + 1;
	int c2 = rank*2 + 2;
	if(c1 < size) {
		MPI_Request* pending = new MPI_Request;
		MPI_Isend(&m,sizeof(MSG),MPI_BYTE, c1, TAG_BCAST, comm, pending);
		pendingSendReq.push_back(boost::shared_ptr<MPI_Request>(pending));
		DBG("message sent to child " << c1);
	}
	if(c2 < size) {
		MPI_Request* pending = new MPI_Request;
		MPI_Isend(&m,sizeof(MSG),MPI_BYTE, c2, TAG_BCAST, comm, pending);
		pendingSendReq.push_back(boost::shared_ptr<MPI_Request>(pending));
		DBG("message sent to child " << c2);
	}
	toDeliver.push_back(m);
}

template<typename MSG>
void MPILayer<MSG>::Sync(MSG m)
{
	int parent = (rank-1)/2;
	// node process, search for a valid entry in pending barriers
	typename std::set<sync_msg>::iterator it = pendingBarriers.find(sync_msg(m));
	if(it != pendingBarriers.end()) {
		sync_msg sm = *it;
		pendingBarriers.erase(it);
		sm.me++;
		pendingBarriers.insert(sm);
	} else {
		sync_msg sm(m);
		sm.me++;
		if(2*(rank+1) >= size) sm.rc++;
		if(2*rank+1 >= size) sm.lc++;
		pendingBarriers.insert(sm);
	}
	// here we are sure a sync_msg is present in the set for this barrier
	it = pendingBarriers.find(sync_msg(m));
	sync_msg sm = *it;
	if(sm.ready()) {
		pendingBarriers.erase(it);
		sm.decrease();
		// if we have reached rank 0, broadcast the message
		if(rank == 0) {
			Bcast(sm.msg);
		} else {
			// otherwise, send to parent
			MPI_Request* pending = new MPI_Request;
			MPI_Isend(&m,sizeof(MSG),MPI_BYTE,parent, TAG_SYNC, comm, pending);
			pendingSendReq.push_back(boost::shared_ptr<MPI_Request>(pending));
		}
		// if the barrier is dead, delete it
		if(!sm.dead()) {
			pendingBarriers.insert(sm);
		}
	}
}

}
#endif
