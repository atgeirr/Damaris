#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "Damaris.h"

#define MAX_CYCLES 10


int WIDTH;
int HEIGHT;
int domains; // change the number of domains from .xml file (both from the <domains count=".."> tag and domains parameter)
int size;
int rank;
int local_width;
int local_height;
int GX0;    // ****************************************************
int GX1;    // * The ghost zones for each process will be something like:
int GY0;    // * ghost="GX0:GX1,GY0:GY1". Do not forget to update the related parameters as well.
int GY1;    // ****************************************************
int total_width;
int total_height;
int ghost_value = 666;


void normalWrite(int iteration , int* array) {

    int (*parray)[total_height] =  (int (*)[total_height]) array;

    int offset_width = rank*local_width;
    int offset_height = 0;

    int64_t position_space[2];

    position_space[0] = offset_width;
    position_space[1] = offset_height;

    for(int x = 0; x < total_width; x++)
        for(int y = 0; y < total_height; y++){

            if ((x < GX0) || (y < GX0))
                parray[x][y] = ghost_value;
            else if ((x >= total_width - GX1) || (y >= total_height - GY1))
                parray[x][y] = ghost_value;
            else
                parray[x][y] = rank + iteration;
        }

    damaris_set_position("space",position_space);

    damaris_write("space",parray);
    damaris_end_iteration();
}

void blockWrite(int iteration , int* array){

    int (*parray)[total_height] =  (int (*)[total_height]) array;

    for(int dom=0; dom<domains ; dom++) {
        int offset_width = rank * local_width;
        int offset_height = dom * local_height;

        int64_t position_space[2];

        position_space[0] = offset_width;
        position_space[1] = offset_height;

        for (int x = 0; x < total_width; x++) {
            for (int y = 0; y < total_height; y++) {
                if ((x < GX0) || (y < GX0))
                    parray[x][y] = ghost_value;
                else if ((x >= total_width - GX1) || (y >= total_height - GY1))
                    parray[x][y] = ghost_value;
                else
                    parray[x][y] = rank*10 +dom + iteration;
            }
        }

        damaris_set_block_position("space", dom, position_space);
        damaris_write_block("space", dom, array);
    }

    damaris_end_iteration();
}

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		fprintf(stderr,"Usage: %s <2dmesh.xml>\n",argv[0]);
		exit(0);
	}

	MPI_Init(&argc,&argv);
	
	damaris_initialize(argv[1],MPI_COMM_WORLD);

	int is_client;
	int err = damaris_start(&is_client);
	
	if((err == DAMARIS_OK || err == DAMARIS_NO_SERVER) && is_client) {
	
		MPI_Comm comm;
		damaris_client_comm_get(&comm);

		damaris_parameter_get("WIDTH" , &WIDTH , sizeof(int));
		damaris_parameter_get("HEIGHT" , &HEIGHT , sizeof(int));
		damaris_parameter_get("domains", &domains , sizeof(int));
        damaris_parameter_get("GX0" , &GX0 , sizeof(int));
        damaris_parameter_get("GY0" , &GY0 , sizeof(int));
        damaris_parameter_get("GX1" , &GX1 , sizeof(int));
        damaris_parameter_get("GY1" , &GY1 , sizeof(int));

        MPI_Comm_rank(comm , &rank);
        MPI_Comm_size(comm , &size);

        local_width      = WIDTH/size;
        local_height     = HEIGHT/domains;

        total_width = local_width + GX0 + GX1; // GX:GX
        total_height = local_height + GY0 + GY1; // GY:GY

        int (*space)[total_height] = malloc((total_height)*(total_width)*sizeof(int));

		for(int i=0; i < MAX_CYCLES; i++) {
			double t1 = MPI_Wtime();

            if (domains == 1)
                normalWrite(i , (int*)space);
            else
                blockWrite(i , (int*)space);
		
			MPI_Barrier(comm);

			double t2 = MPI_Wtime();

			if(rank == 0) {
				printf("2dmesh: Iteration %d done in %f seconds\n",i,(t2-t1));
			}
		}

        if (rank == 0)
        {
            printf("\nTotal array for rank 0 is: \n");
            for (int i = 0; i < total_width; i++) {
                for (int j = 0; j < total_height; j++)
                    printf("%3d ", space[i][j]);

                printf("\n");
            }
        }

		damaris_stop();
        free(space);
	}

	damaris_finalize();
	MPI_Finalize();
	return 0;
}
