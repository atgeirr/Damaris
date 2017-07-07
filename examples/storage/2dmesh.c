#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "Damaris.h"

#define MAX_CYCLES 10

int WIDTH;
int HEIGHT;




int main(int argc, char** argv)
{
	if(argc != 2)
	{
		fprintf(stderr,"Usage: %s <2dmesh.xml>\n",argv[0]);
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

		damaris_parameter_get("WIDTH",&WIDTH,sizeof(int));
		damaris_parameter_get("HEIGHT",&HEIGHT,sizeof(int));


        MPI_Comm_rank(comm , &rank);
        MPI_Comm_size(comm , &size);

        printf("size is: %d rank is: %d \n",size,rank);


        int local_width      = WIDTH/size;
		int local_height     = HEIGHT;

		int offset_y = 0;
        int offset_x = rank*local_width;

		int space[local_width][local_height];


		int x,y;
		int64_t position_space[2];

        position_space[0] = rank*local_width;
        position_space[1] = 0;

        /*
        // tile paritioning
        switch (rank) {
            case 0:
                position_space[0] = 0;
                position_space[1] = 0;
                break;
            case 1:
                position_space[0] = 0;
                position_space[1] = 8;
                break;
            case 2:
                position_space[0] = 8;
                position_space[1] = 0;
                break;
            case 3:
                position_space[0] = 8;
                position_space[1] = 8;
                break;
        }*/

		damaris_set_position("space",position_space);


		int i;
		for(i=0; i < MAX_CYCLES; i++) {
			double t1 = MPI_Wtime();

			for(x = 0; x < local_width; x++)
				for(y = 0; y < local_height; y++)
					space[x][y] = rank+i;


			damaris_write("space",space);
			damaris_end_iteration();
		
			MPI_Barrier(comm);

			double t2 = MPI_Wtime();
		}

		damaris_stop();
	}

	damaris_finalize();
	MPI_Finalize();
	return 0;
}
