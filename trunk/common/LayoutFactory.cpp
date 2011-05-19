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

#ifdef __ENABLE_FORTRAN
	#include "common/FCMangle.h"
#endif

#include <stdio.h>
#include "common/Layout.hpp"
#include "common/LayoutFactory.hpp"

namespace Damaris {
	
	
	
}

/** Note: the following functions allow the definition
 * of a layout from a fortran program, they are used because
 * the configuration part of Damaris is not programmed yet,
 * but they are not part of the Damaris API.
 */

// for Fortran binding
extern "C" {
#ifdef __ENABLE_FORTRAN

	int FC_FUNC_GLOBAL(df_define_a3d_layout,DC_DEFINE_A3D_LAYOUT)
		(int64_t* lptr, int32_t* type, int32_t* start_z, int32_t* end_z,
		int32_t* start_y, int32_t* end_y, int32_t* start_x, int32_t* end_x)
	{
		std::vector<int64_t> args(6,0);
		args[0] = (int64_t)(*start_x);
		args[1] = (int64_t)(*end_x);
		args[2] = (int64_t)(*start_y);
		args[3] = (int64_t)(*end_y);
		args[4] = (int64_t)(*start_z);
		args[5] = (int64_t)(*end_z);
		*lptr = (int64_t)(new Damaris::Layout((Damaris::basic_type_e)(*type),(int32_t)3,args));
		
		return 0;
	}
	
	int FC_FUNC_GLOBAL(df_define_a2d_layout,DC_DEFINE_A2D_LAYOUT)
		(int64_t* lptr, int32_t* type, int32_t* start_y, int32_t* end_y,
		int32_t* start_x, int32_t* end_x)
	{
		std::vector<int64_t> args(4,0);
		args[0] = (int64_t)(*start_x);
		args[1] = (int64_t)(*end_x);
		args[2] = (int64_t)(*start_y);
		args[3] = (int64_t)(*end_y);
		*lptr = (int64_t)(new Damaris::Layout((Damaris::basic_type_e)(*type), (int32_t)2,args));
		
		return 0;
	}
	
	int FC_FUNC_GLOBAL(df_define_a1d_layout,DC_DEFINE_A1D_LAYOUT)
		(int64_t* lptr, int32_t* type, int32_t* start_x, int32_t* end_x)
	{
		std::vector<int64_t> args(2,0);
		args[0] = (int64_t)(*start_x);
		args[1] = (int64_t)(*end_x);
		*lptr = (int64_t)(new Damaris::Layout((Damaris::basic_type_e)(*type), (int32_t)1,args));
		
		return 0;
	}
	
	int FC_FUNC_GLOBAL(df_define_scalar_layout,DC_DEFINE_SCALAR_LAYOUT)
		(int64_t* lptr, int32_t* type)
	{
		*lptr = (int64_t)(new Damaris::Layout((Damaris::basic_type_e)(*type),(int32_t)0));
		return 0;
	}
	
	int FC_FUNC_GLOBAL(df_free_layout,DC_FREE_LAYOUT)
		(int64_t* lptr) 
	{
		delete ((Damaris::Layout*)(*lptr));
		return 0;
	}
#endif	
}
