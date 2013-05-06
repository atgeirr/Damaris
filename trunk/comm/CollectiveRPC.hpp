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
 * \file Collective.hpp
 * \date February 2013
 * \author Matthieu Dorier
 * \version 0.8
 */

#ifndef __DAMARIS_COLLECTIVE_RPC_H
#define __DAMARIS_COLLECTIVE_RPC_H

#include <map>
#include "comm/Communication.hpp"
#include "core/Debug.hpp"

namespace Damaris {

/**
 * This class manages different classes of Collective RPC:
 * Multi-RPC are RPC called by one processes which are executed
 * by all the processes. Collective-RPC are RPC that must be
 * called by all the processes before being executed by all the
 * processes.
 * The template parameter provides the function type.
 * This should be a function taking no argument, or an object
 * with an implementation of operator()(void).
 *
 * Note: The implementation ensures that two calls of a Multi-RPC
 * from a same process will be executed in the same order,
 * but no order can be assumed for Collective RPC or between
 * a Collective RPC and a Multi RPC.
 */
template<typename F>
class CollectiveRPC {

	public:
		/**
		 * Types of RPC (Multi of Collective).
		 */
		enum rpc_type { RPC_MULTI, RPC_COLLECTIVE };
	
		/**
		 * Id of an RPC function, first field is the
		 * channel number, second field is the RPC id.
		 */
		typedef std::pair<int,int> rpc_id;

	private:
		std::map<rpc_id, std::pair<rpc_type,F> > rpcs; 
		/*!< Map associating RPC ids to a pair (type of RPC, function pointer). */

		Communication<rpc_id>* commLayer; /*!< Communication layer. */

		/**
		 * Constructor, private. Use CollectiveRPC::New to create a CollectiveRPC object.
		 */
		CollectiveRPC() : commLayer(NULL) {}

		/**
		 * Private destructor, use CollectiveRPC::Delete from outside the class.
		 */
		virtual ~CollectiveRPC() {};
	public:
		
		/**
		 * Creates a new CollectiveRPC object based on the
		 * provided communication layer.
		 */
		static CollectiveRPC* New(Communication<rpc_id>* comm);

		/**
		 * Deletes the CollectiveRPC object.
		 */
		static void Delete(CollectiveRPC* c);

		/**
		 * Updates the communication layer,
		 * may call callback RPCs.
		 */
		virtual void Update();

		/**
		 * Register a function as a multi-RPC and associate it with a
		 * particular id. Will overwrite any function previously
		 * registered with this same id.
		 */
		virtual void RegisterMulti(F rpc, int channel, int id);

		/**
		 * Register a function a collective-RPC and associate it with a
		 * particular id. Will overwrite any function previously
		 * registerd with this same id.
		 */
		virtual void RegisterCollective(F rpc, int channel, int id);

		/**
		 * Call a given RPC specified by its id.
		 */
		virtual void Call(int channel, int id);

};


template<typename F>
CollectiveRPC<F>* CollectiveRPC<F>::New(Communication<CollectiveRPC::rpc_id>* comm)
{
	CollectiveRPC<F>* c = new CollectiveRPC<F>();
	c->commLayer = comm;
	return c;
}

template<typename F>
void CollectiveRPC<F>::Delete(CollectiveRPC<F>* c)
{
	if(c != NULL) delete c;
}

/**
 * Updates the communication layer.
 */
template<typename F>
void CollectiveRPC<F>::Update()
{
	commLayer->Update(10);
	rpc_id msg;
	if(commLayer->Deliver(&msg)) {
		typename std::map<rpc_id,std::pair<rpc_type,F> >::iterator it = rpcs.find(msg);
		if(it != rpcs.end()) {
			F f = it->second.second;
			f();
		}
	}
}

template<typename F>
void CollectiveRPC<F>::Call(int channel, int id)
{
	typename std::map<rpc_id,std::pair<rpc_type,F> >::iterator it 
		= rpcs.find(rpc_id(channel,id));
	if(it != rpcs.end()) {
		switch(it->second.first) {
		case RPC_MULTI:
			commLayer->Bcast(rpc_id(channel,id));
			break;
		case RPC_COLLECTIVE:
			commLayer->Sync(rpc_id(channel,id));
			break;
		}
	}
}

template<typename F>
void CollectiveRPC<F>::RegisterMulti(F rpc, int channel, int id)
{
	rpcs[rpc_id(channel,id)] = std::pair<rpc_type,F>(RPC_MULTI,rpc);
}

template<typename F>
void CollectiveRPC<F>::RegisterCollective(F rpc, int channel, int id)
{
	rpcs[rpc_id(channel,id)] = std::pair<rpc_type,F>(RPC_COLLECTIVE,rpc);
}

}

#endif
