#include <iostream>
#include <unistd.h>
#include <math.h>

#include "mpi.h"
#include "Damaris.h"


int Steps = 10000;
int PI = 3.1415;

using namespace std;

struct simdata {
    double* cube;
    int step;
    int rank;
    int size;
    int x;
    int y;
    int z;
};

template <typename T>
T getValue(simdata& sim, int i , int j , int k=0) {
    return sim.cube[i + j * sim.x + k * sim.y * sim.x];  // row major
}

template <typename T>
void setValue(simdata& sim , T value, int i , int j , int k=0) {
    sim.cube[i + j * sim.x + k * sim.y * sim.x] =  value; // row major
}

double GetFillValue(simdata& sim, int i , int j , int k)
{
    if ((sim.step % 100) == i)
        return -100;

    int a = (int)k/10;
    int b = (int)i/10;

    return ((a+b)*10+(sim.rank*100));
}

void InitSimData(simdata &sim ,  MPI_Comm comm)
{
    int X,Y,Z;
    MPI_Comm_size(comm , &sim.size);
    MPI_Comm_rank(comm , &sim.rank);

    damaris_parameter_get("WIDTH",&X,sizeof(int));
    damaris_parameter_get("HEIGHT",&Y,sizeof(int));
    damaris_parameter_get("DEPTH",&Z,sizeof(int));

    // Split the cube over the Z direction
    int local_z = Z/sim.size;

    sim.cube = new double[X*Y*local_z];

    sim.x = X;
    sim.y = Y;
    sim.z = local_z;
}

void FreeSimData(simdata& sim)
{
    delete [] sim.cube;
    sim.cube = NULL;
}

void WriteCoordinates(simdata sim)
{
	long size = (sim.x+1) * (sim.y+1) * (sim.z+1);

    float* XCoord = new float[size];
    float* YCoord = new float[size];
    float* ZCoord = new float[size];

	for(int i=0; i<=sim.x ; i++)
		for(int j=0; j<=sim.y ; j++)
			for(int k=0; k<=sim.z ; k++) {

                double tetta = 10 + 70*j/sim.y;
                double radi = tetta * PI/180;

				int index = i + j*(sim.x+1) + k*(sim.y+1) * (sim.x+1);
				XCoord[index] = (i+10)*cos(radi);
				YCoord[index] = (i+10)*sin(radi);
				ZCoord[index] = k+sim.rank*sim.z;
            }


    damaris_write("coord/x" , XCoord);
    damaris_write("coord/y" , YCoord);
    damaris_write("coord/z" , ZCoord);
}

void SimMainLoop(simdata& sim)
{
	for(int i=0; i<sim.x; i++)
		for(int j=0; j<sim.y; j++)
			for(int k=0; k<sim.z; k++)
                setValue(sim , GetFillValue(sim , i,j,k) ,  i , j , k  );

    // write results to Damaris
    if (sim.step % 10 == 0)
    {
        if (sim.rank == 0)
        {
			cout << "Sector example: Iteration " << sim.step << " out of " << Steps << endl;
        }

		int64_t pos[3];
		pos[0] = 0;
		pos[1] = 0;
		pos[2] = sim.rank*sim.z;

		damaris_set_position("pressure" , pos);
        damaris_write("pressure" , sim.cube);
        damaris_end_iteration();

		sleep(3);
    }
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc , &argv);

	if (argc != 2)
	{
		std::cout << "The example should be run like: mpirun -np <X> ./sector ./<xml_file> " << std::endl;
		return 1;
	}

	damaris_initialize(argv[1] , MPI_COMM_WORLD);


    int is_client;
    int err = damaris_start(&is_client);

    if ((err == DAMARIS_OK || err == DAMARIS_NO_SERVER) && is_client) {
        simdata sim;
        MPI_Comm comm;

        damaris_client_comm_get(&comm);

        InitSimData(sim , comm);
        WriteCoordinates(sim);

        for(int s=0; s < Steps ; s++) {
            sim.step = s;
            SimMainLoop(sim);
        }

        FreeSimData(sim);
        damaris_stop();
    }

    damaris_finalize();
    MPI_Finalize();

    return 0;
}
