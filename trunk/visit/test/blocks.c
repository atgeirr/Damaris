#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

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
simulation_data_dtor(simulation_data *sim)
{
}

void read_input_deck(int argc, char** argv) 
{ 
	if(argc != 2) {
		printf("Usage: %s mesh.xml\n",argv[0]);
		exit(0);
	}
	DC_initialize(argv[1],0);
}

void simulate_one_timestep(simulation_data *sim);

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

    //do
    {
		//simulate_one_timestep(&sim);
		exposeDataToDamaris(&sim);
    } //while(!sim.done);
    
	/* Clean up */
    simulation_data_dtor(&sim);
}

int main(int argc, char **argv)
{
    /* Read input problem setup, geometry, data. */
    read_input_deck(argc,argv);
    /* Call the main loop. */
    mainloop();

	DC_finalize();
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

void exposeDataToDamaris(simulation_data* sim) {
	static int firstCall = 0;
	
	if(firstCall == 0) {
		int i;
		for(i=0; i<4; i++) {
			DC_write_block("coordinates/x2d",sim->cycle,i,rmesh_x[i%2]);
			DC_write_block("coordinates/y2d",sim->cycle,i,rmesh_y[i/2]);

			DC_write_block("coordinates/x3d",sim->cycle,i,cmesh_x[i%2]);
			DC_write_block("coordinates/y3d",sim->cycle,i,cmesh_y[i/2]);
			DC_write_block("coordinates/z3d",sim->cycle,i,cmesh_z);
		}
		firstCall = 1;
	}

	DC_end_iteration(sim->cycle);
}
