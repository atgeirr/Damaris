#include "include/Damaris.h"

void sim_main_loop(MPI_Comm c)
{
	int i;
	for(i=0;i<100;i++) {
		// do something
		DC_end_iteration();
	}
}
 
int main(int argc, char** argv)
{
	MPI_Init(&argc,&argv);
	if(DC_mpi_init_and_start("config.xml",MPI_COMM_WORLD)) {
		MPI_Comm comm = DC_mpi_get_client_comm();
		sim_main_loop(comm);
		DC_kill_server();
		DC_finalize();
	}
	MPI_Finalize();
	return 0;
}
