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

#ifndef __DAMARIS_CLIENT_C_H
#define __DAMARIS_CLIENT_C_H

#include <stdlib.h>

int 	DC_initialize(const char* configfile, int32_t core_id);

int 	DC_write(const char* varname, int32_t iteration, const void* data);

void* 	DC_alloc(const char* varname, int32_t iteration);

int 	DC_commit(const char* varname, int32_t iteration);

int 	DC_signal(const char* signal_name, int32_t iteration);

int 	DC_get_parameter(const char* param_name, void* buffer);

int 	DC_finalize();

#endif
