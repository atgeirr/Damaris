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
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 * \see Client.hpp
 * Definition of the Fortran functions.
 */
#ifdef __ENABLE_FORTRAN
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
		client = new Damaris::Client(config_file_name,*core_id_f);
		*ierr_f = 0;
	}

void FC_FUNC_GLOBAL(df_write,DF_WRITE)
	(char* var_name_f, int32_t* iteration_f, void* data_f, int32_t* ierr_f, int var_name_size)
	{
		// make a copy of the name and delete possible spaces at the end of the string
		char* varname_copy = (char*)malloc(var_name_size+1);
		memset(varname_copy,' ',var_name_size+1);
		memcpy(varname_copy,var_name_f,var_name_size);
		int i = var_name_size;
		while(varname_copy[i] == ' ' && i != 0) i--;
		varname_copy[i+1] = '\0';

		std::string var_name(varname_copy);
		*ierr_f = client->write(var_name,*iteration_f,data_f);
		free(varname_copy);
	}

void FC_FUNC_GLOBAL_(df_chunk_set,DF_CHUNK_SET)
	(unsigned int* dimensions, int* si, int* ei, int64_t* chunkh)
	{
		std::vector<int> sti(si,si+(*dimensions));
		std::vector<int> eni(ei,ei+(*dimensions));
		// arrays in Fortran must be reversed
		std::reverse(sti.begin(),sti.end());
		std::reverse(eni.begin(),eni.end());
		*chunkh = (int64_t)(client->chunk_set(*dimensions,sti,eni));
	}

void FC_FUNC_GLOBAL_(df_chunk_free,DF_CHUNK_FREE)
	(int64_t* chunkh)
	{
		client->chunk_free((Damaris::chunk_h)(*chunkh));
		chunkh = NULL;
	}

void FC_FUNC_GLOBAL(df_chunk_write,DF_WRITE)
	(int64_t* chunkh, char* var_name_f, int32_t* iteration_f, 
	void* data_f, int32_t* ierr_f, int var_name_size)
	{
		// make a copy of the name and delete possible spaces at the end of the string
		char* varname_copy = (char*)malloc(var_name_size+1);
		memset(varname_copy,' ',var_name_size+1);
		memcpy(varname_copy,var_name_f,var_name_size);
		int i = var_name_size;
		while(varname_copy[i] == ' ' && i != 0) i--;
		varname_copy[i+1] = '\0';

		std::string var_name(varname_copy);
		*ierr_f = client->chunk_write((Damaris::chunk_h)(*chunkh),
				var_name,*iteration_f,data_f);
		free(varname_copy);
	}

void* FC_FUNC_GLOBAL(df_alloc,DF_ALLOC)
        (char* var_name_f, int32_t* iteration_f, int32_t* ierr_f, int var_name_size)
	{
		// make a copy of the name and delete possible spaces at the end of the string
		char* varname_copy = (char*)malloc(var_name_size+1);
		memset(varname_copy,' ',var_name_size+1);
		memcpy(varname_copy,var_name_f,var_name_size);
		int i = var_name_size;
		while(varname_copy[i] == ' ' && i != 0) i--;
		varname_copy[i+1] = '\0';

		std::string var_name(varname_copy);
		void* result = client->alloc(var_name,*iteration_f);
		free(varname_copy);
		DBG("function alloc called with argument " << var_name << ", " << *iteration_f);
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
		// make a copy of the name and delete possible spaces at the end of the string
		char* varname_copy = (char*)malloc(var_name_size+1);
		memset(varname_copy,' ',var_name_size+1);
		memcpy(varname_copy,var_name_f,var_name_size);
		int i = var_name_size;
		while(varname_copy[i] == ' ' && i != 0) i--;
		varname_copy[i+1] = '\0';

		std::string var_name(varname_copy);
		DBG("commiting " << var_name);
		*ierr_f = client->commit(var_name,*iteration_f);
		free(varname_copy);
	}

void FC_FUNC_GLOBAL(df_signal,DF_SIGNAL)
	(char* event_name_f, int32_t* iteration_f, int* ierr_f, int event_name_size)
	{
		// make a copy of the name and delete possible spaces at the end of the string
		char* eventname_copy = (char*)malloc(event_name_size+1);
		memset(eventname_copy,' ',event_name_size+1);
		memcpy(eventname_copy,event_name_f,event_name_size);
		int i = event_name_size;
		while(eventname_copy[i] == ' ' && i != 0) i--;
		eventname_copy[i+1] = '\0';

		std::string event_name(eventname_copy);
		*ierr_f = client->signal(event_name,*iteration_f);
		free(eventname_copy);
	}

void FC_FUNC_GLOBAL(df_get_parameter,DF_GET_PARAMETER)
	(char* param_name_f, void* buffer_f, int* ierr_f, int param_name_size)
	{
		// make a copy of the name and delete possible spaces at the end of the string
		char* paramname_copy = (char*)malloc(param_name_size+1);
		memset(paramname_copy,' ',param_name_size+1);
		memcpy(paramname_copy,param_name_f,param_name_size);
		int i = param_name_size;
		while(paramname_copy[i] == ' ' && i != 0) i--;
		paramname_copy[i+1] = '\0';

		std::string paramName(paramname_copy);
		*ierr_f = client->get_parameter(paramName,buffer_f);
		free(paramname_copy);
	}

void FC_FUNC_GLOBAL(df_kill_server,DF_KILL_SERVER)
	(int* ierr_f)
	{
		*ierr_f = client->kill_server();
	}

void FC_FUNC_GLOBAL(df_finalize,DF_FINALIZE)
	(int* ierr_f)
	{
		delete client;
		*ierr_f = 0;
	}
}
#endif
