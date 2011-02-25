#include <stdio.h>
#include "common/Layout.hpp"
#include "common/LayoutFactory.hpp"

namespace Damaris {
	
	
	
}


// for Fortran binding
extern "C" {
	
	int dc_define_a3d_layout_(int64_t* lptr, int32_t* type, int32_t* start_z, int32_t* end_z,
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
	
	int dc_define_a2d_layout_(int64_t* lptr, int32_t* type, int32_t* start_y, int32_t* end_y,
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
	
	int dc_define_a1d_layout_(int64_t* lptr, int32_t* type, int32_t* start_x, int32_t* end_x)
	{
		std::vector<int64_t> args(2,0);
		args[0] = (int64_t)(*start_x);
		args[1] = (int64_t)(*end_x);
		*lptr = (int64_t)(new Damaris::Layout((Damaris::basic_type_e)(*type), (int32_t)1,args));
		
		return 0;
	}
	
	int dc_define_scalar_layout_(int64_t* lptr, int32_t* type)
	{
		*lptr = (int64_t)(new Damaris::Layout((Damaris::basic_type_e)(*type),(int32_t)0));
		return 0;
	}
	
	int dc_free_layout_(int64_t* lptr) 
	{
		delete ((Damaris::Layout*)(*lptr));
		return 0;
	}
	
}
