#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <mpi.h>

#include "Damaris.h"

typedef struct
{
    int     cycle;
    double  time;
    int     done;
} simulation_data;

void
simulation_data_ctor(simulation_data *sim)
{
    sim->cycle = 0;
    sim->time = 0.;
    sim->done = 0;
}

void
simulation_data_dtor(simulation_data* sim __attribute__((unused)))
{
}

void simulate_one_timestep(simulation_data *sim)
{
    ++sim->cycle;
    sim->time += 0.0134;
    printf("Simulating time step: cycle=%d, time=%lg\n", sim->cycle, sim->time);
    sleep(1);
}

void exposeDataToDamaris(simulation_data* s);

void mainloop(void)
{
    /* Set up some simulation data. */
    simulation_data sim;
    simulation_data_ctor(&sim);
    int i;
    for(i=0; i<60; i++) {
	simulate_one_timestep(&sim);
	exposeDataToDamaris(&sim);
    }
    simulation_data_dtor(&sim);
}

int main(int argc, char **argv)
{
	MPI_Init(&argc,&argv);
	
	damaris_initialize("blocks-hdf5.xml",MPI_COMM_WORLD);
	
	int is_client;
	int err = damaris_start(&is_client);
	
	if((err == DAMARIS_OK || err == DAMARIS_NO_SERVER) && is_client) {
		mainloop();
		damaris_stop();
	}

	damaris_finalize();
	
	MPI_Finalize();
	return 0;
}

/* Rectilinear mesh */
float rmesh_x[2][4] = {{0., 1., 2.5, 5.},{5., 6., 7.5, 10.}};
float rmesh_y[2][5] = {{0., 2., 2.25, 2.55,  5.},{5., 7., 7.25, 7.55,  10.}};

/* Curvilinear mesh */
float cmesh_x[2][2][3][4] = { 
  {
   {{0.,1.,2.,3.},{0.,1.,2.,3.}, {0.,1.,2.,3.}},
   {{0.,1.,2.,3.},{0.,1.,2.,3.}, {0.,1.,2.,3.}}
  }, {
   {{4.,5.,6.,7.},{4.,5.,6.,7.}, {4.,5.,6.,7.}},
   {{4.,5.,6.,7.},{4.,5.,6.,7.}, {4.,5.,6.,7.}}
  }
};

float cmesh_y[2][2][3][4] = {
  {
   {{0.5,0.,0.,0.5},{1.,1.,1.,1.}, {1.5,2.,2.,1.5}},
   {{0.5,0.,0.,0.5},{1.,1.,1.,1.}, {1.5,2.,2.,1.5}}
  }, {
   {{4.5,4.,4.,4.5},{5.,5.,5.,5.}, {5.5,6.,6.,5.5}},
   {{4.5,4.,4.,4.5},{5.,5.,5.,5.}, {5.5,6.,6.,5.5}}
 }
};

float cmesh_z[2][3][4] = {
   {{0.,0.,0.,0.},{0.,0.,0.,0.},{0.,0.,0.,0.}},
   {{1.,1.,1.,1.},{1.,1.,1.,1.},{1.,1.,1.,1.}}
};

void exposeDataToDamaris(simulation_data* sim __attribute__((unused))) {
	static int firstCall = 0;
	
	if(firstCall == 0) {
		int i;
		for(i=0; i<4; i++) {

            int64_t position[1];

            position[0] = 4*i;
            damaris_set_block_position("coordinates/x2d",i,position);
			damaris_write_block("coordinates/x2d",i,rmesh_x[i%2]);

            position[0] = 5*i;
            damaris_set_block_position("coordinates/y2d",i,position);
            damaris_write_block("coordinates/y2d",i,rmesh_y[i/2]);

			damaris_write_block("coordinates/x3d",i,cmesh_x[i%2]);
			damaris_write_block("coordinates/y3d",i,cmesh_y[i/2]);
			damaris_write_block("coordinates/z3d",i,cmesh_z);
		}
		firstCall = 1;
	}

	damaris_end_iteration();
}
