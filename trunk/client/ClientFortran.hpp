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
 * \date July 2011
 * \author Matthieu Dorier
 * \version 0.1
 * \see Client.hpp
 * Definition of the Fortran functions.
 */
#ifdef __ENABLE_FORTRAN
#include "common/FCMangle.h"
#include "client/Client.hpp"

extern "C" {
/**
 * \fn df_initialize(config_file_name, core_id, ierr)
 * \brief Initializes the client-side Fortran library.
 * \see Damaris::Client::Client
 * \param[in] config_file_name : name of the XML configuration file (character*).
 * \param[in] core_id : identifier for the client (should be unique).
 * \param[out] ierr : returned error (0 in case of success, forces the program to stop in case of failure).
 */
void FC_FUNC_GLOBAL(df_initialize,DF_INITIALIZE)
	(char* config_file_name_f, int32_t* core_id_f, int32_t* ierr_f, int config_file_name_size);

/** 
 * \fn df_write(var_name, iteration, data, ierr)
 * \brief Writes a variable.
 * \see Damaris::Client::write
 * \param[in] var_name : name of the variable to write (character*).
 * \param[in] iteration : iteration at which the variable is written (integer).
 * \param[in] data : pointer to the data to write (integer).
 * \param[out] ierr: error core. 0 in case of success,
 *         -1 if the layout or the variable is not defined,
 *         -2 if the layout has a bad size (0 or < 0),
 *         -3 if it fails to allocate shared memory to write.
 */	
void FC_FUNC_GLOBAL(df_write,DF_WRITE)
	(char* var_name_f, int32_t* iteration_f, void* data_f, int32_t* ierr_f, int var_name_size);

/**
 * \fn df_signal(event_name, iteration, ierr)
 * \brief Sends an event to the dedicated core.
 * \see Damaris::Client::signal
 * \param[in] event_name : name of the event (character*).
 * \param[in] iteration : iteration at which the event is sent.
 * \param[out] ierr : 0 in case of success, -1 in case of failure.
 */	
void FC_FUNC_GLOBAL(df_signal,DF_SIGNAL)
	(char* event_name_f, int32_t* iteration_f, int* ierr_f, int event_name_size);

/**
 * \fn df_get_parameter(param_name, buffer, ierr)
 * \brief Retrieve the value associated to a parameter.
 * \see Damaris::Client::getParameter
 * \param[in] param_name : name of the parameter (character*).
 * \param[out] buffer : pointer to the buffer to hold the value.
 * \param[out] ierr: 0 in case of success, -1 if the parameter does not exist.
 */
void FC_FUNC_GLOBAL(df_get_parameter,DF_GET_PARAMETER)
	(char* param_name_f, void* buffer_f, int* ierr_f, int param_name_size);

/**
 * \fn df_finalize(ierr)
 * \brief Call client's destructor.
 * \see Damaris::Client::~Client
 * \param[out] ierr : 0
 */
void FC_FUNC_GLOBAL(df_finalize,DF_FINALIZE)
	(int* ierr_f);
}
#endif
