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
 * \date April 2012
 * \author Matthieu Dorier
 * \version 0.5
 */

#ifndef __DAMARIS_MPI_LAYER_H
#define __DAMARIS_MPI_LAYER_H

#include <mpi.h>
#include <list>
#include <boost/shared_ptr.hpp>
#include "core/Communication.hpp"

namespace Damaris {

/**
 * This class represents an asynchronous communication layer using MPI.
 */
template<typename MSG>
class MPILayer : public Communication<MSG> {

	private:
		static const int TAG_SEND  = 0; /*!< MPI tag indicating that the message is sent. */
		static const int TAG_BCAST = 1;	/*!< MPI tag indicating that the message is broadcasted. */

		MPI_Comm comm; /*!< Communicator through which sending messages. */
		std::list<MSG> toDeliver; /*!< List of messages ready to be delivered. */
		std::list<boost::shared_ptr<MPI_Request> > pendingSendReq; /*!< List of requests associated to message sent. */
		int rank; /*!< Rank of the process in the communicator. */
		int size; /*!< Size of the communicator. */

		/**
		 * Constructor. The constructor is private, use New to create an instance.
		 */
		MPILayer(const MPI_Comm &c);
		
		/**
		 * Destructor (private)
		 */
		~MPILayer();

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
		 * Update (try receiving messages from other processes and put them in
		 * the toDeliver queue).
		 */
		void update();

		/**
		 * Sends a message (non-blocking). The message will be eventually
		 * delevered by the process identifyed by its ID.
		 */
		void send(int receiver, MSG *m);

		/**
		 * Delivers the next message in the queue. If there is no message,
		 * returns false.
		 */
		bool deliver(MSG* m);

		/**
		 * Broadcast a message to all processes in the communication layer.
		 * The message will be eventually delivered by all processes.
		 */
		void bcast(MSG* m);
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
void MPILayer<MSG>::update()
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
		// if the tag is a TAG_BCAST, forward to child processes
		if(status.MPI_TAG == TAG_BCAST) {
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
		}
		// in any case, deliver it
		toDeliver.push_back(m);
		listening = false;
	}
}

template<typename MSG>
void MPILayer<MSG>::send(int recvid, MSG *m)
{
	MPI_Request* pending = new MPI_Request;
	MPI_Isend(m,sizeof(MSG),MPI_BYTE, recvid, TAG_SEND, comm, pending);
	pendingSendReq.push_back(boost::shared_ptr<MPI_Request>(pending));
}

template<typename MSG>
bool MPILayer<MSG>::deliver(MSG* m)
{
	update();
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
void MPILayer<MSG>::bcast(MSG* m)
{
	DBG("entering bcast");
	int c1 = rank*2 + 1;
	int c2 = rank*2 + 2;
	if(c1 < size) {
		MPI_Request* pending = new MPI_Request;
		MPI_Isend(m,sizeof(MSG),MPI_BYTE, c1, TAG_BCAST, comm, pending);
		pendingSendReq.push_back(boost::shared_ptr<MPI_Request>(pending));
		DBG("message sent to child " << c1);
	}
	if(c2 < size) {
		MPI_Request* pending = new MPI_Request;
		MPI_Isend(m,sizeof(MSG),MPI_BYTE, c2, TAG_BCAST, comm, pending);
		pendingSendReq.push_back(boost::shared_ptr<MPI_Request>(pending));
		DBG("message sent to child " << c2);
	}
	toDeliver.push_back(*m);
}

}
#endif
