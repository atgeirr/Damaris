/*****************************************************************************
 *
 * Copyright (c) 2000 - 2010, The Regents of the University of California
 * Produced at the Lawrence Livermore National Laboratory
 * LLNL-CODE-400142
 * All rights reserved.
 *
 * This file is  part of VisIt. For  details, see https://visit.llnl.gov/.  The
 * full copyright notice is contained in the file COPYRIGHT located at the root
 * of the VisIt distribution or at http://www.llnl.gov/visit/copyright.html.
 *
 * Redistribution  and  use  in  source  and  binary  forms,  with  or  without
 * modification, are permitted provided that the following conditions are met:
 *
 *  - Redistributions of  source code must  retain the above  copyright notice,
 *    this list of conditions and the disclaimer below.
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this  list of  conditions  and  the  disclaimer (as noted below)  in  the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of  the LLNS/LLNL nor the names of  its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR  IMPLIED WARRANTIES, INCLUDING,  BUT NOT  LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND  FITNESS FOR A PARTICULAR  PURPOSE
 * ARE  DISCLAIMED. IN  NO EVENT  SHALL LAWRENCE  LIVERMORE NATIONAL  SECURITY,
 * LLC, THE  U.S.  DEPARTMENT OF  ENERGY  OR  CONTRIBUTORS BE  LIABLE  FOR  ANY
 * DIRECT,  INDIRECT,   INCIDENTAL,   SPECIAL,   EXEMPLARY,  OR   CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT  LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF  USE, DATA, OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER
 * CAUSED  AND  ON  ANY  THEORY  OF  LIABILITY,  WHETHER  IN  CONTRACT,  STRICT
 * LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY  WAY
 * OUT OF THE  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#include "Damaris.h"

int NPTS = 10000;

typedef struct
{
	int     cycle;
	double  time;
	int     done;

	float    angle;
	float   *x;
	float   *y;
} simulation_data;

	void
simulation_data_ctor(simulation_data *sim)
{
	sim->cycle = 0;
	sim->time = 0.;
	sim->done = 0;

	sim->angle = 0.;
	sim->x = (float *)malloc(sizeof(float) * NPTS);
	sim->y = (float *)malloc(sizeof(float) * NPTS);
}

	void
simulation_data_dtor(simulation_data *sim)
{
	free(sim->x);
	free(sim->y);
}

void simulate_one_timestep(simulation_data *sim)
{
	int i;
	for(i = 0; i < NPTS; ++i)
	{
		float t = ((float)i) / ((float)(NPTS-1));
		float a = 4. * 3.14159 * t;
		sim->x[i] = a;
		sim->y[i] = sin(sim->angle + a);
	}

	sim->angle = sim->angle + 0.05;

	++sim->cycle;
	sim->time += (M_PI / 10.);

	printf("Simulating time step: cycle=%d, time=%lg\n", sim->cycle, sim->time);
}

void exposeDataToDamaris(simulation_data* s);

void mainloop(simulation_data *sim)
{
	do
	{
		sleep(1);
		simulate_one_timestep(sim);
		exposeDataToDamaris(sim);
	} while(!sim->done);
}

int main(int argc, char **argv)
{
	MPI_Init(&argc,&argv);

	if(argc != 2) {
		printf("Usage: %s curve.xml\n",argv[0]);
		exit(0);
	}

	DC_initialize(argv[1],MPI_COMM_WORLD);

	simulation_data sim;
	DC_parameter_get("NPTS",&NPTS,sizeof(NPTS));

	simulation_data_ctor(&sim);

	mainloop(&sim);

	DC_kill_server();
	DC_finalize();

	simulation_data_dtor(&sim);

	return 0;
}

void exposeDataToDamaris(simulation_data* sim) { 
	DC_write("coordinates/x",sim->x);
	DC_write("coordinates/y",sim->y);
	DC_end_iteration();
}
