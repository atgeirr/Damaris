#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include <mpi.h>
#include "Damaris.h"

//#define MAX_CYCLES 3

void print_usage(char* exename) {
   
      fprintf(stderr,"Usage: %s <3dmesh_py.xml> [-v] [-r] [-s X]\n",exename);
      fprintf(stderr,"-v  <X>    X = 0 default, do not print arrays\n");
      fprintf(stderr,"           X = 1 Verbose mode, prints arrays\n");
      fprintf(stderr,"           X = 2 Verbose mode, prints summation of arrays\n");
      fprintf(stderr,"-r         Array values set as rank of process\n");
      fprintf(stderr,"-s  <Y>    Y is integer time to sleep in sconds between iterations\n");
      fprintf(stderr,"-i  <I>    I is the number of iterations of simulation to run\n");
}

int WIDTH;
int HEIGHT;
int DEPTH;
int MAX_CYCLES ;


int main(int argc, char** argv)
{
   if(argc < 2)
   {
      print_usage(argv[0]) ;
      exit(0);
   }

   MPI_Init(&argc , &argv);

   damaris_initialize(argv[1],MPI_COMM_WORLD);
   
  int verbose = 0;
  int rank_only = 0;
  int current_arg = 2 ;  
  int time = 1 ;
  MAX_CYCLES = 5;  // default number of iterations to run
  while (current_arg < argc ) 
  {
    if (strcmp(argv[current_arg],"-v") == 0) {
        current_arg++;
        verbose = atoi(argv[current_arg]);
    }
    else if (strcmp(argv[current_arg],"-r") == 0)
      rank_only = 1 ;
    else if (strcmp(argv[current_arg],"-s") == 0) {
        current_arg++;
        time = atoi(argv[current_arg]);
    } else if (strcmp(argv[current_arg],"-i") == 0) {
        current_arg++;
        MAX_CYCLES = atoi(argv[current_arg]);
    } else if (strcmp(argv[current_arg],"-h") == 0) {
        print_usage(argv[0]) ;
        exit(0);
    }
    
      current_arg++;
  }

  

   int size_client, rank_client, whd_layout;

   int is_client;
   int err = damaris_start(&is_client);

   // if((err == DAMARIS_OK || err == DAMARIS_NO_SERVER) && is_client) {
   if(err == DAMARIS_OK && is_client) {

        MPI_Comm comm;
        damaris_client_comm_get(&comm);

        damaris_parameter_get("WIDTH" , &WIDTH , sizeof(int));
        damaris_parameter_get("HEIGHT", &HEIGHT, sizeof(int));
        damaris_parameter_get("DEPTH" , &DEPTH , sizeof(int));
        damaris_parameter_get("whd_layout" , &whd_layout , sizeof(int));

        MPI_Comm_rank(comm , &rank_client);
        MPI_Comm_size(comm , &size_client);

        fprintf(stdout,"Input paramaters found: v=%d r=%d (0 is not found)\n",verbose, rank_only);

        // Dynamically update the size used in the Damaris layout configuration
        damaris_parameter_set("size" , &size_client , sizeof(int));

        int64_t position_cube[3];
        int local_width, local_height, local_depth  ;
        int rank_start = 0 ;

     
        if (size_client > WIDTH) {
         printf("ERROR: MPI process count (size=%2d) is greater than the blocked index (WIDTH=%2d)\t", size_client, WIDTH  );
         exit(-1);
        }
        // used with: <layout name="cells_whd_wf" type="int" dimensions="WIDTH/size,HEIGHT,DEPTH" global="WIDTH,HEIGHT,DEPTH" />
        local_width      = WIDTH ; // WIDTH/size;
        local_height     = HEIGHT;
        local_depth      = DEPTH/size_client;

        position_cube[0] = rank_client*local_depth;
        position_cube[1] = 0;
        position_cube[2] = 0;

        rank_start = rank_client * local_width * local_height ;

        // allocate the local data array
        int cube[local_depth][local_height][local_width];
        float cube_f[local_depth][local_height][local_width];

        // set the appropriate position for the current rank
        damaris_set_position("cube_i",position_cube);
        damaris_set_position("cube_f",position_cube);

        // do not do this here as we will not get an updated value (if time-varying="true" )
        // damaris_write("last_iter" , &MAX_CYCLES);

        int i, d, h, w ;
        for( i=0; i < MAX_CYCLES; i++) {
            double t1 = MPI_Wtime();         
            int sequence ;
            sequence =  i ;  // this is the start of the sequence for this iteration
            if (verbose == 0) { // default - do not print values to screen

                for ( d = 0; d < local_depth; d++){
                    for ( h = 0; h < local_height; h++){
                         for ( w = 0; w < local_width; w++) {

                            cube[d][h][w] = (int) sequence + rank_start;
                            cube_f[d][h][w] = (float) sequence + rank_start +0.5f;
                            if (rank_only==0) sequence++;
                         }  
                    }
                }            
            } else  if (verbose == 1) { // print values to screen
            int current_rank = 0 ;
            int sending_rank = 0 ;
             // serialize the print statements
             while ( current_rank < size_client)
             {
                printf("\n"); 
                if (rank_client == current_rank) { // start of serialized section

                  for ( d = 0; d < local_depth; d++){
                    for ( h = 0; h < local_height; h++){
                     for ( w = 0; w < local_width; w++) {
                        cube[d][h][w] = (int) sequence + rank_start;
                        cube_f[d][h][w] = (float) sequence + rank_start +0.5f;
                        printf("%2d\t", cube[d][h][w] );
                        if (rank_only==0) sequence++;
                     }
                     printf("\n");
                    }
                    printf("\n");
                  }
                  fflush(stdin); 
                  sending_rank = current_rank ;
                  current_rank++ ;
                } // end of serialized section
                MPI_Bcast(&current_rank,1, MPI_INT,sending_rank, comm);
                sending_rank = current_rank ;
            } // end of in-order loop over ranks
            } else  if (verbose == 2) { // print sumation values to screen
                int current_rank = 0 ;
                int sending_rank = 0 ;
                int sumdata = 0 ;
                 // serialize the print statements
                 while ( current_rank < size_client)
                 {
                   // printf("\n"); 
                   if (rank_client == current_rank) { // start of serialized section                

                      for ( d = 0; d < local_depth; d++){
                        for ( h = 0; h < local_height; h++){
                         for ( w = 0; w < local_width; w++) {
                            cube[d][h][w] = (int) sequence + rank_start ;
                            cube_f[d][h][w] = (float) sequence + rank_start +0.5f;
                            sumdata += cube[d][h][w] ;
                            if (rank_only==0) sequence++;
                         }
                        }
                      }
                      printf("Iteration %d Rank %d Sum = %8d\n", i, current_rank, sumdata );
                      fflush(stdin); 
                      sending_rank = current_rank ;
                      current_rank++ ;
                   } // end of serialized section
                   MPI_Bcast(&current_rank,1, MPI_INT,sending_rank, comm);
                   sending_rank = current_rank ;
                } // end of in-order loop over ranks
            }
         
         

            damaris_write("cube_i" , cube);
            damaris_write("cube_f" , cube_f);
            damaris_write("last_iter" , &MAX_CYCLES);
            sleep(time);

            damaris_end_iteration();
            MPI_Barrier(comm);

            // sleep(time);

            double t2 = MPI_Wtime();

            if(rank_client == 0) {
            printf("Iteration %d done in %f seconds\n",i,(t2-t1));
            fflush(stdin); 
            }
        }  // end of for loop over MAX_CYCLES

        damaris_stop();
   }

   damaris_finalize();
   MPI_Finalize();
   return 0;
}
