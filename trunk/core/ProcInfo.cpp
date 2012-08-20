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
 * \file ProcInfo.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.5
 */
#include <stdint.h>
#include <string.h>

#include "core/ProcInfo.hpp"

#ifdef BGP
	#include <spi/kernel_interface.h>
	#include <common/bgp_personality.h>
	#include <common/bgp_personality_inlines.h>

	#define MAX_PROCESSOR_NAME 128
#else
	#include <mpi.h>

	#define MAX_PROCESSOR_NAME MPI_MAX_PROCESSOR_NAME
#endif

namespace Damaris {

namespace ProcInfo {

static int computeHashValue(char* str, size_t len)
{
	uint64_t nhash = (uint64_t)(14695981039346656037ULL);
	uint64_t fnv =  ((uint64_t)1 << 40) + (1 << 8) + 0xb3;
	for(unsigned int i=0; i < len; i++) {
		uint64_t c = (uint64_t)(str[i]);
		nhash = nhash xor c;
		nhash *= fnv;
	}
	return ((int)nhash >= 0) ? (int)nhash : - ((int)nhash);
}

int GetNodeID()
{
	char procname[MAX_PROCESSOR_NAME];
	int len = 0;
#ifdef BGP
	_BGP_Personality_t personality;
	Kernel_GetPersonality(&personality, sizeof(personality));
	BGP_Personality_getLocationString(&personality, procname);
	len = strlen(procname);
#else
	MPI_Get_processor_name(procname,&len);
#endif
	return computeHashValue(procname,len);
}



}

} // namespace Damaris

