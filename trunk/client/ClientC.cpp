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
 * \file ClientC.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 * \see Client.hpp Client.h
 * Definition of the client function used in the C binding.
 */
#include "client/Client.hpp"
#include "xml/BcastXML.hpp"

extern "C" {

#include "client/Client.h"

extern Damaris::Client *__client;

int DC_initialize(MPI_Comm comm, const char* configfile)
{
	std::string config_str(configfile);
	std::auto_ptr<Damaris::Model::Simulation> mdl
		= Damaris::Model::BcastXML(comm,config_str);
	int core_id;
	MPI_Comm_rank(comm,&core_id);
	__client = Damaris::Client::New(mdl,core_id);
	return 0;
}
	
int DC_write(const char* varname, const void* data)
{
	std::string varname_str(varname);
	return __client->write(varname_str,data);
}

int DC_write_block(const char* varname, int32_t block, const void* data)
{
	std::string varname_str(varname);
	return __client->write_block(varname_str,block,data);
}

/*
int DC_chunk_write(DC_chunk_handle_t chunkh, const char* varname, 
		int32_t iteration, const void* data)
{
	std::string varname_str(varname);
	return __client->chunk_write((Damaris::chunk_h)chunkh,varname_str,iteration,data);
}
*/
/*
DC_chunk_handle_t DC_chunk_set(unsigned int dimensions, int* si, int* ei)
{
	std::vector<int> sti(si,si+dimensions);
	std::vector<int> eni(ei,ei+dimensions);
	return (DC_chunk_handle_t)__client->chunk_set(dimensions,sti,eni);
}
*/
/*
void DC_chunk_free(DC_chunk_handle_t chunkh)
{
	__client->chunk_free((Damaris::chunk_h)chunkh);
}
*/

void* DC_alloc(const char* varname)
{
	std::string varname_str(varname);
	return __client->alloc(varname_str);
}

void* DC_alloc_block(const char* varname, int32_t block)
{
	std::string varname_str(varname);
	return __client->alloc_block(varname_str,block);
}

int DC_commit(const char* varname)
{
	std::string varname_str(varname);
	return __client->commit(varname_str);
}

int DC_commit_block(const char* varname, int32_t block)
{
	std::string varname_str(varname);
	return __client->commit_block(varname_str,block);
}

int DC_commit_iteration(const char* varname, int32_t iteration)
{
	std::string varname_str(varname);
	return __client->commit(varname_str,iteration);
}

int DC_commit_block_iteration(const char* varname, int32_t block, int32_t iteration)
{
	std::string varname_str(varname);
	return __client->commit_block(varname_str,block,iteration);
}

int DC_signal(const char* signal_name)
{
	std::string signal_name_str(signal_name);
	return __client->signal(signal_name_str);
}

int DC_parameter_get(const char* param_name, void* buffer, unsigned int size)
{
	std::string paramName(param_name);
	return __client->get_parameter(paramName,buffer,size);
}

int DC_parameter_set(const char* param_name, const void* buffer, unsigned int size)
{
	std::string paramName(param_name);
	return __client->set_parameter(paramName,buffer,size);
}

MPI_Comm DC_mpi_get_client_comm()
{
	return __client->mpi_get_client_comm();
}

int DC_kill_server()
{
	return __client->kill_server();
}

int DC_end_iteration()
{
	return __client->end_iteration();
}

int DC_finalize()
{
	delete __client;
	return 0;
}

}
