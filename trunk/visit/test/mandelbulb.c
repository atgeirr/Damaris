#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "include/Damaris.h"

#define MAX_ITERATION 255
#define ORDER    8
#define WIDTH  100
#define HEIGHT 100
#define DEPTH  200
#define RANGE  1.2

typedef struct {
	double x;
	double y;
	double z;
} vector;

double do_r(const vector* v)
{
	return sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
}

double do_theta(int n, const vector* v)
{
	return n*atan2(v->y,v->x);
}

double do_phi(int n, const vector* v)
{
	return n*asin(v->z/do_r(v));
}

int iterate(const vector* v0)
{
	vector v = *v0;
	int n = ORDER;
	int i;
	for(i=0; i < MAX_ITERATION && (v.x*v.x+v.y*v.y+v.z*v.z < 2.0); i++) {
		double r 		= do_r(&v);
		double theta 	= do_theta(n,&v);
		double phi 		= do_phi(n,&v);
		double rn 		= pow(r,n);
		double cos_theta = cos(theta);
		double sin_theta = sin(theta);
		double cos_phi	 = cos(phi);
		double sin_phi	 = sin(phi);
		vector vn = {
			rn*cos_theta*cos_phi,
			rn*sin_theta*cos_phi,
			-rn*sin_phi
		};
		v.x = vn.x + v0->x;
		v.y = vn.y + v0->y;
		v.z =vn.z + v0->z;
	}
	return i;
}

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		fprintf(stderr,"Usage: %s <config.xml>\n",argv[0]);
		exit(0);
	}

	MPI_Init(&argc,&argv);

	int nbprocs, rank;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&nbprocs);

	int size = (int)sqrt((double)nbprocs);
	if(size*size != nbprocs) {
		fprintf(stderr,"Number of processes must be a square\n");
		MPI_Finalize();
		exit(0);
	}

	DC_initialize(argv[1],rank);

	int offset_x = (rank/size)*WIDTH;
	int offset_y = (rank%size)*HEIGHT;
	int offset_z = 0;

	int space[DEPTH][HEIGHT+1][WIDTH+1];
	double coord_x[WIDTH+1];
	double coord_y[HEIGHT+1];
	double coord_z[DEPTH];

	int x,y,z;
	for(z = 0; z < DEPTH; z++)
	for(y = 0; y < HEIGHT+1; y++)
	for(x = 0; x < WIDTH+1; x++)
	{
		vector v = {
			2.0*RANGE*(x+offset_x)/(WIDTH*size)  - RANGE,
			2.0*RANGE*(y+offset_y)/(HEIGHT*size) - RANGE,
			2.0*RANGE*(z+offset_z)/DEPTH - RANGE
		};
		space[z][y][x] = iterate(&v);
	}

	for(x = 0; x < WIDTH+1; x++)  coord_x[x] = ((double)(x+offset_x))/((double)(WIDTH*size));
	for(y = 0; y < HEIGHT+1; y++) coord_y[y] = ((double)(y+offset_y))/((double)(HEIGHT*size));
	for(z = 0; z < DEPTH; z++) 	  coord_z[z] = ((double)(z+offset_z))/DEPTH;

	DC_write("coord/x",0,coord_x);
	DC_write("coord/y",0,coord_y);
	DC_write("coord/z",0,coord_z);
	DC_write("space",0,space);

	DC_end_iteration(0);

	DC_finalize();

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();

	return 0;
}
