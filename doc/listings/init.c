#include "include/Damaris.h"

void sim_main_loop()
{
	int i;
	for(i=0;i<100;i++) {
		// do something
		DC_end_iteration();
	}
}
 
int main(int argc, char** argv)
{
	int id = 0;
	int err;
	err = DC_initialize("config.xml",MPI_COMM_WORLD);
	sim_main_loop();
	DC_finalize();
	return 0;
}
