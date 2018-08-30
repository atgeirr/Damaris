#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <unistd.h>

#include "Damaris.h"

#define MAX_CYCLES 100


struct SimData {
	int WIDTH;
	int HEIGHT;
	int domains; // change the number of domains from .xml file (both from the <domains count=".."> tag and domains parameter)
	int size;
	int rank;
	int local_width;
	int local_height;
	int GX0;    // ***************** NOT IMPLEMENTED YET **************
	int GX1;    // * The ghost zones for each process will be something like:
	int GY0;    // * ghost="GX0:GX1,GY0:GY1". Do not forget to update the related parameters as well.
	int GY1;    // ****************************************************
	int total_width;
	int total_height;
	int ghost_value;
	int* data;
	int iteration;
};

void setValue(const SimData& sim , int value , int i , int j)
{
	sim.data[i + j * sim.local_width] = value;  // row major
}

void fillValue(const SimData& sim , int i , int j)
{
	int a = i/5;
	int b = j/5;
	int shift = (sim.iteration % 5) + 2;

	int value = (a*b)*10+(sim.rank+1)*100 ;

	if ((a+b) % shift == 0)
		value *= -1;

	setValue(sim , value , i , j);
}

void FreeSimData(SimData& sim)
{
	delete [] sim.data;
	sim.data = nullptr;
}

void InitSimData(SimData& sim , MPI_Comm comm)
{
	damaris_parameter_get("WIDTH" , &sim.WIDTH , sizeof(int));
	damaris_parameter_get("HEIGHT" , &sim.HEIGHT , sizeof(int));
	damaris_parameter_get("domains", &sim.domains , sizeof(int));
	damaris_parameter_get("GX0" , &sim.GX0 , sizeof(int));
	damaris_parameter_get("GY0" , &sim.GY0 , sizeof(int));
	damaris_parameter_get("GX1" , &sim.GX1 , sizeof(int));
	damaris_parameter_get("GY1" , &sim.GY1 , sizeof(int));

	MPI_Comm_rank(comm , &sim.rank);
	MPI_Comm_size(comm , &sim.size);

	sim.ghost_value = 666; // a wiered value

	sim.local_width      = sim.WIDTH/sim.size;
	sim.local_height     = sim.HEIGHT/sim.domains;

	sim.total_width = sim.local_width + sim.GX0 + sim.GX1; // GX:GX
	sim.total_height = sim.local_height + sim.GY0 + sim.GY1; // GY:GY

	sim.data = new int[(sim.total_height)*(sim.total_width)*sizeof(int)];
}

void WriteCoordinatesNormal(const SimData& sim)
{
	float* xCoord = new float[sim.local_width+1];
	float* yCoord = new float[sim.local_height+1];

	for (int i=0; i<=sim.local_width ; i++)
		xCoord[i] = sim.rank*sim.local_width + i;

	for (int j=0; j<=sim.local_height ; j++)
		yCoord[j] = j;

	damaris_write("coord/x" , xCoord);
	damaris_write("coord/y" , yCoord);

	delete [] xCoord;
	delete [] yCoord;
}

void MainLoopNormal(const SimData& sim)
{
    int x,y;

	int offset_width = sim.rank*sim.local_width;
	int offset_height = 0;

    int64_t position_space[2];

    position_space[0] = offset_width;
    position_space[1] = offset_height;

	for(x = 0; x < sim.total_width; x++)
		for(y = 0; y < sim.total_height; y++) {

			if ((x < sim.GX0) || (y < sim.GY0))
				setValue(sim , sim.ghost_value , x, y);
			else if ((x >= sim.total_width - sim.GX1) || (y >= sim.total_height - sim.GY1))
				setValue(sim , sim.ghost_value , x, y);
            else
				fillValue(sim , x , y);
        }

	damaris_set_position("space" , position_space);
	damaris_write("space" , sim.data);
    damaris_end_iteration();
}

void WriteCoordinatesBlock(const SimData& sim)
{
	float* xCoord = new float[sim.local_width+1];
	float* yCoord = new float[sim.local_height+1];

	for(int dom=0; dom<sim.domains ; dom++) {

		for (int i=0; i<=sim.local_width ; i++)
			xCoord[i] = sim.rank*sim.local_width + i;

		for (int j=0; j<=sim.local_height ; j++)
			yCoord[j] = dom * sim.local_height + j;

		damaris_write_block("coord/x" , dom , xCoord);
		damaris_write_block("coord/y" , dom , yCoord);
	}

	delete [] xCoord;
	delete [] yCoord;
}

void MainLoopBlock(const SimData& sim)
{
    int dom;

	for(dom=0; dom<sim.domains ; dom++) {
		int offset_width = sim.rank * sim.local_width;
		int offset_height = dom * sim.local_height;

        int64_t position_space[2];
        int x,y;

        position_space[0] = offset_width;
        position_space[1] = offset_height;

		for (x = 0; x < sim.total_width; x++) {
			for (y = 0; y < sim.total_height; y++) {
				if ((x < sim.GX0) || (y < sim.GY0))
					setValue(sim , sim.ghost_value , x, y);
				else if ((x >= sim.total_width - sim.GX1) || (y >= sim.total_height - sim.GY1))
					setValue(sim , sim.ghost_value , x, y);
                else
					fillValue(sim , x, y);
            }
        }

        damaris_set_block_position("space", dom, position_space);
		damaris_write_block("space", dom, sim.data);
    }

    damaris_end_iteration();
}

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		std::cout << "ERROR: Usage is like: mpirun -np <N> " << argv[0] << "  <plate.xml>" << std::endl;
		exit(0);
	}

	MPI_Init(&argc,&argv);
	
	damaris_initialize(argv[1] , MPI_COMM_WORLD);

	int is_client;
	int err = damaris_start(&is_client);
	
	if((err == DAMARIS_OK || err == DAMARIS_NO_SERVER) && is_client) {
	
		MPI_Comm comm;
		SimData sim;
		damaris_client_comm_get(&comm);

		InitSimData(sim , comm);

		//Writing coordinates
		if (sim.domains == 1)
			WriteCoordinatesNormal(sim);
		else
			WriteCoordinatesBlock(sim);

		// Main loop
		for(int i=0; i < MAX_CYCLES; i++) {
			double t1 = MPI_Wtime();
			sim.iteration = i;

			if (sim.domains == 1)
				MainLoopNormal(sim);
			else
				MainLoopBlock(sim);

			sleep(3);

			MPI_Barrier(comm);

			double t2 = MPI_Wtime();

			if(sim.rank == 0)
				printf("Plate Simulation: Iteration %d done in %f seconds\n",i,(t2-t1));
		}

		damaris_stop();
		FreeSimData(sim);
	}

	damaris_finalize();
	MPI_Finalize();
	return 0;
}
