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
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 * \see Client.hpp
 * Definition of the Fortran functions.
 */
#ifdef __ENABLE_FORTRAN
#include "core/Debug.hpp"
#include "core/FCMangle.h"
#include "client/ClientFortran.hpp"

// Converts a fortran string to a cpp string and deletes any
// blanc spaces at the end of the string
static std::string fortranStringToCpp(char* fstr, int size)
{
	char* fstr_copy = (char*)malloc(size+1);
	memset(fstr_copy,' ',size+1);
	memcpy(fstr_copy,fstr,size);
	int i = size;
	while(fstr_copy[i] == ' ' && i != 0) i--;
	fstr_copy[i+1] = '\0';
	std::string s(fstr_copy);
	free(fstr_copy);
	return s;
}

extern "C" {

/** The actual object is defined in ClientC.cpp */	
extern Damaris::Client *client;

void FC_FUNC_GLOBAL(df_initialize,DF_INITIALIZE)
	(char* config_file_name_f, int32_t* core_id_f, int32_t* ierr_f, int config_file_name_size)
	{
		std::string config_file_name = fortranStringToCpp(config_file_name_f, config_file_name_size);
		client = Damaris::Client::New(config_file_name,*core_id_f);
		if(ierr_f != NULL) *ierr_f = 0;
	}

void FC_FUNC_GLOBAL(df_write,DF_WRITE)
	(char* var_name_f, int32_t* iteration_f, void* data_f, int32_t* ierr_f, int var_name_size)
	{
		std::string var_name = fortranStringToCpp(var_name_f, var_name_size);
		int res = client->write(var_name,*iteration_f,data_f);
		if(ierr_f != NULL) *ierr_f = res;
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
		std::string var_name = fortranStringToCpp(var_name_f,var_name_size);
		int res = client->chunk_write((Damaris::chunk_h)(*chunkh),
				var_name,*iteration_f,data_f);
		if(ierr_f != NULL) *ierr_f = res;
	}

void* FC_FUNC_GLOBAL(df_alloc,DF_ALLOC)
        (char* var_name_f, int32_t* iteration_f, int32_t* ierr_f, int var_name_size)
	{
		std::string var_name = fortranStringToCpp(var_name_f,var_name_size);
		void* result = client->alloc(var_name,*iteration_f);
		DBG("function alloc called with argument " << var_name << ", " << *iteration_f);
		if(result == NULL) {
			if(ierr_f != NULL) *ierr_f = -1;
			return NULL;
		} else {
			if(ierr_f != NULL) *ierr_f = 0;
			return result;
		}
	}

void FC_FUNC_GLOBAL(df_commit,DF_COMMIT)
	(char* var_name_f, int32_t* iteration_f, int32_t* ierr_f, int var_name_size)
	{
		std::string var_name = fortranStringToCpp(var_name_f,var_name_size);
		DBG("commiting " << var_name);
		int res = client->commit(var_name,*iteration_f);
		if(ierr_f != NULL) *ierr_f = res;
	}

void FC_FUNC_GLOBAL(df_signal,DF_SIGNAL)
	(char* event_name_f, int32_t* iteration_f, int* ierr_f, int event_name_size)
	{
		std::string event_name = fortranStringToCpp(event_name_f,event_name_size);
		int res = client->signal(event_name,*iteration_f);
		if(ierr_f != NULL) *ierr_f = res;
	}

void FC_FUNC_GLOBAL(df_get_parameter,DF_GET_PARAMETER)
	(char* param_name_f, void* buffer_f, int* size, int* ierr_f, int param_name_size)
	{
		std::string paramName = fortranStringToCpp(param_name_f,param_name_size);
		int res = client->get_parameter(paramName,buffer_f,*size);
		if(ierr_f != NULL) *ierr_f = res;
	}

void FC_FUNC_GLOBAL(df_set_parameter,DF_SET_PARAMETER)
        (char* param_name_f, void* buffer_f, int* size, int* ierr_f, int param_name_size)
	{
		std::string paramName = fortranStringToCpp(param_name_f,param_name_size);
		int res = client->set_parameter(paramName,buffer_f,*size);
		if(ierr_f != NULL) *ierr_f = res;
	}

void FC_FUNC_GLOBAL(df_mpi_get_client_comm,DF_GET_MPI_CLIENT_COMM)
        (MPI_Fint* fcomm)
	{
		*fcomm = MPI_Comm_c2f(client->mpi_get_client_comm());
	}

void FC_FUNC_GLOBAL(df_kill_server,DF_KILL_SERVER)
	(int* ierr_f)
	{
		int res = client->kill_server();
		if(ierr_f != NULL) *ierr_f = res;
	}

void FC_FUNC_GLOBAL(df_end_iteration,DF_END_ITERATION)
	(int* it, int* ierr_f)
	{
		int res = client->end_iteration(*it);
		if(ierr_f != NULL) *ierr_f = res;
	}

void FC_FUNC_GLOBAL(df_finalize,DF_FINALIZE)
	(int* ierr_f)
	{
		int res;
		if(client != NULL) {
			delete client;
			res = 0;
		} else {
			res = -1;
		}
		if(ierr_f != NULL)
			*ierr_f = res;
	}
}
#endif
