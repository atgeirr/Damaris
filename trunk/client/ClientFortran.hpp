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
 * \file ClientFortran.hpp
 * \date November 2012
 * \author Matthieu Dorier
 * \version 0.7
 * \see Client.hpp
 * Definition of the Fortran client functions.
 */
#ifdef __ENABLE_FORTRAN
#include "core/FCMangle.h"
#include "client/Client.hpp"

#ifndef FC_FUNC_GLOBAL
#define FC_FUNC_GLOBAL(name,NAME) name##_
#endif

#ifndef FC_FUNC_GLOBAL_
#define FC_FUNC_GLOBAL_(name,NAME) name##_
#endif

extern "C" {
/**
 * \fn df_initialize(fcomm, config_file_name, ierr)
 * \brief Initializes the client-side Fortran library.
 * \see Damaris::Client::Client
 * \param[in] fcomm : MPI communicator of the simulation.
 * \param[in] config_file_name : name of the XML configuration file (character*).
 * \param[out] ierr : returned error
 *	(0 in case of success, forces the program to stop in case of failure).
 */
void FC_FUNC_GLOBAL_(df_initialize,DF_INITIALIZE)
	(char* config_file_name_f, MPI_Fint* fcomm, int32_t* ierr_f, int config_file_name_size);

/** 
 * \fn df_write(var_name, data, ierr)
 * \brief Writes a variable.
 * \see Damaris::Client::write
 */	
void FC_FUNC_GLOBAL_(df_write,DF_WRITE)
	(char* var_name_f, void* data_f, int32_t* ierr_f, int var_name_size);

/**
 * \fn df_write_block(var_name, block_id, data, ierr)
 * \brief Writes a block of variable.
 * \see Damaris::Client::write_block
 */
void FC_FUNC_GLOBAL_(df_write_block,DF_WRITE)
        (char* var_name_f, int32_t* block_f, void* data_f, int32_t* ierr_f, int var_name_size);

/** 
 * \fn df_alloc(var_name, ierr)
 * \brief Allocate space for a variable.
 * \see Damaris::Client::alloc
 */
void* FC_FUNC_GLOBAL(df_alloc,DF_ALLOC)
	(char* var_name_f, int32_t* ierr_f, int var_name_size);

/**
 * \fn df_alloc_block(varname, block_id, ierr)
 * \brief Allocate space for a block of variable.
 * \see Damaris::Client::alloc_block
 */
void* FC_FUNC_GLOBAL(df_alloc_block,DF_ALLOC_BLOCK)
	(char* var_name_f, int32_t* block_f, int32_t* ierr_f, int var_name_size);

/**
 * \fn df_commit
 * \brief Commit a previously allocated variable.
 * \see Damaris::Client::commit
 */
void FC_FUNC_GLOBAL(df_commit,DF_COMMIT)
	(char* var_name_f, int32_t* ierr_f, int var_name_size);

void FC_FUNC_GLOBAL(df_commit_block,DF_COMMIT_BLOCK)
        (char* var_name_f, int32_t* block_f, int32_t* ierr_f, int var_name_size);

void FC_FUNC_GLOBAL(df_commit_iteration,DF_COMMIT_ITERATION)
        (char* var_name_f, int32_t* iteration_f, int32_t* ierr_f, int var_name_size);

void FC_FUNC_GLOBAL(df_commit_block_iteration,DF_COMMIT_BLOCK_ITERATION)
        (char* var_name_f, int32_t* block_f, int32_t* iteration_f, int32_t* ierr_f, int var_name_size);

/**
 * \fn df_signal(event_name, ierr)
 * \brief Sends an event to the dedicated core.
 * \see Damaris::Client::signal
 */	
void FC_FUNC_GLOBAL(df_signal,DF_SIGNAL)
	(char* event_name_f, int* ierr_f, int event_name_size);

/**
 * \fn df_parameter_get(param_name, buffer, size, ierr)
 * \brief Retrieve the value associated to a parameter.
 * \see Damaris::Client::get_parameter
 */
void FC_FUNC_GLOBAL(df_parameter_get,DF_PARAMETER_GET)
	(char* param_name_f, void* buffer_f, int* size, int* ierr_f, int param_name_size);

/**
 * \fn df_parameter_set(param_name, buffer, size, ierr)
 * \brief Modify the value associated to a parameter.
 * \see Damaris::Client::set_parameter
 */
void FC_FUNC_GLOBAL(df_parameter_set,DF_PARAMETER_SET)
	(char* param_name_f, void* buffer_f, int* size, int* ierr_f, int param_name_size);

/**
 * \fn df_mpi_get_client_comm
 * \brief Retrieves the MPI communicator gathering clients.
 * \see Damaris::Client::get_client_communicator
 */
void FC_FUNC_GLOBAL(df_mpi_get_client_comm,DF_GET_MPI_CLIENT_COMM)
	(MPI_Fint* fcomm);
/**
 * \fn df_kill_server
 * \brief Sends a "kill" signal to the server.
 * \see Damaris::Client::kill_server
 */
void FC_FUNC_GLOBAL(df_kill_server,DF_KILL_SERVER)
	(int* ierr);

/**
 * \fn df_end_iteration(ierr)
 * \brief Notifies the server that the iteration has ended.
 */
void FC_FUNC_GLOBAL(df_end_iteration,DF_END_ITERATION)
    (int* ierr);

/**
 * \fn df_finalize(ierr)
 * \brief Call client's destructor.
 * \see Damaris::Client::~Client
 */
void FC_FUNC_GLOBAL(df_finalize,DF_FINALIZE)
	(int* ierr_f);
}
#endif
