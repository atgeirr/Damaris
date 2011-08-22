#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "server/Server.h"
#include "client/Client.h"

#define CORES_PER_NODE 4

void client_main_loop(MPI_Comm c, int rank);

int main (int argc, char** argv)
{
	int rank, size, is_client;
	MPI_Comm comm;
	
	/* MPI initialization */
	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);

	/* Splitting communicator between servers and clients */
	is_client = (rank % CORES_PER_NODE) != 0;
	MPI_Comm_split(MPI_COMM_WORLD,is_client,rank,&comm);

	/* Starting servers and simulations code */
	if(is_client) {
		/* Entering simulation's main loop */
		client_main_loop(comm, rank);
	} else {
		/* Starting server */
		DC_server("config.xml",rank);
	}

	MPI_Finalize();
  	return 0;
}

void client_main_loop(MPI_Comm c, int rank)
{
	sleep(5);
	DC_initialize("config.xml",rank);
	
	sleep(5);
	DC_kill_server();

	DC_finalize();
}
