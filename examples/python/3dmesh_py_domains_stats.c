#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include <mpi.h>
#include "Damaris.h"

/**
    A Damaris example of using Python integration with Dask distributed. This version sets up a dataset
    which has multiple domains (or blocks) written per iteration, and is to be run multiple times and the
    values written are summarised i.e. a running average and standard deviation is computed through use of
    the damaris4py DaskStats Python class.
    
    To run this example, a Dask scheduler needs to be spun up:

      dask-scheduler --scheduler-file "/home/user/dask_file.json" &

    The --scheduler-file argument must match what is in the Damaris XML file <pyscript> tag.

    Then run the simulation multiple times:

    for i in `seq 1 10` do
        sbatch slurm_launcher.sh $i  # Use the correct submission command for your cluster's resource manager
    done
    
    Inside the launch script slurm_launcher.sh will be something like the following:
    
      #SBATCH #OAR #PBS etc.... 
      # MY_VAL is the value to be added to the 3D array on each iteration
      MY_VAL=$1  
      # Assumes 4 Damaris clients and 2 Damaris server cores as per the xml file
      mpirun --oversubscribe  -np 6 ./3dmesh_dask_stats 3dmesh_dask_stats.xml -i 10 -v $MY_VAL -d 4 -s 5
    
    
    N.B. Set the global mesh size values WIDTH, HEIGHT and DEPTH using the XML input file.
         Set the name of the Python script to run via the <pyscript ... file="3dmesh_dask_stats.py" ...> tag

    The simulation code (via Damaris C++ pyscript class) will create the Dask workers (one per Damaris server core) 
    and have them connect to the Dask scheduler. The simulation code will remove the workers at the end of program
    execution, unless  keep-workers="yes" is specified in 3dmesh_dask.xml <pyscript> tag
*/

void print_usage(char* exename) {
    fprintf(stderr,"Usage: %s <3dmesh_dask.xml> [-i I] [-d D] [-r] [-s S]\n",exename);
    fprintf(stderr,"-i  I    I is the number of iterations of simulation to run\n");
    fprintf(stderr,"-v  V    V is the value to add to each array element (default = 5)\n");
    fprintf(stderr,"-d  D    D is the number of domains to split data into (must divide into Width perfectly)\n");
    fprintf(stderr,"-s  S    S is integer time to sleep in seconds between iterations\n");
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

   // argument 1 is the Damaris XML file name
   damaris_initialize(argv[1],MPI_COMM_WORLD);
   
   int verbose = 0;
   int rank_only = 1;
   int current_arg = 2 ; 
   int myval = 5 ;  
   int time = 1 ;
   int domains = 1 ;
   MAX_CYCLES = 5;  // default number of iterations to run
   while (current_arg < argc ) 
   {
     if (strcmp(argv[current_arg],"-v") == 0) {
         rank_only = 1 ;
         current_arg++;
         myval = atoi(argv[current_arg]);
     } else if (strcmp(argv[current_arg],"-i") == 0) {
         current_arg++;
         MAX_CYCLES = atoi(argv[current_arg]);
     } else if (strcmp(argv[current_arg],"-d") == 0) {
         current_arg++;
         domains = atoi(argv[current_arg]);
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

        // These are the desired sizes of the array. We get the dimensions from the XML input file.
        // We could have, conversely, got the values from the command line (or somewhere else) and
        // then used damaris_paramater_set() to tell Damaris how big the arrays will be.
        damaris_parameter_get("WIDTH" , &WIDTH , sizeof(int));
        damaris_parameter_get("HEIGHT", &HEIGHT, sizeof(int));
        damaris_parameter_get("DEPTH" , &DEPTH , sizeof(int));
        
        // This is only be available on the first iteration if time-varing=false
        // damaris_write("last_iter" , &MAX_CYCLES); // we could set it now and publish it in Dask to keep it around as needed.
        
        MPI_Comm_rank(comm , &rank_client);
        MPI_Comm_size(comm , &size_client);

        fprintf(stdout,"Input paramaters found: v=%d r=%d (0 is not found)\n",verbose, rank_only) ;

        // Dynamically update the size used in the Damaris layout configuration
        damaris_parameter_set("size" , &size_client , sizeof(int)) ;
        
        // Dynamically update the number of domains being used in the Damaris layout configuration
        damaris_parameter_set("blocks" , &domains , sizeof(int)) ;
        

        int64_t position_cube[3];
        int local_width, local_height, local_depth ;
        // rank_start is the value a ranks data will start at and then be incremented 
        // at each position onward (if -r not present)
        int rank_start = 0 ; 

     
        if (size_client > DEPTH) {
         printf("ERROR: MPI process count (size=%2d) is greater than the blocked index (DEPTH=%2d)\t", size_client, DEPTH  );
         exit(-1);
        }
        if (WIDTH % domains != 0) {
         int temp_int  =WIDTH % domains ;
         printf("ERROR: WIDTH mod domains must divide perfectly %d mod %d = %d\t", WIDTH, domains, temp_int  );
         exit(-1);
        }
        
        // used with: <layout name="cells_whd_wf" type="int" dimensions="DEPTH/size,HEIGHT,WIDTH/domains" 
        // global="DEPTH,HEIGHT,WIDTH" />
        // These sizes do not change 
        local_width      = WIDTH / domains ;         
        local_height     = HEIGHT ;
        local_depth      = DEPTH / size_client ;
        int i, d, h, w ;
        for( i=0; i < MAX_CYCLES; i++) 
        {   
            double array_sum = 0.0 ;
            long int reduction_result = 0 ;
            double t1 = MPI_Wtime() ; 
            
            damaris_write("last_iter" , &MAX_CYCLES) ; // The Damaris XML variable has time-varing=true 
            
            // We are writing multiple blocks of data per Damaris client rank.
            // This requires that we use damaris_set_block_position() and damaris_write_block() API calls
            // instead of damaris_set_position() and damaris_write()
            for (int block = 0 ; block < domains ; block++)
            {
                position_cube[0] = rank_client*local_depth ;
                position_cube[1] = 0 ;
                position_cube[2] = block * local_width ;

                rank_start = rank_client * local_width * local_height ;

                // allocate the local data array
                int cube[local_depth][local_height][local_width] ;

                // set the appropriate position for the current rank
                damaris_set_block_position("cube_i", block, position_cube) ;
 
                int sequence ;
                sequence =  i ;  // this is the start of the sequence for this iteration
                
                // print sumation values to screen
                long int sumdata = 0 ;
                for ( d = 0; d < local_depth; d++){
                    for ( h = 0; h < local_height; h++){
                        for ( w = 0 ; w < local_width; w++) {
                           cube[d][h][w] = (int) rank_start +  myval ;
                           sumdata += cube[d][h][w] ;
                           if (rank_only==0) sequence++;
                        }
                    }
                } 
                  
                // Each MPI process sends its rank to reduction, root MPI process collects the result
                MPI_Reduce(&sumdata, &reduction_result, 1, MPI_LONG, MPI_SUM, 0, comm) ;                                    
                array_sum += reduction_result ;

                damaris_write_block("cube_i" , block, cube) ;

            }  // end of loop over blocks
            sleep(time) ;
            MPI_Barrier(comm) ;
            double t2 = MPI_Wtime() ;
            if(rank_client == 0) {
                printf("Iteration %d done in %f seconds\n",i,(t2-t1)) ;
                fflush(stdin); 
            }
            if (rank_client == 0)
                printf("Iteration %d Rank %d Sum = %f\n", i, rank_client, array_sum ) ;
            damaris_write("array_sum" , &array_sum) ;  // Used to confirm the summation value found in Dask        
           
            damaris_end_iteration() ;
        }  // end of for loop over MAX_CYCLES

        damaris_stop() ;
   }

   damaris_finalize() ;
   MPI_Finalize() ;
   return 0;
}
