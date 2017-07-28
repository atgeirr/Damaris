#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "Damaris.h"

#define MAX_CYCLES 3

int LENGTH;
int domains; // change the domain in .xml file (both in the domain tag and the domains parameter)


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
        damaris_parameter_get("domains",&domains,sizeof(int));

        MPI_Comm_rank(comm , &rank);
        MPI_Comm_size(comm , &size);

        int local_length      = LENGTH/size;
        int process_offset = rank*local_length;
        int block_offset;

		float* bar = (float*)malloc(local_length* sizeof(float));

		int x,y;
		int64_t position[1];

		for(int i=0; i < MAX_CYCLES; i++) {
			double t1 = MPI_Wtime();

            if (domains == 1){
                for(int z = 0; z < local_length; z++)
                    bar[z] = rank;

                position[0] = process_offset;
                damaris_set_position("bar",position);
                damaris_write("bar",bar);
            } else {
                for(int y=0; y<domains ; y++){
                    for(int z = 0; z < local_length; z++)
                        bar[z] = rank*10 + y;

                    block_offset = y*(local_length/domains);
                    position[0] = process_offset + block_offset;
                    damaris_set_block_position("bar" , y , position);
                    damaris_write_block("bar" , y , bar );
                }
            }

            damaris_end_iteration();

            MPI_Barrier(comm);

			double t2 = MPI_Wtime();

			if(rank == 0) {
				printf("Vector: Iteration %d done in %f seconds\n",i,(t2-t1));
			}
		}

		damaris_stop();
        free(bar);

    }

	damaris_finalize();
	MPI_Finalize();
	return 0;
}
