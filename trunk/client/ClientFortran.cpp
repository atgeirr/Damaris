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
 * \file ClientFortran.cpp
 * \date July 2011
 * \author Matthieu Dorier
 * \version 0.1
 * \see Client.hpp
 * Definition of the Fortran functions.
 */
#ifdef __ENABLE_FORTRAN
#define __DEBUG
#include "common/Debug.hpp"
#include "common/FCMangle.h"
#include "client/ClientFortran.hpp"

extern "C" {

/** The actual object is defined in ClientC.cpp */	
extern Damaris::Client *client;

void FC_FUNC_GLOBAL(df_initialize,DF_INITIALIZE)
	(char* config_file_name_f, int32_t* core_id_f, int32_t* ierr_f, int config_file_name_size)
	{
		std::string config_file_name(config_file_name_f, config_file_name_size);
		client = new Damaris::Client(&config_file_name,*core_id_f);
		*ierr_f = 0;
	}

void FC_FUNC_GLOBAL(df_write,DF_WRITE)
	(char* var_name_f, int32_t* iteration_f, void* data_f, int32_t* ierr_f, int var_name_size)
	{
		std::string var_name(var_name_f,var_name_size);
		*ierr_f = client->write(&var_name,*iteration_f,data_f);
	}

void* FC_FUNC_GLOBAL(df_alloc,DF_ALLOC)
        (char* var_name_f, int32_t* iteration_f, int32_t* ierr_f, int var_name_size)
	{
		std::string var_name(var_name_f,var_name_size);
		void* result = client->alloc(&var_name,*iteration_f);
		DBG("function alloc called with argument " << var_name.c_str() << ", " << *iteration_f);
		if(result == NULL) {
			*ierr_f = -1;
			return NULL;
		} else {
			*ierr_f = 0;
			return result;
		}
	}

void FC_FUNC_GLOBAL(df_commit,DF_COMMIT)
	(char* var_name_f, int32_t* iteration_f, int32_t* ierr_f, int var_name_size)
	{
		std::string var_name(var_name_f,var_name_size);
		DBG("commiting " << var_name.c_str());
		*ierr_f = client->commit(&var_name,*iteration_f);
	}

void FC_FUNC_GLOBAL(df_signal,DF_SIGNAL)
	(char* event_name_f, int32_t* iteration_f, int* ierr_f, int event_name_size)
	{
		std::string event_name(event_name_f,event_name_size);
		*ierr_f = client->signal(&event_name,*iteration_f);
	}

void FC_FUNC_GLOBAL(df_get_parameter,DF_GET_PARAMETER)
	(char* param_name_f, void* buffer_f, int* ierr_f, int param_name_size)
	{
		std::string paramName(param_name_f,param_name_size);
		*ierr_f = client->getParameter(&paramName,buffer_f);
	}

void FC_FUNC_GLOBAL(df_finalize,DF_FINALIZE)
	(int* ierr_f)
	{
		delete client;
		*ierr_f = 0;
	}
}
#endif
