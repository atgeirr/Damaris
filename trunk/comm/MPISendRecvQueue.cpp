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
	// TODO
}

void MPISendRecvQueue::Send(const void* buffer)
{
	MPI_Isend
}

bool MPISendRecvQueue::TrySend(const void* buffer)
{
	// TODO
}

void MPISendRecvQueue::Receive(void* buffer, size_t buffer_size)
{
	// TODO
}
  
bool MPISendRecvQueue::TryReceive(void *buffer, size_t buffer_size)
{
	update();
	if(toDeliver.empty()) return false;

	char* msg = toDeliver.pop_front();
	memcpy(buffer,msg,min(buffer_size,msgSize));
	delete msg;
	return true;	
}

size_t MPISendRecvQueue::NumMsg()
{
	// TODO
}

}
#endif
