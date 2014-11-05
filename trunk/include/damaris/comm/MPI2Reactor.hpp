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

#ifndef __DAMARIS_MPI2_REACTOR_H
#define __DAMARIS_MPI2_REACTOR_H

#include <map>
#include <set>
#include <vector>
#include <mpi.h>

#include "damaris/util/Pointers.hpp"
#include "damaris/util/Deleter.hpp"
#include "damaris/util/ForwardDcl.hpp"

#include "damaris/comm/Reactor.hpp"

namespace damaris
{

USING_POINTERS;
	
class MPI2Reactor : public Reactor {
	
	friend class Deleter<MPI2Reactor>;
	friend class Channel;
	
private:
	// -------------------------------------------------------------- //
	// attributes necessary for the broadcast/sync tree
	// -------------------------------------------------------------- //
	int radix_;	// number of children in the communication tree
	std::map<int,shared_ptr<Channel> > children_;
	shared_ptr<Channel> leader_; // leader of the communicator
	shared_ptr<Channel> parent_; // parent in the broadcast tree
	std::map<int,std::vector<char> > bcast_buffers_; 
					// keeps buffers used in broadcasts
	std::set<int> sync_tags_; // tags currently waiting for a sync
	std::map<int,int> pending_sync_tags_; // tag queued for later sync
	MsgInfo bcast_info_; // buffer used to receive informations on bcast
	std::map<int,MsgInfo> sync_info_; // buffers for sync messages
	std::map<int,int> sync_counters_; // counts the number of sync received
	
	/**
	 * Constructor. Not supposed to be called anywhere else than from
	 * the static New function.
	 */
	MPI2Reactor() {}
	
	/**
	 * Called by New to initialize the brodcast tree.
	 * \see MPI2Reactor::New
	 */
	virtual bool Init(MPI_Comm comm, int radix);
	
	/**
	 * Destructor.
	 */
	virtual ~MPI2Reactor() {}
	
	/**
	 * This function is used as a callback associated to the tag BCAST.
	 */
	void BcastCallback(int tag, int source, const void* buf, int count);
		
	/**
	 * This function is used as a callback associated to the tag SYNC.
	 */
	void SyncCallback(int tag, int source, const void* buf, int count);
	
public:
	
	/**
	 * \see Reactor::Broadcast
	 */
	virtual bool Broadcast(int tag, const void* buf, int count);

	/**
	 * \see Reactor::Sync
	 */
	virtual bool Sync(int tag);
	
	/**
	 * Creates a new Reactor from an MPI_Comm; this call is collective
	 * accross the processes of the communicator, it will duplicate the
	 * communicator.
	 */
	static shared_ptr<Reactor> New(MPI_Comm comm, int radix=2) {
		MPI_Comm c;
		MPI_Comm_dup(comm,&c);
		shared_ptr<Reactor> reactor;
		if(radix > 0) {
			MPI2Reactor* r = new MPI2Reactor();
			reactor = shared_ptr<Reactor>(r, 
						Deleter<MPI2Reactor>());
			if(not r->Init(c,radix)) {
				reactor.reset();
			}
		}
		return reactor;
	}
};

}

#endif
