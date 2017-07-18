#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "Damaris.h"

#define MAX_CYCLES 10

int LENGTH;


int main(int argc, char** argv)
{
	if(argc != 2)
	{
		fprintf(stderr,"Usage: %s <vector.xml>\n",argv[0]);
		exit(0);
	}

	MPI_Init(&argc,&argv);
	
	damaris_initialize(argv[1],MPI_COMM_WORLD);

	int size, rank;

	int is_client;
	int err = damaris_start(&is_client);
	
	if((err == DAMARIS_OK || err == DAMARIS_NO_SERVER) && is_client) {
	
		MPI_Comm comm;
		damaris_client_comm_get(&comm);

		damaris_parameter_get("LENGTH",&LENGTH,sizeof(int));


        MPI_Comm_rank(comm , &rank);
        MPI_Comm_size(comm , &size);


        int local_length      = LENGTH/size;

        int offset = rank*local_length;

		//int space[local_width];
		float* bar = (float*)malloc(local_length* sizeof(float));


		int x,y;
		int64_t position[1];

        position[0] = offset;


		damaris_set_position("bar",position);


		int i;
		for(i=0; i < MAX_CYCLES; i++) {
			double t1 = MPI_Wtime();

			for(x = 0; x < local_length; x++)
					bar[x] = rank + i;

			damaris_write("bar",bar);
			damaris_end_iteration();
		
			MPI_Barrier(comm);

			double t2 = MPI_Wtime();

			if(rank == 0) {
				printf("Iteration %d done in %f seconds\n",i,(t2-t1));
			}
		}

		damaris_stop();
        free(bar);

    }


	damaris_finalize();
	MPI_Finalize();
	return 0;
}
