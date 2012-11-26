#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "include/Damaris.h"

#define MAX_CYCLES 500

int WIDTH;
int HEIGHT;
int DEPTH;

#define RANGE  1.2

typedef struct {
	double x;
	double y;
	double z;
} vector;

MPI_Comm comm;

double do_r(const vector* v)
{
	return sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
}

double do_theta(double n, const vector* v)
{
	return n*atan2(v->y,v->x);
}

double do_phi(double n, const vector* v)
{
	return n*asin(v->z/do_r(v));
}

int iterate(const vector* v0, double order)
{
	vector v = *v0;
	double n = order;
	int i;
	for(i=0; i < MAX_CYCLES && (v.x*v.x+v.y*v.y+v.z*v.z < 2.0); i++) {
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
		v.z = vn.z + v0->z;
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
	if( ! DC_mpi_init_and_start(argv[1],MPI_COMM_WORLD)) {
		MPI_Finalize();
		return 0;
	}
	comm = DC_mpi_get_client_comm();

	DC_parameter_get("WIDTH",&WIDTH,sizeof(int));
	DC_parameter_get("HEIGHT",&HEIGHT,sizeof(int));
	DC_parameter_get("DEPTH",&DEPTH,sizeof(int));

	MPI_Comm_rank(comm,&rank);
	MPI_Comm_size(comm,&nbprocs);

/*
	int size = (int)sqrt((double)nbprocs);
	if(size*size != nbprocs) {
		fprintf(stderr,"Number of processes must be a square\n");
		DC_kill_server();
		DC_finalize();
		MPI_Finalize();
		exit(0);
	}
*/

	int offset_z = rank*DEPTH;

	int space[DEPTH+1][HEIGHT][WIDTH];
	double coord_x[WIDTH];
	double coord_y[HEIGHT];
	double coord_z[DEPTH+1];

	int x,y,z;
	for(x = 0; x < WIDTH; x++)   coord_x[x] = (double)x;
	for(y = 0; y < HEIGHT; y++)  coord_y[y] = (double)y;
	for(z = 0; z < DEPTH+1; z++) coord_z[z] = (double)(z+offset_z);

	int i;
	for(i=0; i < MAX_CYCLES; i++) {
		double order = 4.0 + ((double)i)*8.0/MAX_CYCLES;

		double t1 = MPI_Wtime();

		for(z = 0; z < DEPTH+1; z++)
		for(y = 0; y < HEIGHT; y++)
		for(x = 0; x < WIDTH; x++)
		{
			vector v = {
				2.0*RANGE*x/WIDTH - RANGE,
				2.0*RANGE*y/HEIGHT - RANGE,
				2.0*RANGE*(z+offset_z)/(DEPTH*nbprocs) - RANGE
			};
			space[z][y][x] = iterate(&v,order);
		}

		if(i == 0) {
			DC_write("coord/x",coord_x);
			DC_write("coord/y",coord_y);
			DC_write("coord/z",coord_z);
		}

		DC_write("space",space);
	
		DC_signal("clean");

		DC_end_iteration();
		
		MPI_Barrier(comm);

		double t2 = MPI_Wtime();
		if(rank == 0) {
			printf("Iteration %d done in %f seconds\n",i,(t2-t1));
		}
	}

	DC_kill_server();
	DC_finalize();
	MPI_Finalize();
	return 0;
}
