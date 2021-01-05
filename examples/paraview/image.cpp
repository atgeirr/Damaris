#include <iostream>
#include <unistd.h>

#include "mpi.h"
#include "Damaris.h"


int Steps = 10000;


struct simdata {
  double* zonal_cube;
  double* nodal_cube;
    int step;
    int rank;
    int size;

  int zonal_x;
  int zonal_y;
  int zonal_z;

  int nodal_x;
  int nodal_y;
  int nodal_z;
};


template <typename T>
void setZonalValue(simdata& sim , T value, int i , int j , int k=0) {
  if ((i == sim.zonal_x) || (j == sim.zonal_y) || (k == sim.zonal_z))
    return;

  sim.zonal_cube[i + j * sim.zonal_x + k * sim.zonal_y * sim.zonal_x] = value; // row major
}

template <typename T>
void setNodalValue(simdata& sim , T value, int i , int j , int k=0) {
  sim.nodal_cube[i + j * sim.nodal_x + k * sim.nodal_y * sim.nodal_x] = value; // row major
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
    
    // Pass the specific size parameter back to Damaris
    damaris_parameter_set("size",&sim.size,sizeof(int));

    damaris_parameter_get("WIDTH",&X,sizeof(int));
    damaris_parameter_get("HEIGHT",&Y,sizeof(int));
    damaris_parameter_get("DEPTH",&Z,sizeof(int));

    // Split the cube over the Z direction
    int local_z = Z/sim.size;

      sim.zonal_x = X;
      sim.zonal_y = Y;
      sim.zonal_z = local_z;
      sim.zonal_cube = new double[sim.zonal_x*sim.zonal_y*sim.zonal_z];

      sim.nodal_x = sim.zonal_x + 1;
      sim.nodal_y = sim.zonal_y + 1;
      sim.nodal_z = sim.zonal_z + 1;
      sim.nodal_cube = new double[sim.nodal_x*sim.nodal_y*sim.nodal_z];
}

void FreeSimData(simdata& sim)
{
   delete [] sim.zonal_cube;
   delete [] sim.nodal_cube;

   sim.zonal_cube = nullptr;
   sim.nodal_cube = nullptr;
}

void WriteCoordinates(simdata sim)
{
  float* XCoord = new float[sim.nodal_x];
  float* YCoord = new float[sim.nodal_y];
  float* ZCoord = new float[sim.nodal_z];

  for(int i=0; i<sim.nodal_x ; i++)
    XCoord[i] = i*2;

  for(int j=0; j<sim.nodal_y ; j++)
    YCoord[j] = j*3;

  for(int k=0; k<sim.nodal_z ; k++)
    ZCoord[k] = k+sim.rank*sim.zonal_z;

  damaris_write("coord/x" , XCoord);
  damaris_write("coord/y" , YCoord);
  damaris_write("coord/z" , ZCoord);
    
  delete [] XCoord;
  delete [] YCoord;
  delete [] ZCoord;
}

void SimMainLoop(simdata& sim)
{
  for(int i=0; i<=sim.zonal_x; i++)
    for(int j=0; j<=sim.zonal_y; j++)
      for(int k=0; k<=sim.zonal_z; k++)
      {
        setZonalValue(sim , GetFillValue(sim , i , j , k) ,  i , j , k  );
        setNodalValue(sim , i*j*GetFillValue(sim , i,j,k) ,  i , j , k  );
      }


   // write results to Damaris
   if (sim.step % 10 == 0)
   {
      if (sim.rank == 0)
      {
         std::cout << "Image example: Iteration " << sim.step << " out of " << Steps << std::endl;
      }
      int64_t pos[3];

      pos[0] = 0;
      pos[1] = 0;
      pos[2] = sim.rank*sim.zonal_z;

      damaris_set_position("zonal_pressure" ,  pos);
      damaris_write("zonal_pressure" , sim.zonal_cube);

      damaris_set_position("nodal_pressure" ,  pos);
      damaris_write("nodal_pressure" , sim.nodal_cube);

      damaris_end_iteration();

      sleep(5);
   }
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc , &argv);
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
