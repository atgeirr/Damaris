#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "include/Damaris.h"  

int client_code(int id) 
{
	float mydata[64][16][4];
	char* varname = "my group/my variable";
	char* event = "my event";

	int i,j,k;
	for(i = 0; i < 64; i++) {
	for(j = 0; j < 16; j++) {
	for(k = 0; k <  4; k++) {
		mydata[i][j][k] = i*j*k;
	}
	}
	}

	DC_write(varname,0,mydata);
	DC_signal(event,0);
	return 0;
}    

int main(int argc, char** argv) 
{
	MPI_Comm all = MPI_COMM_WORLD;
	MPI_Comm clients;
	int rank, size;

	if(argc != 2) {
		printf("Usage: mpirun -np 2 ./test_mpi_c <config.xml>\n");
		exit(0);
	}
	
	MPI_Init(&argc,&argv);

	if(DC_start_mpi_entity(argv[1],all)) {
		DC_get_clients_communicator(&clients);
		MPI_Comm_rank(clients,&rank);
		MPI_Comm_size(clients,&size);
		client_code(rank);
		DC_kill_server();
		DC_finalize();
	}

	MPI_Finalize();
	return 0;
}
