#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "Damaris.h"

#define MAX_CYCLES 10

int WIDTH;
int HEIGHT;
int DEPTH;


int main(int argc, char** argv)
{
	if(argc != 2)
	{
		fprintf(stderr,"Usage: %s <3dmesh.xml>\n",argv[0]);
		exit(0);
	}

	MPI_Init(&argc , &argv);
	
	damaris_initialize(argv[1],MPI_COMM_WORLD);

	int size, rank;

	int is_client;
	int err = damaris_start(&is_client);
	
	if((err == DAMARIS_OK || err == DAMARIS_NO_SERVER) && is_client) {
	
		MPI_Comm comm;
		damaris_client_comm_get(&comm);

		damaris_parameter_get("WIDTH" , &WIDTH , sizeof(int));
		damaris_parameter_get("HEIGHT" , &HEIGHT , sizeof(int));
        damaris_parameter_get("DEPTH" , &DEPTH , sizeof(int));


        MPI_Comm_rank(comm , &rank);
        MPI_Comm_size(comm , &size);

        int local_width      = WIDTH/size;
		int local_height     = HEIGHT;
        int local_depth      = DEPTH;

        int offset_x = rank*local_width;
        int offset_y = 0;
        int offset_z = 0;


        int cube[local_width][local_height][local_depth];


		int x,y,z;
		int64_t position_cube[3];

        position_cube[0] = rank*local_width;
        position_cube[1] = 0;
        position_cube[2] = 0;

		damaris_set_position("cube",position_cube);


		int i;
		for(i=0; i < MAX_CYCLES; i++) {
			double t1 = MPI_Wtime();

			for(x = 0; x < local_width; x++)
				for(y = 0; y < local_height; y++)
                    for (z=0; z < local_depth ; z++)
					    cube[x][y][z] = rank + i;


			damaris_write("cube" , cube);
			damaris_end_iteration();
		
			MPI_Barrier(comm);

			double t2 = MPI_Wtime();

            if(rank == 0) {
                printf("Iteration %d done in %f seconds\n",i,(t2-t1));
            }
        }

		damaris_stop();
	}

	damaris_finalize();
	MPI_Finalize();
	return 0;
}
