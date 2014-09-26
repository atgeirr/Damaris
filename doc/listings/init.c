#include "Damaris.h"

void sim_main_loop(MPI_Comm comm)
{
	int i;
	for(i=0;i<100;i++) {
		// do something
        // using comm as global communicator
		damaris_end_iteration();
	}
}
 
int main(int argc, char** argv)
{
    MPI_Init(&argc,&argv);
    int id = 0;
    int err;
    int is_client;
    MPI_Comm global;
    err = damaris_initialize("config.xml",MPI_COMM_WORLD);
    damaris_start(&is_client);
    if(is_client) {
        damaris_client_comm_get(&global);
        sim_main_loop();
        damaris_stop();
    }
    damaris_finalize();
    MPI_Finalize();
    return 0;
}
