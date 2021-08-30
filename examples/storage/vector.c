#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "Damaris.h"

#define MAX_CYCLES 4

int LENGTH;
int domains; // change the domain in .xml file (both in the domain tag and the domains parameter)


int main(int argc, char** argv)
{
    
    int size, rank;
    int is_client;
    int LENGTH = 1024 ;
    
    
    if(argc != 2)
    {
        fprintf(stderr,"Usage: %s <vector.xml>\n",argv[0]);
        exit(0);
    }

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD , &rank);
    if ( rank == 0 ) {
        fprintf(stdout, "INFO: argv[1] : %s", argv[1]);
        fflush(stdout) ;
    }
    
    damaris_initialize(argv[1],MPI_COMM_WORLD);
    int err = damaris_start(&is_client);

    if((err == DAMARIS_OK || err == DAMARIS_NO_SERVER) && is_client) {

        MPI_Comm comm;
        damaris_client_comm_get(&comm);

        int size_in_xml ;
        err = damaris_parameter_get("LENGTH",&LENGTH, sizeof(int));
        if (err != DAMARIS_OK ) {
          fprintf(stderr, "ERROR: Damaris damaris_parameter_set():\nparamater: LENGTH");
        }
        // damaris_parameter_get("LENGTH",&LENGTH,sizeof(int));
        damaris_parameter_get("domains",&domains,sizeof(int));
        

        MPI_Comm_rank(comm , &rank);
        MPI_Comm_size(comm , &size);

        damaris_parameter_set("size",&size,sizeof(int));

        int i,x,y,z;
        int64_t position[1];


        for(i=0; i < MAX_CYCLES; i++) {
            double t1 = MPI_Wtime();
            if (LENGTH/size/domains > 1){
                int local_length      = LENGTH/size;  // progressively make the data smaller
                int process_offset    = rank*local_length;
                int block_offset;


                float* bar = (float*)malloc(local_length* sizeof(float));

                for(z = 0; z < local_length; z++)
                    bar[z] = -1.0 ;


                err = damaris_parameter_set("LENGTH",&LENGTH, sizeof(int));
                if (err != DAMARIS_OK ) {
                  fprintf(stderr, "ERROR: Damaris damaris_parameter_set():\nparamater: LENGTH");
                }


                if (domains == 1){
                    for(z = 0; z < local_length; z++)
                        bar[z] = rank;

                    position[0] = process_offset;
                    // damaris_set_position() does nothing for the output of the data to HDF5 in filePerProcess mode
                    // however, it is important in the Collective mode as it sets the metadata of where the data is positioned
                    damaris_set_position("bar",position);
                    damaris_write("bar",bar);
                } else {
                    for(y=0; y<domains ; y++){
                        for(z = 0; z < local_length/domains; z++)
                            bar[z] = rank*10 + y;

                        block_offset = y*(local_length/domains);
                        position[0] = process_offset + block_offset;
                        // damaris_set_block_position() does nothing for the output of the data to HDF5 in filePerProcess mode
                        // however, it is important in the Collective mode as it sets the metadata of where the data is positioned
                        damaris_set_block_position("bar" , y , position);
                        damaris_write_block("bar" , y , bar );
                    }
                }
                LENGTH /= 2 ;
                damaris_end_iteration();

                MPI_Barrier(comm);
                free(bar);
            } else {
                fprintf(stderr, "ERROR: Vector: Possibly running example with too many processes or domains");
            }
            double t2 = MPI_Wtime();

            if(rank == 0) {
                printf("Vector: Iteration %d done in %f seconds\n",i,(t2-t1));
            }
        }

        damaris_stop();
        // free(bar);
    }

    damaris_finalize();
    MPI_Finalize();
    return 0;
}
