/*****************************************************************************
* This program is a modified version of a program originaly provided with the
* VisIt software developed by LLNL and following the above Copyright.
*
* This program is part of the Damaris software, provided under LGPL-3 licence.
******************************************************************************/

/*****************************************************************************
*
* Copyright (c) 2000 - 2010, Lawrence Livermore National Security, LLC
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

/* SIMPLE SIMULATION SKELETON */
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
    for(i=0;i<60;i++){
		simulate_one_timestep(&sim);
		exposeDataToDamaris(&sim);
	    sleep(1);
    }

    /* Clean up */
    simulation_data_dtor(&sim);
}

int main(int argc, char **argv)
{
	MPI_Init(&argc,&argv);
	
	damaris_initialize("blocks.xml",MPI_COMM_WORLD);
	
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
float rmesh_x[] = {0., 1., 2.5, 5.};
float rmesh_y[] = {0., 2., 2.25, 2.55,  5.};
float zonal[] = {1.,2.,3.,4.,5.,6.,7.,8.,9.,10.,11.,12.};
float zonal_vector[][2] = {
   { 1., 2.},{ 3., 4.},{ 5., 6.},{ 7., 8.},{ 9.,10.},{11.,12.},
   {13.,14.},{15.,16.},{17.,18.},{19.,20.},{21.,22.},{23.,24.}
};
const char *zonal_labels = "zone1\0\0zone2\0\0zone3\0\0zone4\0\0zone5\0\0zone6\0\0zone7\0\0zone8\0\0zone9\0\0zone10\0zone11\0zone12";

/* Curvilinear mesh */
float cmesh_x[2][3][4] = {
   {{0.,1.,2.,3.},{0.,1.,2.,3.}, {0.,1.,2.,3.}},
   {{0.,1.,2.,3.},{0.,1.,2.,3.}, {0.,1.,2.,3.}}
};
float cmesh_y[2][3][4] = {
   {{0.5,0.,0.,0.5},{1.,1.,1.,1.}, {1.5,2.,2.,1.5}},
   {{0.5,0.,0.,0.5},{1.,1.,1.,1.}, {1.5,2.,2.,1.5}}
};
float cmesh_z[2][3][4] = {
   {{0.,0.,0.,0.},{0.,0.,0.,0.},{0.,0.,0.,0.}},
   {{1.,1.,1.,1.},{1.,1.,1.,1.},{1.,1.,1.,1.}}
};
double nodal[2][3][4] = {
   {{1.,2.,3.,4.},{5.,6.,7.,8.},{9.,10.,11.,12}},
   {{13.,14.,15.,16.},{17.,18.,19.,20.},{21.,22.,23.,24.}}
};
double nodal_vector[2][3][4][3] = {
   { {{ 0., 1., 2.},{ 3., 4., 5.},{ 6., 7., 8.},{ 9.,10.,11.}},
     {{12.,13.,14.},{15.,16.,17.},{18.,19.,20.},{21.,22.,23.}},
     {{24.,25.,26.},{27.,28.,29.},{30.,31.,32.},{33.,34.,35.}} },

   { {{36.,37.,38.},{39.,40.,41.},{42.,43.,44.},{45.,46.,47.}},
     {{48.,49.,50.},{51.,52.,53.},{54.,55.,56.},{57.,58.,59.}},
     {{60.,61.,62.},{63.,64.,65.},{66.,67.,68.},{69.,70.,71}} }
};

void exposeDataToDamaris(simulation_data* sim __attribute__((unused))) {
	static int firstCall = 0;

	if(firstCall == 0) {
		damaris_write("coordinates/x2d",rmesh_x);
		damaris_write("coordinates/y2d",rmesh_y);

		damaris_write("coordinates/x3d",cmesh_x);
		damaris_write("coordinates/y3d",cmesh_y);
		damaris_write("coordinates/z3d",cmesh_z);
		firstCall = 1;
	}

	damaris_write("zonal/scalar",zonal);
	damaris_write("zonal/vector",zonal_vector);
	damaris_write("zonal/labels",zonal_labels);

	damaris_write("nodal/scalar",nodal);
	damaris_write("nodal/vector",nodal_vector);
	
	damaris_end_iteration();
}
