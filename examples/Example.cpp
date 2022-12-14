#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <mpi.h>

#include "Damaris.h"


int main(int argc, char** argv)
{
	MPI_Init(&argc,&argv);
	
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	
	int NROW, NCOL;


/*	if(size != 3) {
		if(rank == 0) {
			fprintf(stderr,
				"This example is meant for 3 processes\n");
			MPI_Abort(MPI_COMM_WORLD,911);
		}
	}
*/
	 
	// All processes must initialize Damaris with the XML configuration
	damaris_initialize("example.xml",MPI_COMM_WORLD);
	
	damaris_parameter_get("NROW",&NROW,sizeof(int));
	damaris_parameter_get("NCOL",&NCOL,sizeof(int));
	
	if(rank == 0) {
		printf("NROW = %d, NCOL = %d\n",NROW,NCOL);
	}
	
	int is_client;
	// The following call starts the servers. Servers will run inside this
	// function until they are asked to stop by clients. On clients,
	// is_client will be set to 1 (0 on servers).
	int err = damaris_start(&is_client);
	
	// We check that Damaris has been properly started and that this
	// process is a client, before continuing.
	if((err == DAMARIS_OK || err == DAMARIS_NO_SERVER) && is_client) {
		
		printf("Ahaha\n");
		
		// We stop the servers.
		damaris_stop();
	}
	
	// Everybody executes this, servers are restarted.
	err = damaris_start(&is_client);
	
	
	// Again, we check that servers have been started properly and that
	// this process is a client.
	if((err == DAMARIS_OK || err == DAMARIS_NO_SERVER) && is_client) {
		
		int* data = (int*)malloc(NROW*NCOL/2);
		
		// Entering a simulation loop.
		int i;
		for(i=0; i<5; i++) {
			//printf("rank %d iteration %d\n",rank,i);
			
			assert(damaris_write("life/cells", data) == DAMARIS_OK);
			
			// Sends a signal.
			assert(damaris_signal("test_event") == DAMARIS_OK);
			
			// Finishes the current iteration.
			assert(damaris_end_iteration() == DAMARIS_OK);
			
		}
		
		free(data);
		
		for(i=5; i<10; i++) {
			//printf("rank %d iteration %d\n",rank,i);
			
			assert(damaris_alloc("life/cells",(void**)(&data)) == DAMARIS_OK);
			
			assert(damaris_commit("life/cells") == DAMARIS_OK);
			
			// Sends a signal.
			assert(damaris_signal("test_event") == DAMARIS_OK);
			assert(damaris_signal("test_local_event") == DAMARIS_OK);
			if(rank == 0) {
				assert(damaris_signal("test_bcast_event") == DAMARIS_OK);
			}
			assert(damaris_clear("life/cells") == DAMARIS_OK);
			
			// Finishes the current iteration.
			assert(damaris_end_iteration() == DAMARIS_OK);
			
		}
		
		// After the loop, we stop the server.
		damaris_stop();
	}
	
	damaris_finalize();
	
	MPI_Finalize();
	return 0;
}
