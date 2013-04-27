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
 * \date November 2012 
 * \author Matthieu Dorier
 * \version 0.7
 * Main header to include in a C client.
 */
#ifndef __DAMARIS_CLIENT_C_H
#define __DAMARIS_CLIENT_C_H

#include <stdlib.h>
#include <mpi.h>

/**
 * \brief Initializes the client-side C library.
 * \see Damaris::Client::Client
 * \param[in] comm : simulation's communicator.
 * \param[in] configfile : name of the XML configuration file.
 * \return 0 in case of success, forces the program to stop in case of failure.
 */
int 	DC_initialize(const char* configfile, MPI_Comm comm);

/** 
 * \brief Writes a variable.
 * \see Damaris::Client::write
 * \param[in] varname : name of the variable to write.
 * \param[in] iteration : iteration at which the variable is written.
 * \param[in] data : pointer to the data to write.
 * \return the number of bytes witten in case of success,
 *         -1 if the variable is unknown,
 *         -2 if the memory allocation failed,
 */
int 	DC_write(const char* varname, const void* data);

/**
 * \see Damaris::Client::write_block
 */
int 	DC_write_block(const char* varname, int32_t block, const void* data);

/**
 * \brief Allocates the data required for a variable to be entirely written in memory.
 * \return a pointer to the allocated region in case of success, NULL in case of failure.
 * \see Damaris::Client::alloc
 */
void* 	DC_alloc(const char* varname);

/**
 * \see Damaris::Client::alloc_block
 */
void*	DC_alloc_block(const char* varname, int32_t block);

/**
 * \brief Commits an allocated variable.
 * \see Damaris::Client::commit
 */
int 	DC_commit(const char* varname);
int 	DC_commit_block(const char* varname, int32_t block);
int	DC_commit_iteration(const char* varname, int32_t iteration);
int	DC_commit_block_iteration(const char* varname, int32_t block, int32_t iteration);

/**
 * \brief Sends an event to the dedicated core.
 * \see Damaris::Client::signal
 * \param[in] signal_name : name of the event.
 * \return  0 in case of success, 
	   -1 if the action name is unknown.
 */
int 	DC_signal(const char* signal_name);

/**
 * \brief Retrieve the value associated to a parameter.
 * \see Damaris::Client::get_parameter
 * \param[in] param_name : name of the parameter.
 * \param[out] buffer : pointer to the buffer to hold the value.
 * \param[in] size : maximum size of the buffer.
 * \return 0 in case of success, -1 if the parameter does not exist.
 */
int 	DC_parameter_get(const char* param_name, void* buffer, unsigned int size);

/**
 * \brief Change the value of a parameter.
 * \see Damaris::Client::set_parameter
 * \param[in] param_name :  name of the parameter.
 * \param[in] buffer : address of the value to copy.
 * \param[in] size : maximum size of the buffer.
 * \return 0 in case of success, -1 if the parameter was not found.
 */
int 	DC_parameter_set(const char* param_name, const void* buffer, unsigned int size);

/**
 * Set the position of blocks in a virtual space.
 * \see Damaris::Client::set_position and Damaris::Client::set_block_position.
 */
int 	DC_set_position(const char* var_name, const int* position);
int	DC_set_block_position(const char* var_name, const int* position, int32_t block);

/**
 * \brief Get the communicator gathering all clients.
 * The program must have been started through the start_mpi_entity function.
 * \return the communicator.
 */
MPI_Comm DC_mpi_get_client_comm();

/**
 * \brief Send a kill signal to the server.
 * \see Damaris::Client::killServer
 * \return 0 in case of success, -1 in case of failure.
 */
int	DC_kill_server();

/**
 * Notifies the server that the iteration has ended.
 * This will update potential external backends such as VisIt.
 */
int 	DC_end_iteration();

/**
 * \brief Call client's destructor.
 * \see Damaris::Client::~Client
 * \return 0
 */
int 	DC_finalize();

#endif
