#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "include/Damaris.h"

#define MAX_ITERATION 255
#define ORDER    8
#define WIDTH  100
#define HEIGHT 100
#define DEPTH  100
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

	DC_initialize(argv[1],0);

	int space[WIDTH][HEIGHT][DEPTH];
	double coord_x[WIDTH];
	double coord_y[HEIGHT];
	double coord_z[DEPTH];

	int x,y,z;
	for(x = 0; x < WIDTH; x++)
	for(y = 0; y < HEIGHT; y++)
	for(z = 0; z < DEPTH; z++)
	{
		vector v = {
			2.0*RANGE*x/WIDTH - RANGE,
			2.0*RANGE*y/HEIGHT - RANGE,
			2.0*RANGE*z/DEPTH - RANGE
		};
		space[x][y][z] = iterate(&v);
	}

	for(x = 0; x < WIDTH; x++) 	coord_x[x] = ((double)x)/WIDTH;
	for(y = 0; y < HEIGHT; y++) coord_y[y] = ((double)y)/HEIGHT;
	for(z = 0; z < DEPTH; z++)	coord_z[z] = ((double)z)/DEPTH;

	DC_write("coord/x",0,coord_x);
	DC_write("coord/y",0,coord_y);
	DC_write("coord/z",0,coord_z);
	DC_write("space",0,space);
	DC_end_iteration(0);
	DC_finalize();
	return 0;
}
