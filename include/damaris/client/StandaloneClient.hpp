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
#ifndef __DAMARIS_STANDALONE_CLIENT_H
#define __DAMARIS_STANDALONE_CLIENT_H

#include <string>
#include <vector>
#include <stdint.h>

#include "damaris/util/Deleter.hpp"
#include "damaris/util/Pointers.hpp"

namespace damaris {

USING_POINTERS;
	
/**
 * The StandaloneClient object represents a single core running the simulation.
 * It is instanciated when no dedicated core is required by the user, and
 * calls itself plugins instead of relying on a dedicated core. This class
 * inherites from Client.
 */
class StandaloneClient : public Client {

	friend class Deleter<StandaloneClient>;
	
	/** 
	 * Constructor.
	 */ 
	StandaloneClient(); 
		
	/**
	 * Destructor.
	 */
	virtual ~StandaloneClient() {}

	public:
	
	/**
	 * Creates a StandaloneClient object and returns it as a 
	 * shared_ptr<Client>.
	 */
	static shared_ptr<Client> New() {
		return shared_ptr<Client>(new StandaloneClient(), 
				Deleter<StandaloneClient>());
	}
		
	/**
	 * Overwrites the Connect function of the Client class to avoid sending
	 * a message to a non-existing dedicated core. A call to Connect is
	 * still required right after the creation of the StandaloneClient in
	 * order for some backends (such as VisIt) to work properly.
	 */
	virtual int Connect();

	/**
	 * Copy the data associated to the variable into Damaris' structures.
	 * Since there is no dedicated core, the data is not sent, only a copy
	 * is done.
	 *
	 * \param[in] varname : name of the variable to write.
	 * \param[in] block : id of the block to write.
	 * \param[in] data : pointer to the data to write.
	 * \param[in] blocking : wether to block or not if the required memory
	 * is not available. Forced to false in StandaloneClient.
	 */
	virtual int Write(const std::string &varname,
			int32_t block, const void* data,
			bool blocking = false);

	/**
	 * Triggers the action associated to the passed name.
	 * 
	 * \param[in] signame : signal name.
	 */
	virtual int Signal(const std::string & signame);

	/**
	 * Allocates memory to store the block of the specified variable.
	 * \see Client::Alloc
	 */
	virtual int Alloc(const std::string & varname,
		int32_t block, void** buffer, bool blocking = true);

	/** 
	 * Commit the specified block of a variable, i.e. set the block as
	 * read-only.
	 *
	 * \param[in] varname : variable name.
	 * \param[in] block : block id.
	 * \param[in] iteration : iteration number of the block to commit.
	 * Defaults to current iteration.
	 */
	virtual int Commit(const std::string & varname, 
			int32_t block, int32_t iteration = -1);

	/**
	 * Clears the specified block of a variable, i.e. set the block as
	 * read/write again. Does NOT free the memory. The memory should be
	 * freed either by a plugin function or by the garbage collector.
	 *
	 * \param[in] varname : name of the variable to clear.
	 * \param[in] block : id of the block.
	 * \param[in] iteration : iteration to clear. Defaults to current.
	 */
	virtual int Clear(const std::string & varname, 
			int32_t block, int32_t iteration = -1);

	/**
	 * Sends a signal to the server to shut it down. Since there is no
	 * server in Standalone mode, does nothing.
	 */
	virtual int StopServer();

	/**
	* Indicates that the iteration has terminated, this will potentially
	* update connected backends such as VisIt.
	*/
	virtual int EndIteration();

};

}

#endif
