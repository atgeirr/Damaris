#include <iostream>
#include <unistd.h>
#include <math.h>

#include "mpi.h"
#include "Damaris.h"


int Steps = 10000;
int PI = 3.1415;
int rate = 10;

using namespace std;

struct SimData {
	double* candy;
	double* handle;
    int step;
    int rank;
    int size;
	int candy_x;
	int candy_y;
	int candy_z;
	int handle_x;
	int handle_y;
	int handle_z;
};

void setCandyValue(const SimData& sim , double value, int i , int j , int k=0)
{
	sim.candy[i + j * sim.candy_x + k * sim.candy_y * sim.candy_x] =  value; // row major
}

void setHandleValue(const SimData& sim , double value, int i , int j , int k=0)
{
	sim.handle[i + j * sim.handle_x + k * sim.handle_y * sim.handle_x] =  value; // row major
}

double GetHandleFillValue(const SimData& sim, int i , int j , int k)
{
	int a = (int)k/10;
	int b = (int)i/10;

	return (a+b)*100;
}

double GetCandyFillValue(const SimData& sim, int i , int j , int k)
{
    if ((sim.step % 100) == i)
        return -100;

    int a = (int)k/10;
    int b = (int)i/10;
	int rate = (b % 2 == 0)? 1 : -1;

	return rate*((a+b)*10+((sim.rank+1)*100));
}

void InitSimData(SimData &sim ,  MPI_Comm comm)
{
    int X,Y,Z;
    MPI_Comm_size(comm , &sim.size);
    MPI_Comm_rank(comm , &sim.rank);

	damaris_parameter_get("CANDY_WIDTH",  &sim.candy_x , sizeof(int));
	damaris_parameter_get("CANDY_HEIGHT", &sim.candy_y , sizeof(int));
	damaris_parameter_get("CANDY_DEPTH",  &sim.candy_z , sizeof(int));

	damaris_parameter_get("HANDLE_WIDTH",  &sim.handle_x , sizeof(int));
	damaris_parameter_get("HANDLE_HEIGHT", &sim.handle_y , sizeof(int));
	damaris_parameter_get("HANDLE_DEPTH",  &sim.handle_z , sizeof(int));

    // Split the cube over the Z direction
	sim.candy_z = sim.candy_z/sim.size;
	sim.handle_z = sim.handle_z/sim.size;

	//Memory allocation
	sim.candy = new double[sim.candy_x * sim.candy_y * sim.candy_z];
	sim.handle = new double[sim.handle_x * sim.handle_y * sim.handle_z];
}

void FreeSimData(SimData& sim)
{
	delete [] sim.candy;
	sim.candy = nullptr;

	delete [] sim.handle;
	sim.handle = nullptr;
}

void WriteHandleCoords(SimData sim)
{
	float* XCoord = new float[sim.handle_x+1];
	float* YCoord = new float[sim.handle_y+1];
	float* ZCoord = new float[sim.handle_z+1];

	for(int i=0; i<=sim.handle_x ; i++)
		XCoord[i] = i + 120;

	for(int j=0; j<=sim.handle_y ; j++)
		YCoord[j] = j;

	for(int k=0; k<=sim.handle_z ; k++)
		ZCoord[k] = k+sim.rank*sim.handle_z;

	damaris_write("coord/handle_x" , XCoord);
	damaris_write("coord/handle_y" , YCoord);
	damaris_write("coord/handle_z" , ZCoord);
}

void WriteCandyCoords(const SimData& sim)
{
	long size = (sim.candy_x+1) * (sim.candy_y+1) * (sim.candy_z+1);

    float* XCoord = new float[size];
    float* YCoord = new float[size];
    float* ZCoord = new float[size];

	for(int i=0; i<=sim.candy_x ; i++)
		for(int j=0; j<=sim.candy_y ; j++)
			for(int k=0; k<=sim.candy_z ; k++) {

				//double tetta = 10 + 300*j/sim.y;
				double tetta = 380*j/sim.candy_y;
                double radi = tetta * PI/180;

				int index = i + j*(sim.candy_x+1) + k*(sim.candy_y+1)*(sim.candy_x+1);
				XCoord[index] = (i+10)*cos(radi);
				YCoord[index] = (i+10)*sin(radi);
				ZCoord[index] = k+sim.rank*sim.candy_z;
            }

	damaris_write("coord/candy_x" , XCoord);
	damaris_write("coord/candy_y" , YCoord);
	damaris_write("coord/candy_z" , ZCoord);
}

void HandleMainLoop(const SimData& sim)
{
	for(int i=0; i<sim.handle_x; i++)
		for(int j=0; j<sim.handle_y; j++)
			for(int k=0; k<sim.handle_z; k++) {
				double value = GetHandleFillValue(sim , i,j,k);
				setHandleValue(sim , value ,  i , j , k  );
			}

	// write results to Damaris
	if (sim.step % rate == 0)
	{
		int64_t pos[3];

		pos[0] = 0;
		pos[1] = 0;
		pos[2] = sim.rank*sim.handle_z;

		damaris_set_position("handle" ,  pos);
		damaris_write("handle" , sim.handle);
	}
}


void CandyMainLoop(const SimData& sim)
{
	for(int i=0; i<sim.candy_x; i++)
		for(int j=0; j<sim.candy_y; j++)
			for(int k=0; k<sim.candy_z; k++) {
				double value = GetCandyFillValue(sim , i , j , k);
				setCandyValue(sim , value , i , j , k  );
			}

    // write results to Damaris
	if (sim.step % rate == 0)
    {
		int64_t pos[3];
		pos[0] = 0;
		pos[1] = 0;
		pos[2] = sim.rank*sim.candy_z;

		damaris_set_position("candy" , pos);
		damaris_write("candy" , sim.candy);
    }
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc , &argv);

	if (argc != 2) {
		std::cout << "ERROR: The example should be run like: mpirun -np <X> ./Lollipop ./<xml_file> " << std::endl;
		return 1;
	}

	damaris_initialize(argv[1] , MPI_COMM_WORLD);

    int is_client;
    int err = damaris_start(&is_client);

    if ((err == DAMARIS_OK || err == DAMARIS_NO_SERVER) && is_client) {
		SimData sim;
        MPI_Comm comm;

        damaris_client_comm_get(&comm);

        InitSimData(sim , comm);
		WriteCandyCoords(sim);
		WriteHandleCoords(sim);

        for(int s=0; s < Steps ; s++) {
            sim.step = s;

			CandyMainLoop(sim);
			HandleMainLoop(sim);

			if (s % rate == 0) {
				if (sim.rank == 0) {
					cout << "Lollipop example: Iteration " << s << " out of " << Steps << endl;
				}
				damaris_end_iteration();
				sleep(3);
			}
        }

        damaris_stop();
		FreeSimData(sim);
    }

    damaris_finalize();
    MPI_Finalize();

    return 0;
}
