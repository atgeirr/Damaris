#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "Damaris.h"

#define MAX_CYCLES 10

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
		fprintf(stderr,"Usage: %s <mandelbulb-hdf5.xml>\n",argv[0]);
		exit(0);
	}

	MPI_Init(&argc,&argv);
	
	damaris_initialize(argv[1],MPI_COMM_WORLD);

	int size, rank;

	int is_client;
	int err = damaris_start(&is_client);
	
	if((err == DAMARIS_OK || err == DAMARIS_NO_SERVER) && is_client) {
	
		MPI_Comm comm;
		damaris_client_comm_get(&comm);

		damaris_parameter_get("WIDTH",&WIDTH,sizeof(int));
		damaris_parameter_get("HEIGHT",&HEIGHT,sizeof(int));
		damaris_parameter_get("DEPTH",&DEPTH,sizeof(int));

        MPI_Comm_rank(comm,&rank);
        MPI_Comm_size(comm,&size);

		int local_width         = WIDTH;
		int local_height        = HEIGHT;
		int local_depth         = DEPTH/size;

		int offset_z = rank*local_depth;

		int space[local_depth+1][local_height][local_width];
		double coord_x[local_width];
		double coord_y[local_height];
		double coord_z[local_depth+1];

		int x,y,z;
		for(x = 0; x < local_width; x++)   coord_x[x] = (double)x;
		for(y = 0; y < local_height; y++)  coord_y[y] = (double)y;
		for(z = 0; z < local_depth+1; z++) coord_z[z] = (double)(z+offset_z);

		int64_t position_space[3] = {offset_z, 0, 0};
		damaris_set_position("space",position_space);
		int64_t position_coord[1] = {offset_z};
		damaris_set_position("coord/z",position_coord);

		int i;
		for(i=0; i < MAX_CYCLES; i++) {
			double order = 4.0 + ((double)i)*8.0/MAX_CYCLES;

			double t1 = MPI_Wtime();

			for(z = 0; z < local_depth+1; z++)
			for(y = 0; y < local_height; y++)
			for(x = 0; x < local_width; x++)
			{
				vector v = {
					2.0*RANGE*x/local_width - RANGE,
					2.0*RANGE*y/local_height - RANGE,
					2.0*RANGE*(z+offset_z)/(local_depth*size) - RANGE
				};
				space[z][y][x] = iterate(&v,order);
			}

			if(i == 0) {
				damaris_write("coord/x",coord_x);
				damaris_write("coord/y",coord_y);
				damaris_write("coord/z",coord_z);
			}

			damaris_write("space",space);
	
			damaris_end_iteration();
		
			MPI_Barrier(comm);

			double t2 = MPI_Wtime();
			if(rank == 0) {
				printf("Iteration %d done in %f seconds\n",i,(t2-t1));
			}
		}

		damaris_stop();
	}
	
	damaris_finalize();
	MPI_Finalize();
	return 0;
}
