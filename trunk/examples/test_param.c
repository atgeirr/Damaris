#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "include/Damaris.h"  

int client_code(int id) 
{
	int x = 0;
	float y = .0;

	int res;

	printf("Starting code...\n");

	res = DC_parameter_get("x",&x,sizeof(int));
	printf("res = %d\n",res);
	res = DC_parameter_get("y",&y,sizeof(float));
	printf("res = %d\n",res);

	printf("x = %d, y = %f\n",x,y);
	
	x = 42;
	y = 13.37;

	res = DC_parameter_set("x",&x,sizeof(int));
	printf("res = %d\n",res);
	res = DC_parameter_set("y",&y,sizeof(float));
	printf("res = %d\n",res);

	int x2;
	float y2;

	res = DC_parameter_get("x",&x2,sizeof(int));
	printf("res = %d\n",res);
	res = DC_parameter_get("y",&y2,sizeof(float));
	printf("res = %d\n",res);

	printf("x = %d, y = %f\n",x2,y2);

	return 0;
}    

int main(int argc, char** argv) 
{
	MPI_Comm all = MPI_COMM_WORLD;
	MPI_Comm clients;
	int rank, size;

	if(argc != 2) {
		printf("Usage: mpirun -np 2 ./test_param_c <config.xml>\n");
		exit(0);
	}
	
	MPI_Init(&argc,&argv);
	int client = DC_mpi_init_and_start(argv[1],all);

	if(client) {
		clients = DC_mpi_get_client_comm();
		MPI_Comm_rank(clients,&rank);
		MPI_Comm_size(clients,&size);
		client_code(rank);
		DC_kill_server();
		MPI_Barrier(clients);
		DC_finalize();
	}

	MPI_Finalize();
	return 0;
}
