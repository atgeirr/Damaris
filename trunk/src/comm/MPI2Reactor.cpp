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

#include "util/Unused.hpp"
#include "util/Bind.hpp"
#include "util/Debug.hpp"
#include "comm/MPI2Reactor.hpp"
#include "comm/Channel.hpp"

namespace damaris
{

bool MPI2Reactor::Init(MPI_Comm comm, int radix)
{
	Reactor::Init(comm);
	if(radix <= 0) return false;
	radix_ = radix;
	for(int i = 1; i <= radix; i++) {
		int c = radix*rank_ + i;
		if(c < size_) {
			shared_ptr<Channel> ch 
				= Channel::New(
				SHARED_FROM_BASE_OF_THIS(Reactor,MPI2Reactor),
				comm_, c, 32);
			// starts listening for sync messages from children
			children_[c] = ch;
			sync_info_[c] = MsgInfo();
			ch->AsyncRecv(SYNC,&sync_info_[c],sizeof(MsgInfo),
				BIND(&MPI2Reactor::SyncCallback,
				SHARED_FROM_BASE_OF_THIS(Reactor,MPI2Reactor),
					_1,_2,_3,_4));
		}
	}
	
	leader_ = Channel::New( SHARED_FROM_BASE_OF_THIS(Reactor,MPI2Reactor),
			comm_, 0, 32);
	
	if(rank_ != 0) {
		parent_ = Channel::New(SHARED_FROM_THIS(),
				comm_,(rank_-1)/radix, 32);
		// start listening for broadcast messages
		parent_->AsyncRecv(BCAST,&bcast_info_,sizeof(bcast_info_),
				BIND(&MPI2Reactor::BcastCallback,
				SHARED_FROM_BASE_OF_THIS(Reactor,MPI2Reactor),
					_1,_2,_3,_4));
	} else {
		// for rank 0, parent_ == himself
		parent_ = Channel::New(SHARED_FROM_THIS(), comm_, 0, 32);
		// start listening for broadcast messages from everybody
		everybody_->AsyncRecv(BCAST,&bcast_info_,sizeof(bcast_info_),
				BIND(&MPI2Reactor::BcastCallback,
				SHARED_FROM_BASE_OF_THIS(Reactor,MPI2Reactor),
					_1,_2,_3,_4));
	}
	
	sync_info_[parent_->GetEndPoint()] = MsgInfo();
	parent_->AsyncRecv(SYNC_WAVE,&sync_info_[parent_->GetEndPoint()],
				sizeof(MsgInfo),
				BIND(&MPI2Reactor::SyncCallback,
				SHARED_FROM_BASE_OF_THIS(Reactor,MPI2Reactor),
					_1,_2,_3,_4));
	
	return true;
}

bool MPI2Reactor::Broadcast(int tag, const void* buf, int count)
{
	if(bcast_buffers_.count(tag) != 0) return false;
	std::vector<char> b(count);
	memcpy(&b[0],buf,count);
	MsgInfo info;
	info.root 	= rank_;
	info.tag 	= tag;
	info.count 	= count;
	bcast_buffers_.insert(std::pair<int,std::vector<char> >(tag,b));
	if(leader_->Send(BCAST,&info,sizeof(info)) == DAMARIS_OK) {
		return true;
	} else {
		bcast_buffers_.erase(tag);
		return false;
	}
}

void MPI2Reactor::BcastCallback(int UNUSED(tag), int UNUSED(source), 
	const void* buf, int count)
{
	MsgInfo* info 	= (MsgInfo*)buf;
	int root 	= info->root;
	int tg 		= info->tag;
	int cnt 	= info->count;
	
	std::map<int,shared_ptr<Channel> >::iterator it, end;
	it = children_.begin();
	end = children_.end();
	for(; it != end; it++) {
		it->second->Send(BCAST,info,sizeof(*info));
	}
	
	std::vector<char> buffer;
	if(rank_ != root) {
		buffer.resize(cnt);
	} else {
		buffer = bcast_buffers_[tg];
		bcast_buffers_.erase(tg);
	}

	void* b = &buffer[0]; 
	int err = MPI_Bcast(b,cnt,MPI_BYTE,root,comm_);
	
	shared_ptr<Callback> cb = int_callbacks_[tg];
	if(cb && (err == MPI_SUCCESS)) (*cb)(tg,root,b,count);
	
	if(rank_ != 0) {
		parent_->AsyncRecv(BCAST,&bcast_info_,sizeof(bcast_info_),
			BIND(&MPI2Reactor::BcastCallback,
				SHARED_FROM_BASE_OF_THIS(Reactor,MPI2Reactor),
				_1,_2,_3,_4));
	} else {
		everybody_->AsyncRecv(BCAST,&bcast_info_,sizeof(bcast_info_),
			BIND(&MPI2Reactor::BcastCallback,
				SHARED_FROM_BASE_OF_THIS(Reactor,MPI2Reactor),
				_1,_2,_3,_4));
	}
}

bool MPI2Reactor::Sync(int tag)
{
	if(sync_tags_.count(tag) != 0) {
		if(pending_sync_tags_.count(tag) == 0) {
			pending_sync_tags_[tag] = 1;
		} else {
			pending_sync_tags_[tag] += 1;
		}
		return true;
	}
		
	MsgInfo info;
	info.root 	= rank_;
	info.tag 	= tag;
	info.count 	= 0;
	// associate the buffer and count to a tag
	sync_tags_.insert(tag);
	if(sync_counters_.count(tag) == 0) {
		sync_counters_[tag] = 1;
	} else {
		sync_counters_[tag] += 1;
	}
	
	unsigned int critical_mass = children_.size() + 1;
	if(sync_counters_[tag] == (int)critical_mass) {
		//critical mass achieved, send to the parent.
		//if we are root, broadcast to children
		if(rank_ == 0) {
			// when rank = 0, parent = this process, actually
			parent_->Send(SYNC_WAVE,&info,sizeof(info));
		} else {
			parent_->Send(SYNC,&info,sizeof(info));
		}
	}
	return true;
}

void MPI2Reactor::SyncCallback(int tag, int source, const void* buf, int UNUSED(count))
{
	MsgInfo* info = (MsgInfo*)buf;
	// if sent by the parent
	// we can call the callback after forwarding to children
	if(tag == SYNC_WAVE) {
		// forward to children
		std::map<int, shared_ptr<Channel> >::iterator it, end;
		it = children_.begin();
		end = children_.end();
		for(;it != end; it++) {
			it->second->Send(SYNC_WAVE,info,sizeof(MsgInfo));
		}
		// call the callback
		if(int_callbacks_.count(info->tag)) {
			shared_ptr<Callback> cb = int_callbacks_[info->tag];
			if(cb) (*cb)(info->tag,rank_,NULL,0);
		} 
	  

		// reset the counter
    sync_counters_.erase(info->tag);

		sync_tags_.erase(info->tag);
		// if there is a pending sync for the same tag, 
		// initiate it
		if(pending_sync_tags_.count(tag) != 0) {
						pending_sync_tags_[tag] -= 1;
						if(pending_sync_tags_.count(tag) == 0) {
										pending_sync_tags_.erase(tag);
						}
						Sync(tag);
		}

	} else {
		// receiving from a child
		if(sync_counters_.count(info->tag) == 0) {
			sync_counters_[info->tag] = 1;
		} else {
			sync_counters_[info->tag] += 1;
		}
		// count the number of request required to broadcast to parent
		int nb = 1 + children_.size();
		if(sync_counters_[info->tag] == nb) {
			// critical mass achieved, send to the parent.
			// if we are root, send the sync wave to self
			if(rank_ == 0) {
				parent_->Send(SYNC_WAVE,info,sizeof(MsgInfo));
			} else {
				parent_->Send(SYNC,info,sizeof(MsgInfo));
			}
		}
	}
	
	// reload a callback for sync on this source
	if(source != parent_->GetEndPoint()) {
		shared_ptr<Channel> ch = children_[source];
			ch->AsyncRecv(SYNC,&sync_info_[source],
				sizeof(MsgInfo),
				BIND(&MPI2Reactor::SyncCallback,
				SHARED_FROM_BASE_OF_THIS(Reactor,MPI2Reactor),
					_1,_2,_3,_4));
	} else {
		parent_->AsyncRecv(SYNC_WAVE,&sync_info_[source],
				sizeof(MsgInfo),
				BIND(&MPI2Reactor::SyncCallback,
				SHARED_FROM_BASE_OF_THIS(Reactor,MPI2Reactor),
					_1,_2,_3,_4));
	}
}

}
