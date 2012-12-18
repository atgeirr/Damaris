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
 * \date November 2012
 * \author Matthieu Dorier
 * \version 0.7
 * \see Client.hpp
 * Definition of the Fortran functions.
 */
#ifdef __ENABLE_FORTRAN
#include "core/Debug.hpp"
#include "core/FCMangle.h"
#include "client/ClientFortran.hpp"
#include "xml/BcastXML.hpp"

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
extern Damaris::Client *__client;

void FC_FUNC_GLOBAL(df_initialize,DF_INITIALIZE)
	(char* config_file_name_f, MPI_Fint* fcomm, int32_t* ierr_f, int config_file_name_size)
	{
		std::string config_file_name = 
			fortranStringToCpp(config_file_name_f, config_file_name_size);
		MPI_Comm cc = MPI_Comm_f2c(*fcomm);
		std::auto_ptr<Damaris::Model::Simulation> mdl
			= Damaris::Model::BcastXML(cc, config_file_name);
		__client = Damaris::Client::New(mdl,cc);
		if(ierr_f != NULL) *ierr_f = 0;
	}

void FC_FUNC_GLOBAL(df_write_block,DF_WRITE)
	(char* var_name_f, int32_t* block_f, 
	 void* data_f, int32_t* ierr_f, int var_name_size)
	{
		std::string var_name = fortranStringToCpp(var_name_f, var_name_size);
		int res = __client->write_block(var_name,*block_f,data_f);
		if(ierr_f != NULL) *ierr_f = res;
	}

void FC_FUNC_GLOBAL(df_write,DF_WRITE)
        (char* var_name_f, 
         void* data_f, int32_t* ierr_f, int var_name_size)
	{
		std::string var_name = fortranStringToCpp(var_name_f, var_name_size);
		int res = __client->write(var_name,data_f);
		if(ierr_f != NULL) *ierr_f = res;
	}

void* FC_FUNC_GLOBAL(df_alloc,DF_ALLOC)
        (char* var_name_f, int32_t* ierr_f, int var_name_size)
	{
		std::string var_name = fortranStringToCpp(var_name_f,var_name_size);
		void* result = __client->alloc(var_name);
		if(result == NULL) {
			if(ierr_f != NULL) *ierr_f = -1;
			return NULL;
		} else {
			if(ierr_f != NULL) *ierr_f = 0;
			return result;
		}
	}

void* FC_FUNC_GLOBAL(df_alloc_block,DF_ALLOC_BLOCK)
	(char* var_name_f, int32_t* block_f, int32_t* ierr_f, int var_name_size)
	{
		std::string var_name = fortranStringToCpp(var_name_f,var_name_size);
		void* result = __client->alloc_block(var_name,*block_f);
		if(result == NULL) {
			if(ierr_f != NULL) *ierr_f = -1;
			return NULL;
		} else {
			if(ierr_f != NULL) *ierr_f = 0;
			return result;
		}
	}

void FC_FUNC_GLOBAL(df_commit,DF_COMMIT)
	(char* var_name_f, int32_t* ierr_f, int var_name_size)
	{
		std::string var_name = fortranStringToCpp(var_name_f,var_name_size);
		int res = __client->commit(var_name);
		if(ierr_f != NULL) *ierr_f = res;
	}

void FC_FUNC_GLOBAL(df_commit_block,DF_COMMIT_BLOCK)
        (char* var_name_f, int32_t* block_f, int32_t* ierr_f, int var_name_size)
        {
                std::string var_name = fortranStringToCpp(var_name_f,var_name_size);
                int res = __client->commit(var_name,*block_f);
                if(ierr_f != NULL) *ierr_f = res;
        }

void FC_FUNC_GLOBAL(df_commit_iteration,DF_COMMIT_ITERATION)
	(char* var_name_f, int32_t* iteration_f, int32_t* ierr_f, int var_name_size)
	{
		std::string var_name = fortranStringToCpp(var_name_f,var_name_size);
		int res = __client->commit(var_name,*iteration_f);
		if(ierr_f != NULL) *ierr_f = res;
	}

void FC_FUNC_GLOBAL(df_commit_block_iteration,DF_COMMIT_BLOCK_ITERATION)
        (char* var_name_f, int32_t* block_f, int32_t* iteration_f, int32_t* ierr_f, int var_name_size)
        {
                std::string var_name = fortranStringToCpp(var_name_f,var_name_size);
                int res = __client->commit_block(var_name,*iteration_f);
                if(ierr_f != NULL) *ierr_f = res;
        }


void FC_FUNC_GLOBAL(df_signal,DF_SIGNAL)
	(char* event_name_f, int* ierr_f, int event_name_size)
	{
		std::string event_name = fortranStringToCpp(event_name_f,event_name_size);
		int res = __client->signal(event_name);
		if(ierr_f != NULL) *ierr_f = res;
	}

void FC_FUNC_GLOBAL(df_parameter_get,DF_PARAMETER_GET)
	(char* param_name_f, void* buffer_f, int* size, int* ierr_f, int param_name_size)
	{
		std::string paramName = fortranStringToCpp(param_name_f,param_name_size);
		int res = __client->get_parameter(paramName,buffer_f,*size);
		if(ierr_f != NULL) *ierr_f = res;
	}

void FC_FUNC_GLOBAL(df_parameter_set,DF_PARAMETER_SET)
        (char* param_name_f, void* buffer_f, int* size, int* ierr_f, int param_name_size)
	{
		std::string paramName = fortranStringToCpp(param_name_f,param_name_size);
		int res = __client->set_parameter(paramName,buffer_f,*size);
		if(ierr_f != NULL) *ierr_f = res;
	}

void FC_FUNC_GLOBAL(df_mpi_get_client_comm,DF_GET_MPI_CLIENT_COMM)
        (MPI_Fint* fcomm)
	{
		*fcomm = MPI_Comm_c2f(__client->mpi_get_client_comm());
	}

void FC_FUNC_GLOBAL(df_kill_server,DF_KILL_SERVER)
	(int* ierr_f)
	{
		int res = __client->kill_server();
		if(ierr_f != NULL) *ierr_f = res;
	}

void FC_FUNC_GLOBAL(df_end_iteration,DF_END_ITERATION)
	(int* ierr_f)
	{
		int res = __client->end_iteration();
		if(ierr_f != NULL) *ierr_f = res;
	}

void FC_FUNC_GLOBAL(df_finalize,DF_FINALIZE)
	(int* ierr_f)
	{
		int res;
		if(__client != NULL) {
			delete __client;
			res = 0;
		} else {
			res = -1;
		}
		if(ierr_f != NULL)
			*ierr_f = res;
	}
}
#endif
