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
 * \date July 2011
 * \author Matthieu Dorier
 * \version 0.1
 * \see Client.hpp Client.h
 * Definition of the function used in the C binding.
 */
#include "client/Client.hpp"

extern "C" {

#include "client/Client.h"

Damaris::Client *client;

int DC_initialize(const char* configfile, int32_t core_id)
{
	std::string config_str(configfile);
	client = new Damaris::Client(&config_str,core_id);
	return 0;
}
	
int DC_write(const char* varname, int32_t iteration, const void* data)
{
	std::string varname_str(varname);
	return client->write(&varname_str,iteration,data);
}

void* DC_alloc(const char* varname, int32_t iteration)
{
	std::string varname_str(varname);
	return client->alloc(&varname_str,iteration);
}

int DC_commit(const char* varname, int32_t iteration)
{
	std::string varname_str(varname);
	return client->commit(&varname_str,iteration);
}

int DC_signal(const char* signal_name, int32_t iteration)
{
	std::string signal_name_str(signal_name);
	return client->signal(&signal_name_str,iteration);
}

int DC_get_parameter(const char* param_name, void* buffer)
{
	std::string paramName(param_name);
	return client->getParameter(&paramName,buffer);
}

int DC_finalize()
{
	delete client;
	return 0;
}

}
