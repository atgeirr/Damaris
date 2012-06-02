/******************************************************************************
 * Game of Life demo example for in-situ
 * Written by Jean M Favre, Swiss National Supercomputer Center
 * Wed May 26 11:42:47 CEST 2010
 * inspired by the examples from VisIt, written by Brad Whitlock
 * runs in parallel
 * Some initial conditions were taken from the Game of life Wiki page on Wikipedia
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <mpi.h>
#include "Damaris.h"

/******************************************************************************
 * Life data and functions
 ******************************************************************************/

/* The number of cells (or zones) displayed on screen is NN * NN*/
/*#define NN 48*96*2 */
#define NROW 96
#define NCOL 32

typedef enum {RANDOM = 0, GLIDER=1, ACORN=2, DIEHARD=3} BCtype;

typedef struct
{
    int    Nrows;     /* size of true_life array */
    int    Ncolumns;  /* size of true_life array */
    float *rmesh_x;
    float *rmesh_y;
    int    rmesh_dims[3];
    int    rmesh_ndims;
    int   *true_life;     /* true_life is the NNxNN array exposed to VisIt*/
    int   *working_life;  /* working_life is the (NN+2)x(NN+2) array used to compute it.*/
} life_data;

void
life_data_ctor(int par_size, life_data *life)
{
    life->Nrows = NROW/par_size;
    life->Ncolumns = NCOL;
    life->rmesh_x = NULL;
    life->rmesh_y = NULL;
    life->rmesh_dims[0] = NROW+1; /* shall be redefined later after Parallel init*/
    life->rmesh_dims[1] = NCOL+1;
    life->rmesh_dims[2] = 1;
    life->rmesh_ndims = 2;
    life->true_life = NULL;
    life->working_life = NULL;
}

void
life_data_dtor(life_data *life)
{
    if(life->rmesh_x != NULL)
    {
        free(life->rmesh_x);
        life->rmesh_x = NULL;
    }
    if(life->rmesh_y != NULL)
    {
        free(life->rmesh_y);
        life->rmesh_y = NULL;
    }
    if(life->true_life != NULL)
    {
        free(life->true_life);
        life->true_life = NULL;
    }
    if(life->working_life != NULL)
    {
        free(life->working_life);
        life->working_life = NULL;
    }
}

/* A 2D Rectilinear mesh of size NNxNN is built and will be visualized
   A second array used during the computation, and including ghost-cells is also required.
*/
void
life_data_allocate(life_data *life, int par_rank, int par_size)
{
    int i;
    float offset;

    life->rmesh_x = (float*)malloc(sizeof(float) * (life->Ncolumns + 1));
    life->rmesh_y = (float*)malloc(sizeof(float) * (life->Nrows + 1));

    life->rmesh_dims[0] = life->Ncolumns + 1;
    life->rmesh_dims[1] = life->Nrows + 1;
    life->rmesh_dims[2] = 1;

    for(i=0; i<= life->Ncolumns; i++)
    {
        life->rmesh_x[i] = (float)(i);// /life->Ncolumns;
    }

    offset = (float)(par_rank)*(life->Nrows);
    for(i=0; i<=life->Nrows; i++)
    {
        life->rmesh_y[i] = offset + (float)(i); // /life->Ncolumns;
    }

    /* 2D array of data items exposed as solution */
    life->true_life = (int*)malloc(sizeof(int) * life->Nrows * life->Ncolumns);

    /* 2D array of data items with 2 extra zones for ghost-cells */
    life->working_life = (int*)malloc(sizeof(int) * (life->Nrows+2) * (life->Ncolumns+2));
}

void
life_data_simulate(life_data *life, int par_rank, int par_size)
{
    int nsum, i, j, JPNN, JNN, JMNN, source, dest;
    int *true_life = NULL, *working_life = NULL;
    MPI_Status status;
    MPI_Request request;

    /* Alias for readability */
    working_life = life->working_life;
    true_life = life->true_life;

    /* copy true_life to working_life to prepare for new iteration. Valid for 
       all MPI processes.
     */
    for(j=1; j <= life->Nrows; j++)
    {
        for(i=1; i <= life->Ncolumns; i++)
        {
            working_life[j*(life->Ncolumns + 2) + i] = true_life[(j-1) * life->Ncolumns +(i-1)];
        }
    }

    /* The Parallel allocation splits the whole mesh into horizontal stripes.
       All procs manage the full set of columns while treating only a subet 
       of rows initialize ghost cells from the working_life array. Valid for 
       all MPI processes because each rank has a full set of columns
     */
    for(j=1; j<= life->Nrows; j++)
    {
        /* left-most column at index i=0 */
        working_life[j*(life->Ncolumns+2) + 0]    = true_life[(j-1)*life->Ncolumns + life->Ncolumns-1];
        /* right-most column at index i=life->Ncolumns+1  */
        working_life[j*(life->Ncolumns+2) + life->Ncolumns+1] = true_life[(j-1)*life->Ncolumns + 0];
    }

    /* This next part depends on the MPI rank
       bottom row at index j=0 at rank J gets initialized from the top row from rank J-1
       top row at index j=(life->Nrows+1) at rank J gets initialized from bottom row from rank J+1
       Using the integer modulo op allows us to wrap around the number of procs
     */
	if(par_size > 1) {
		source = (par_size + par_rank - 1) % par_size;
		dest = (par_rank+1) % par_size;
		MPI_Irecv(&working_life[1], life->Ncolumns, MPI_INT, dest, 1000, MPI_COMM_WORLD, &request);
		MPI_Send(&true_life[(life->Nrows-1)*life->Ncolumns], life->Ncolumns, MPI_INT, source,  1000, MPI_COMM_WORLD);
		MPI_Wait( &request, &status);

		MPI_Irecv(&working_life[(life->Nrows+1)*(life->Ncolumns+2) + 1], life->Ncolumns, MPI_INT, dest, 1001, MPI_COMM_WORLD, &request);
		MPI_Send(&true_life[0], life->Ncolumns, MPI_INT, source,  1001, MPI_COMM_WORLD);
		MPI_Wait( &request, &status);
	} else {
    	memcpy(&working_life[1], &true_life[(life->Nrows-1)*life->Ncolumns], life->Ncolumns * sizeof(int));
    	memcpy(&working_life[(life->Nrows+1)*(life->Ncolumns+2) + 1], &true_life[0], life->Ncolumns * sizeof(int));
	}
    /* corners 
       rank 0, lower left corner    <= rank Max upper right corner
       rank 0, lower right corner   <= rank Max, upper left corner
       rank max, upper left corner  <= rank 0, lower right corner
       rank max, upper right corner <= rank 0, lower left corner
    */
	if(par_size > 1) {
		if(par_rank == 0)
    	{
        	source = par_size -1;
        	MPI_Irecv(&working_life[(life->Nrows+1)*(life->Ncolumns+2) + 0], 1, MPI_INT, source, 1002, MPI_COMM_WORLD, &request);
        	MPI_Send(&true_life[life->Ncolumns-1], 1, MPI_INT, source,  1004, MPI_COMM_WORLD);
        	MPI_Wait( &request, &status);
  	
			MPI_Irecv(&working_life[life->Ncolumns+1], 1, MPI_INT, source, 1003, MPI_COMM_WORLD, &request);
			MPI_Send(&true_life[0], 1, MPI_INT, source,  1005, MPI_COMM_WORLD);
			MPI_Wait( &request, &status);
		}
		if(par_rank == (par_size -1) && par_size > 1)
		{
			source = 0;
			MPI_Irecv(&working_life[0], 1, MPI_INT, source, 1004, MPI_COMM_WORLD, &request);
			MPI_Send(&true_life[life->Ncolumns*(life->Nrows-1) + life->Ncolumns-1], 1, MPI_INT, source,  1002, MPI_COMM_WORLD);
			MPI_Wait( &request, &status);

			MPI_Irecv(&working_life[(life->Nrows+1)*(life->Ncolumns+2) + life->Ncolumns+1], 1, MPI_INT, source, 1005, MPI_COMM_WORLD, &request);
			MPI_Send(&true_life[life->Ncolumns*(life->Nrows-1) + 0], 1, MPI_INT, source,  1003, MPI_COMM_WORLD);
			MPI_Wait( &request, &status);
		}
	} else {
		working_life[0              ] = true_life[life->Ncolumns*(life->Nrows-1) + life->Ncolumns-1];     /* copy the upper right corner */
		working_life[life->Ncolumns+1] = true_life[life->Ncolumns*(life->Nrows-1) + 0];                   /* copy the uppper left corner */
		working_life[(life->Nrows+1)*(life->Ncolumns+2) + 0              ] = true_life[life->Ncolumns-1]; /* copy the bottom right corner */
		working_life[(life->Nrows+1)*(life->Ncolumns+2) + life->Ncolumns+1] = true_life[0];               /* copy the bottom left corner */
	}

    /* calculate true_life(i-1, j-1) using working_life */
    for(j=1; j<= life->Nrows; j++)
    {
        JPNN = (j+1)*(life->Ncolumns+2);
        JNN  =  j   *(life->Ncolumns+2);
        JMNN = (j-1)*(life->Ncolumns+2);
        for(i=1; i <= life->Ncolumns; i++)
        {
            nsum =  working_life[JPNN + i - 1] + working_life[JPNN + i] + working_life[JPNN + i + 1]
                  + working_life[ JNN + i - 1]                          + working_life[ JNN + i + 1]
                  + working_life[JMNN + i - 1] + working_life[JMNN + i] + working_life[JMNN + i + 1];
    
            switch(nsum)
            {
            case 3:
                true_life[(j-1)*life->Ncolumns + (i-1)] = 1;
                break;
            case 2:
                /* keep its current life status */
                true_life[(j-1)*life->Ncolumns + (i-1)] = working_life[JNN + i];
                break;
            default:
                /* dies by over-crowding or under-population */
                true_life[(j-1)*life->Ncolumns + (i-1)] = 0;
            }
        }
    }
}

void
life_data_ResetInitialConditions(life_data *life, BCtype bc)
{
    /* in all cases, we center the figure in the middle of the grid */
    int i, j;

    switch(bc)
    {
    case RANDOM:
        for(j=0; j < life->Nrows; j++)
        {
            for(i=0; i < life->Ncolumns; i++)
            {
                float x = rand()/((float)RAND_MAX + 1);
                if(x<0.5)
                    life->true_life[j*life->Ncolumns + i] = 0;
                else
                    life->true_life[j*life->Ncolumns + i] = 1;
            }
        }
        break;

    case GLIDER:
        for(j=0; j < life->Nrows; j++)
        {
            for(i=0; i< life->Ncolumns; i++)
            {
               if(
                 ( (j == life->Nrows/2)     && (i >= (life->Ncolumns-1)/2 && i <= (life->Ncolumns-1)/2 + 2)) || /* row NN/2 */
                 ( (j == (life->Nrows/2)+1) && (i == (life->Ncolumns-1)/2 + 2)) ||                        /* next row up  */
                 ( (j == (life->Nrows/2)+2) && (i == (life->Ncolumns-1)/2 + 1))                          /* next row up */
                 )
                    life->true_life[j*life->Ncolumns + i] = 1;
               else
                    life->true_life[j*life->Ncolumns + i] = 0;
            }
        }
        break;

    case ACORN:
        for(j=0; j < life->Nrows; j++)
        {
            for(i=0; i< life->Ncolumns; i++)
            {
                if(
                  ( (j == life->Nrows/2)     && (i >= (life->Ncolumns-1)/2 + 0 && i <= (life->Ncolumns-1)/2 + 1)) ||
                  ( (j == life->Nrows/2)     && (i >= (life->Ncolumns-1)/2 + 4 && i <= (life->Ncolumns-1)/2 + 6)) ||
                  ( (j == (life->Nrows/2)+1) && (i == (life->Ncolumns-1)/2 + 3)) ||
                  ( (j == (life->Nrows/2)+2) &&  i == (life->Ncolumns-1)/2 + 1)
                  )
                    life->true_life[j*life->Ncolumns + i] = 1;
                else
                    life->true_life[j*life->Ncolumns + i] = 0;
            }
        }
        break;

    case DIEHARD:
        for(j=0; j < life->Nrows; j++)
        {
            for(i=0; i< life->Ncolumns; i++)
            {
                if(
                  ( (j == life->Nrows/2)     && (i == (life->Ncolumns-1)/2 + 1)) ||
                  ( (j == life->Nrows/2)     && (i >= (life->Ncolumns-1)/2 + 5 && i <= (life->Ncolumns-1)/2 + 7)) ||
                  ( (j == (life->Nrows/2)+1)  && (i >= (life->Ncolumns-1)/2 + 0 && i <= (life->Ncolumns-1)/2 + 1)) ||
                  ( (j == (life->Nrows/2)+2) &&  i == (life->Ncolumns-1)/2 + 6)
                  )
                    life->true_life[j*life->Ncolumns + i] = 1;
                else
                    life->true_life[j*life->Ncolumns + i] = 0;
            }
        }
        break;
    } /* end of switch */
}

/******************************************************************************
 * Simulation data and functions
 ******************************************************************************/

typedef struct
{
    int       cycle;
    double    time;
    int       done;
    int       par_rank;
    int       par_size;

    life_data life;
} simulation_data;


void
simulation_data_ctor(simulation_data *sim)
{
    sim->cycle = -1;
    sim->time = 0.;
    sim->done = 0;
    sim->par_rank = 0;
    sim->par_size = 1;

    life_data_ctor(sim->par_size, &sim->life);
}

void
simulation_data_dtor(simulation_data *sim)
{
    life_data_dtor(&sim->life);
}

/******************************************************************************
 ******************************************************************************
 ***
 *** EVENT PROCESSING FUNCTIONS
 ***
 ******************************************************************************
 *****************************************************************************/

void
simulate_one_timestep(simulation_data *sim)
{
    /* Simulate the current round of life. */
    life_data_simulate(&sim->life, sim->par_rank, sim->par_size);

	
	++sim->cycle;
    sim->time += 1;

}

void exposeDataToDamaris(simulation_data* sim);

/******************************************************************************
 *
 * Purpose: This is the main event loop function.
 *
 * Programmer: Jean Favre
 * Date:       Fri Feb  6 14:29:36 PST 2009
 *
 * Modifications:
 *
 *****************************************************************************/

void mainloop(simulation_data *sim)
{
	do {
			simulate_one_timestep(sim);
			exposeDataToDamaris(sim);
			sleep(5);
    } while(!sim->done);
}

void exposeDataToDamaris(simulation_data* sim) {
	static int firstCall = 0;
	
	if(firstCall == 0) {
		DC_write("coordinates/x2d",sim->cycle,sim->life.rmesh_x);
		DC_write("coordinates/y2d",sim->cycle,sim->life.rmesh_y);
		firstCall = 1;
	}

	DC_write("life/cells",sim->cycle,sim->life.true_life);
	if(sim->cycle >= 2) {
		DC_signal("clean",sim->cycle-2);
	}

#ifdef PARALLEL
	MPI_Barrier(MPI_COMM_WORLD);
	if(sim->par_rank == 0) 
#endif
	{
			DC_end_iteration(sim->cycle);
	}
}

/******************************************************************************
 *
 * Purpose: This is the main function for the program.
 *
 * Programmer: Jean Favre
 * Date:       Wed May 26 11:42:47 CEST 2010
 *
 * Input Arguments:
 *   argc : The number of command line arguments.
 *   argv : The command line arguments.
 *
 * Modifications:
 *
 *****************************************************************************/

int main(int argc, char **argv)
{
    simulation_data sim;
    simulation_data_ctor(&sim);

	if(argc != 2)
	{
		fprintf(stderr,"Usage: %s life.xml\n",argv[0]);
		exit(1);
	}

    /* Initialize MPI */
    MPI_Init(&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &sim.par_rank);
    MPI_Comm_size (MPI_COMM_WORLD, &sim.par_size);

    /* Adjust the life partitioning */
    sim.life.Nrows = NROW/sim.par_size; /* assume they divide evenly */
    if((float)(NROW)/sim.par_size - NROW/sim.par_size > 0.0)
    {
        fprintf(stderr,"The total number of rows does not divide evenly by the number of MPI tasks. Resubmit\n");
        exit(1);
    }

	DC_initialize(argv[1],sim.par_rank);

    life_data_allocate(&sim.life, sim.par_rank, sim.par_size);
    life_data_ResetInitialConditions(&sim.life, RANDOM);

    mainloop(&sim);

    simulation_data_dtor(&sim);

    MPI_Finalize();
    return 0;
}
