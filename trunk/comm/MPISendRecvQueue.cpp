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
 * \file MPISendRecvQueue.cpp
 * \date November 2O12
 * \author Matthieu Dorier
 * \version 0.7
 */
#include "comm/MPISendRecvQueue.hpp"

namespace Damaris {
	
void MPISendRecvQueue::update()
{
	static char* msg = NULL;
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
		msg = new char[msgSize];
		MPI_Irecv(msg, msgSize, MPI_BYTE, 
				MPI_ANY_SOURCE, MPI_ANY_TAG, comm, &request);
		listening = true;
	}

	// we check the status of the last MPI_Irecv request
	int done;
	MPI_Test(&request,&done,&status);
	if(done) {
		toDeliver.push_back(msg);
		listening = false;
	}
}

void MPISendRecvQueue::Send(void* buffer)
{
	MPI_Request* pending = new MPI_Request;
	MPI_Isend(buffer,msgSize,MPI_BYTE,receiver,0,comm,pending);
	pendingSendReq.push_back(boost::shared_ptr<MPI_Request>(pending));
}

bool MPISendRecvQueue::TrySend(void* buffer)
{
	Send(buffer);
	return true;
}

void MPISendRecvQueue::Receive(void* buffer, size_t buffer_size)
{
	while(toDeliver.empty()) {
		update();
	}
	char* msg = *(toDeliver.begin());
	memcpy(buffer,msg,std::min(buffer_size,msgSize));
	delete msg;
	toDeliver.pop_front();
}
  
bool MPISendRecvQueue::TryReceive(void *buffer, size_t buffer_size)
{
	update();
	if(toDeliver.empty()) return false;
	char* msg = *(toDeliver.begin());
	memcpy(buffer,msg,std::min(buffer_size,msgSize));
	delete msg;
	toDeliver.pop_front();
	return true;	
}

size_t MPISendRecvQueue::NumMsg()
{
	if(pendingSendReq.empty() && toDeliver.empty()) return 0;
	else return 1;
}

}
