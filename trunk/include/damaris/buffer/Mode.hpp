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
#ifndef __DAMARIS_MODE_H
#define __DAMARIS_MODE_H

#include <sys/mman.h>

namespace damaris {

#define READ_WRITE (PROT_READ | PROT_WRITE)
#define	READ_ONLY   PROT_READ
#define	WRITE_ONLY  PROT_WRITE
#define NO_ACCESS   PROT_NONE
	
}

#endif
