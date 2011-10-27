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
 * \file Client.h
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 * Main header to include in a C client.
 */
#ifndef __DAMARIS_CLIENT_C_H
#define __DAMARIS_CLIENT_C_H

#include <stdlib.h>

typedef int64_t DC_chunk_handle_t;

/**
 * \brief Initializes the client-side C library.
 * \see Damaris::Client::Client
 * \param[in] configfile : name of the XML configuration file.
 * \param[in] core_id : identifier for the client (should be unique).
 * \return 0 in case of success, forces the program to stop in case of failure.
 */
int 	DC_initialize(const char* configfile, int32_t core_id);

/** 
 * \brief Writes a variable.
 * \see Damaris::Client::write
 * \param[in] varname : name of the variable to write.
 * \param[in] iteration : iteration at which the variable is written.
 * \param[in] data : pointer to the data to write.
 * \return 0 in case of success,
 *         -1 if the layout or the variable is not defined,
 *         -2 if the layout has a bad size (0 or < 0),
 *         -3 if it fails to allocate shared memory to write.
 */
int 	DC_write(const char* varname, int32_t iteration, const void* data);

/**
 * \brief Writes a chunk of a variable.
 * \see Damaris::Client::chunk_write
 */
int 	DC_chunk_write(DC_chunk_handle_t chunkh, const char* varname, 
		int32_t iteration, const void* data);

/**
 * \brief Defines a chunk.
 * \see Damaris::Client::chunk_set
 */
DC_chunk_handle_t DC_chunk_set(unsigned int dimensions, int* si, int* ei);

/**
 * \brief Free a chunk handle.
 * \see Damaris::Client::chunk_free
 */
void 	DC_chunk_free(DC_chunk_handle_t chunkh);

/**
 * \brief Allocates the data required for a variable to be entirely written in memory.
 * \return a pointer to the allocated region in case of success, NULL in case of failure.
 * \see Damaris::Client::alloc
 */
void* 	DC_alloc(const char* varname, int32_t iteration);

/**
 * \brief Commits an allocated variable.
 * \see Damaris::Client::commit
 */
int 	DC_commit(const char* varname, int32_t iteration);

/**
 * \brief Sends an event to the dedicated core.
 * \see Damaris::Client::signal
 * \param[in] signal_name : name of the event.
 * \param[in] iteration : iteration at which the event is sent.
 * \return 0 in case of success, -1 in case of failure.
 */
int 	DC_signal(const char* signal_name, int32_t iteration);

/**
 * \brief Retrieve the value associated to a parameter.
 * \see Damaris::Client::getParameter
 * \param[in] param_name : name of the parameter.
 * \param[out] buffer : pointer to the buffer to hold the value.
 * \return 0 in case of success, -1 if the parameter does not exist.
 */
int 	DC_get_parameter(const char* param_name, void* buffer);

/**
 * \brief Send a kill signal to the server.
 * \see Damaris::Client::killServer
 * \return 0 in case of success, -1 in case of failure.
 */
int	DC_kill_server();

/**
 * \brief Call client's destructor.
 * \see Damaris::Client::~Client
 * \return 0
 */
int 	DC_finalize();

#endif
