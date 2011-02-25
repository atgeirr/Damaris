#ifndef __DAMARIS_MESSAGE_H
#define __DAMARIS_MESSAGE_H

#include <boost/interprocess/managed_shared_memory.hpp>

using namespace boost::interprocess;

namespace Damaris {
	
	enum msg_type_e {
		MSG_VAR,
		MSG_POKE
	};

	struct Message {
		msg_type_e type; // the type of message (MSG_VAR, MSG_POKE)
		int32_t sourceID;     // the id of the source sending the message
		int64_t iteration;       // the iteration number in the simulation
		char content[64];   // the content (name of the variable or the poke)
		int64_t layoutInfo[8]; // information related to the data layout
		managed_shared_memory::handle_t handle; // pointer on data
	};
	
}

#endif
