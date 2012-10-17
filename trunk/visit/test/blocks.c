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
float rmesh_x1[] = {0., 1., 2.5, 5.};
float rmesh_y1[] = {0., 2., 2.25, 2.55,  5.};

float rmesh_x2[] = {5., 6., 7.5, 10.};
float rmesh_y2[] = {0., 2., 2.25, 2.55,  5.};

float rmesh_x3[] = {5., 6., 7.5, 10.};
float rmesh_y3[] = {5., 7., 7.25, 7.55,  10.};

float rmesh_x4[] = {0., 1., 2.5, 5.};
float rmesh_y4[] = {5., 7., 7.25, 7.55,  10.};


void exposeDataToDamaris(simulation_data* sim) {
	static int firstCall = 0;
	
	if(firstCall == 0) {
		DC_write_block("coordinates/x2d",sim->cycle,0,rmesh_x1);
		DC_write_block("coordinates/y2d",sim->cycle,0,rmesh_y1);
	
		DC_write_block("coordinates/x2d",sim->cycle,1,rmesh_x2);
		DC_write_block("coordinates/y2d",sim->cycle,1,rmesh_y2);

		DC_write_block("coordinates/x2d",sim->cycle,2,rmesh_x3);
		DC_write_block("coordinates/y2d",sim->cycle,2,rmesh_y3);

		DC_write_block("coordinates/x2d",sim->cycle,3,rmesh_x4);
		DC_write_block("coordinates/y2d",sim->cycle,3,rmesh_y4);
		firstCall = 1;
	}

	DC_end_iteration(sim->cycle);
}
