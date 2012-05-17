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
#include "core/Communication.hpp"

namespace Damaris {

/**
 * This class represents an asynchronous communication layer using MPI.
 */
template<typename MSG>
class MPILayer : public Communication<MSG> {

	private:
		static const int TAG_SEND  = 0;
		static const int TAG_BCAST = 1;

		MPI_Comm comm;
		std::list<MSG> toDeliver;
		int rank;
		int size;

		MPILayer(const MPI_Comm &c);
		~MPILayer();

	public:
		static MPILayer* New(const MPI_Comm &c);
		static void Delete(MPILayer* l);

		void update();
		/**
		 * Sends a message (non-blocking). The message will be eventually
		 * delevered by the process identifyed by its ID.
		 */
		void send(int receiver, const MSG *m);

		/**
		 * Delivers the next message in the queue. If there is no message,
		 * returns false.
		 */
		bool deliver(MSG* m);

		/**
		 * Broadcast a message to all processes in the communication layer.
		 * The message will be eventually delivered by all processes.
		 */
		void bcast(const MSG* m);
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

	if(!listening) {
		MPI_Irecv(&m, sizeof(MSG), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, comm, &request);
		listening = true;
	} else {
		int flag;
		MPI_Test(&request,flag,&status);
		if(flag) {
			toDeliver.push_back(m);
			listening = false;
		}
	}
}

template<typename MSG>
void MPILayer<MSG>::send(int recvid, const MSG *m)
{
	static MPI_Request request;
	MPI_Isend(m,sizeof(MSG),MPI_BYTE, recvid, TAG_SEND, comm, &request);
}

template<typename MSG>
bool MPILayer<MSG>::deliver(MSG* m)
{
	if(toDeliver.empty()) {
		return false;
	} else {
		*m = *(toDeliver.begin());
	}	
	return true;
}

template<typename MSG>
void MPILayer<MSG>::bcast(const MSG* m)
{
	static MPI_Request request;
	MPI_Isend(m,sizeof(MSG),MPI_BYTE, 0, TAG_BCAST, comm, &request);
}

}
#endif
