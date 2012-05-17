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
 * \file Writer.hpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_WRITER_H
#define __DAMARIS_WRITER_H

#include <string>
#include "core/ChunkDescriptor.hpp"
#include "data/Layout.hpp"

namespace Damaris {

typedef ChunkDescriptor* chunk_h;

/**
 * The Writer class is an abstraction for a class that
 * can write data (typically a Client).
 */
class Writer  {
	public:
		/**
		 * \brief Writes a full variable.
		 * Writes a variable into shared memory and sends a message 
		 * notifying the write to the dedicated core running on the same node.
		 * If the memory segment is full, this function will fail.
		 * If the message queue is full, this function will block.
		 * 
		 * \param[in] varname : name of the variable to write. This variable should 
		 *                      be defined in the XML configuration.
		 * \param[in] iteration : iteration number for this write.
		 * \param[in] data : pointer to the data to be copied to the shared memory buffer.
		 *
		 * \param[in] blocking : if true, waits for enough memory to be available.
		 *  
		 * \return the amount of bytes written in case of success,
		 *         -1 if the variable has not been defined,
		 *         -2 if the allocation of memory failed.
		 */
		virtual int write(const std::string & varname, 
			int32_t iteration, const void* data, bool blocking = true) = 0;
		
		/**
		 * \brief Writes a chunk of a variable.
		 * Checks that the chunk is within the bound of the variable's layout first.
		 * 
		 * \param[in] varname : name of the variable to write in. This variable should
		 *                      be defined in the XML configuration.
		 * \param[in] iteration : iteration number for this write.
		 * \param[in] chunkh : handle of a pre-defined chunk.
		 * \param[in] data : pointer to the data to be written in the shared memory buffer.
		 * \param[in] blocking : indicates wether the function should block when there is not
		 *                       enough memory, or if it should return an error code.
		 * \return the amount of bytes written in case of success,
		 *         -1 if the variable has not been defined,
		 *         -2 if the allocation of memory failed,
		 *         -3 if the chunk has an inapropriate shape.
		 */
		virtual int chunk_write(chunk_h chunkh, const std::string & varname, 
			int32_t iteration, const void* data, bool blocking = true) = 0;

		/**
		 * \brief Sends an event.
		 * Sends en event to the dedicated core running on the dedicated core.
		 * If the message queue is full, this function will block 
		 * until it can send the event.
		 * If the event is not explicitely defined in the configuration file,
		 * it will be ignored.
		 * 
		 * \param[in] signam : name of the event to send.
		 * \param[in] iteration : iteration associated to the event.
		 * 
		 * \return 0 in case of success,
		 *         -1 in case of failure when sending the message,
		 *	   -2 in case the event is not defined.
		 */
		virtual int signal(const std::string & signame, int32_t iteration) = 0;

		/**
		 * \brief Allocate a buffer directly in shared memory for future writing.
		 * Allocates a buffer in shared memory so it can be written after by the simulation.
		 * Requires a call to commit to notify the dedicated core that the variable 
		 * has been written. 
		 *
		 * \param[in] varname : name of the variable to write 
		 *		        (must be defined in the configuration file).
		 * \param[in] iteration : iteration at which the variable is written.
		 * \param[in] blocking : blocks until enough memory is available, if true.
		 *
		 * \return a pointer to the allocated memory in case of success,
		 *         NULL in case of failure (variable not defined, allocation error).
		 */
		virtual void* alloc(const std::string & varname, int32_t iteration, bool blocking = true) = 0;

		/** 
		 * \brief Commit a variable.
		 * Notifies the dedicated core that the previously 
		 * allocated buffer has been written 
		 * Blocks if the message queue is full.
		 *
		 * \param[in] varname : name of the variable to notify 
		 *			(must have been previously allocated).
		 * \param[in] iteration : iteration of the associated variable.
		 * 
		 * \return 0 in case of success,
                 *        -1 if the variable hasn't been allocated.
		 */
		virtual int commit(const std::string & varname, int32_t iteration) = 0;

		/**
		 * \brief Defines a chunk.
		 * The representation of the chunk is as in Fortran, given boundaries.
		 * \param[in] type : type of the chunk.
		 * \param[in] dimensions : number of dimensions.
		 * \param[in] startIndices : start indices.
		 * \param[in] endIndices : end indices.
		 * \return A chunk handle. Call chunk_free to free the chunk handle.
		 */
		virtual chunk_h chunk_set(unsigned int dimensions,
					const std::vector<int> & startIndices, 
					const std::vector<int> & endIndices) = 0;

		/**
		 * \brief Free a chunk handle.
		 */
		virtual void chunk_free(chunk_h chunkh) = 0;

}; // class Writer

} // namespace Damaris

#endif
