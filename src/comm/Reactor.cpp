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

#include <climits>

#include "util/Unused.hpp"
#include "util/Bind.hpp"
#include "comm/Reactor.hpp"
#include "comm/MPI2Reactor.hpp"
#include "comm/Channel.hpp"

namespace damaris
{
	
shared_ptr<Reactor> Reactor::New(MPI_Comm comm, int radix)
{
	shared_ptr<Reactor> r;
#if MPI_VERSION == 3
	// for now...
	r = MPI2Reactor::New(comm,radix);
#else
	r = MPI2Reactor::New(comm,radix);
#endif
	return r;
}

bool Reactor::Init(MPI_Comm comm)
{
	running_ = false;
	comm_ = comm;
	MPI_Comm_rank(comm_,&rank_);
	MPI_Comm_size(comm_,&size_);

	everybody_ = Channel::New(SHARED_FROM_THIS(),comm_, MPI_ANY_SOURCE, 0);

	// start listening for dispatch
	everybody_->AsyncRecv(DISPATCH,&dispatch_info_,sizeof(dispatch_info_),
				BIND(&Reactor::DispatchCallback,
					SHARED_FROM_THIS(),
					_1,_2,_3,_4));
	
	return true;
}

bool Reactor::RemoveRequest(unsigned int index)
{
	if(index >= requests_.size()) return false;
	
	int count = requests_.size();
	
	requests_[index]  = requests_[count-1];
	ext_callbacks_[index] = ext_callbacks_[count-1];
	buffers_[index]   = buffers_[count-1];
	
	requests_.resize(count-1);
	ext_callbacks_.resize(count-1);
	buffers_.resize(count-1);
	
	return true;
}

void Reactor::DispatchCallback(int UNUSED(tag), int UNUSED(source), 
		const void* buf, int count)
{
	MsgInfo* info = (MsgInfo*)buf;
	int root = info->root;
	int tg = info->tag;
	int cnt = info->count;
	std::vector<char> buffer(cnt);
	MPI_Status status;
	int err = MPI_Recv(&buffer[0],cnt,MPI_BYTE,root,tg,comm_,&status);
	shared_ptr<Callback> cb = int_callbacks_[tg];
	if(cb && (err == MPI_SUCCESS)) (*cb)(tg,root,&buffer[0],count);
	
	everybody_->AsyncRecv(DISPATCH,&dispatch_info_,sizeof(dispatch_info_),
				BIND(&Reactor::DispatchCallback,
					SHARED_FROM_THIS(),
					_1,_2,_3,_4));
	
}

bool Reactor::Dispatch(int tag, int dest, const void* buf, int count)
{
	if(dest == rank_) {
		shared_ptr<Callback> cb = int_callbacks_[tag];
		if(cb) (*cb)(tag,rank_,buf,count);
	}
	
	MsgInfo info;
	info.root 	= rank_;
	info.tag	= tag;
	info.count 	= count;
	
	int err = MPI_Send(&info,sizeof(MsgInfo),MPI_BYTE,dest,DISPATCH,comm_);
	if(err != MPI_SUCCESS) return false;
	err = MPI_Send((void*)buf,count,MPI_BYTE,dest,tag,comm_);
	if(err != MPI_SUCCESS) return false;
	return true;
}
	
int Reactor::Poll()
{
	int nb = 0;
	while(running_ && PollOne()) {
		nb++;
		if(nb == INT_MAX) return nb;
	}
	return nb;
}

int Reactor::PollOne()
{
	int count = requests_.size();

	if(count == 0) return 0;

	int index;
	MPI_Status status;
	int flag;
	MPI_Testany(count, &(requests_[0]), &index, &flag, &status);
	
	if(not flag) return 0;
	
	shared_ptr<Callback> cb = ext_callbacks_[index];
	char* buf = buffers_[index];
	
	RemoveRequest(index);
	
	int len;
	MPI_Get_count(&status, MPI_BYTE, &len);
	int source = status.MPI_SOURCE;
	int tag = status.MPI_TAG;
	
	if(cb) (*cb)(tag,source,buf,len);
	
	return 1;
}

void Reactor::Run()
{
	running_ = true;
	while(running_) {
		RunOne();
	}
}

void Reactor::RunOne()
{
	int count = requests_.size();
	if(count == 0) return;

	int index;
	MPI_Status status;

	int err = MPI_Waitany(count, &(requests_[0]), &index, &status);
	if(err != MPI_SUCCESS) return;
	
	shared_ptr<Callback> cb = ext_callbacks_[index];
	char* buf = buffers_[index];
	
	RemoveRequest(index);
	
	int len;
	MPI_Get_count(&status, MPI_BYTE, &len);
	int source = status.MPI_SOURCE;
	int tag = status.MPI_TAG;
	
	if(cb) (*cb)(tag,source,buf,len);
}

void Reactor::Stop()
{
	running_ = false;
}

}
