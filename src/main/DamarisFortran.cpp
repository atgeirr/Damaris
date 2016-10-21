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
#include <string>
#include <string.h>

#include "Damaris.h"

#ifdef __ENABLE_FORTRAN
#include "DamarisFCMangle.h"

#ifndef FC_FUNC_GLOBAL
#define FC_FUNC_GLOBAL(name,NAME) name##_
#endif

#ifndef FC_FUNC_GLOBAL_
#define FC_FUNC_GLOBAL_(name,NAME) name##_
#endif

// Converts a fortran string to a cpp string and deletes any
// blanc spaces at the end of the string
static std::string FortranStringToCpp(const char* fstr, int size)
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

void FC_FUNC_GLOBAL(damaris_initialize_f,DAMARIS_INITIALIZED_F)
	(const char* configfile, MPI_Fint* comm, int* err, int size)
{
	MPI_Comm c = MPI_Comm_f2c(*comm);
	std::string conf = FortranStringToCpp(configfile,size);
	*err = damaris_initialize(conf.c_str(),c);
}

void FC_FUNC_GLOBAL(damaris_finalize_f,DAMARIS_FINALIZE_F)(int* err)
{
	*err = damaris_finalize();
}

void FC_FUNC_GLOBAL(damaris_start_f,DAMARIS_START_F)(int* is_client, int* err)
{
	*err = damaris_start(is_client);
}

void FC_FUNC_GLOBAL(damaris_stop_f,DAMARIS_STOP_F)(int* err)
{
	*err = damaris_stop();
}


void FC_FUNC_GLOBAL(damaris_write_f,DAMARIS_WRITE_F)
	(const char* varname, const void* data, int* err, int size)
{
	std::string var = FortranStringToCpp(varname,size);
	*err = damaris_write(var.c_str(),data);
}


void FC_FUNC_GLOBAL(damaris_write_block_f,DAMARIS_WRITE_BLOCK_F)
	(const char* varname, int* block, const void* data, int* err, int size)
{
	std::string var = FortranStringToCpp(varname,size);
	*err = damaris_write_block(var.c_str(),*block,data);
}

void* FC_FUNC_GLOBAL(damaris_alloc_f,DAMARIS_ALLOC_F)
	(const char* varname, int* err, int size)
{
	void* ptr = NULL;
	std::string var = FortranStringToCpp(varname,size);
	*err = damaris_alloc(var.c_str(),&ptr);
	return ptr;
}

void* FC_FUNC_GLOBAL(damaris_alloc_block_f,DAMARIS_ALLOC_BLOCK_F)
	(const char* varname, int* block, int* err, int size)
{
	void* ptr = NULL;
	std::string var = FortranStringToCpp(varname,size);
	*err = damaris_alloc_block(var.c_str(),*block,&ptr);
	return ptr;
}

void FC_FUNC_GLOBAL(damaris_commit_f,DAMARIS_COMMIT_F)
	(const char* varname, int* err, int size)
{
	std::string var = FortranStringToCpp(varname,size);
	*err = damaris_commit(var.c_str());
}

void FC_FUNC_GLOBAL(damaris_commit_block_f,DAMARIS_COMMIT_BLOCK_F)
	(const char* varname, int* block, int* err, int size)
{
	std::string var = FortranStringToCpp(varname,size);
	*err = damaris_commit_block(var.c_str(),*block);
}

void FC_FUNC_GLOBAL(damaris_commit_iteration_f,DAMARIS_COMMIT_ITERATION_F)
	(const char* varname, int* iteration, int* err, int size)
{
	std::string var = FortranStringToCpp(varname,size);
	*err = damaris_commit_iteration(var.c_str(),*iteration);
}

void FC_FUNC_GLOBAL(damaris_commit_block_iteration_f,DAMARIS_COMMIT_BLOCK_ITERATION)
	(const char* varname, int* block, int* iteration, int* err, int size)
{
	std::string var = FortranStringToCpp(varname,size);
	*err = damaris_commit_block_iteration(var.c_str(),*block,*iteration);
}

void FC_FUNC_GLOBAL(damaris_clear_f,DAMARIS_CLEAR_F)
	(const char* varname, int* err, int size)
{
	std::string var = FortranStringToCpp(varname,size);
	*err = damaris_clear(var.c_str());
}

void FC_FUNC_GLOBAL(damaris_clear_block_f,DAMARIS_CLEAR_BLOCK_F)
	(const char* varname, int* block, int* err, int size)
{
	std::string var = FortranStringToCpp(varname,size);
	*err = damaris_clear_block(var.c_str(),*block);
}

void FC_FUNC_GLOBAL(damaris_clear_iteration_f,DAMARIS_CLEAR_ITERATION_F)
	(const char* varname, int* iteration, int* err, int size)
{
	std::string var = FortranStringToCpp(varname,size);
	*err = damaris_clear_iteration(var.c_str(),*iteration);
}

void FC_FUNC_GLOBAL(damaris_clear_block_iteration_f,DAMARIS_CLEAR_BLOCK_ITERATION_F)
	(const char* varname, int* block, int* iteration, int* err, int size)
{
	std::string var = FortranStringToCpp(varname,size);
	*err = damaris_clear_block_iteration(var.c_str(),*block,*iteration);
}


void FC_FUNC_GLOBAL(damaris_signal_f,DAMARIS_SIGNAL_F)
	(const char* signal_name, int* err, int size)
{
	std::string sig = FortranStringToCpp(signal_name,size);
	*err = damaris_signal(sig.c_str());
}


void FC_FUNC_GLOBAL(damaris_parameter_get_f,DAMARIS_PARAMETER_GET_F)
	(const char* pname, void* buffer, int* bufsize, int* err, int size)
{
	std::string name = FortranStringToCpp(pname,size);
	unsigned int max = (unsigned int)(*bufsize);
	*err = damaris_parameter_get(name.c_str(), buffer, max);
}

void FC_FUNC_GLOBAL(damaris_parameter_set_f,DAMARIS_PARAMETER_SET_F)
	(const char* pname, const void* buffer, int* bufsize, int* err, int size)
{
	std::string name = FortranStringToCpp(pname,size);
	unsigned int max = (unsigned int)(*bufsize);
	*err = damaris_parameter_set(name.c_str(), buffer, max);
}

void FC_FUNC_GLOBAL(damaris_set_position_f,DAMARIS_SET_POSITION_F)
	(const char* varname, const int64_t* position, int* err, int size)
{
	std::string var = FortranStringToCpp(varname,size);
	*err = damaris_set_position(var.c_str(),position);
}

void FC_FUNC_GLOBAL(damaris_set_block_position_f,DAMARIS_SET_BLOCK_POSITION_F)
	(const char* varname, int* block, 
	 const int64_t* position, int* err, int size)
{
	std::string var = FortranStringToCpp(varname,size);
	*err = damaris_set_block_position(var.c_str(),*block,position);
}

void FC_FUNC_GLOBAL(damaris_client_comm_get_f,DAMARIS_CLIENT_COMM_GET_F)
	(MPI_Fint* comm, int* err)
{
	MPI_Comm c;
	*err = damaris_client_comm_get(&c);
	if(*err == DAMARIS_OK) {
		*comm = MPI_Comm_c2f(c);
	}
}

void FC_FUNC_GLOBAL(damaris_end_iteration_f,DAMARIS_END_ITERATION_F)(int* err)
{
	*err = damaris_end_iteration();
}

void FC_FUNC_GLOBAL(damaris_get_iteration_f,DAMARIS_GET_ITERATION_F)
	(int* iteration, int* err)
{
	*err = damaris_get_iteration(iteration);
}

}

#endif