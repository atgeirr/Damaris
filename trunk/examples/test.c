#include <stdio.h>
#include <stdlib.h>
#include "include/Damaris.h"      

int main(int argc, char** argv) 
{
	int id = 0;
	float mydata[64][16][4];
	char* varname = "my group/my variable";
	char* event = "my script";

	if(argc != 2) {
		printf("Usage: ./test_c <config.xml>\n");
		exit(0);
	}

	DC_initialize(MPI_COMM_WORLD,argv[1]);

	int i,j,k;
	for(i = 0; i < 64; i++) {
	for(j = 0; j < 16; j++) {
	for(k = 0; k <  4; k++) {
		mydata[i][j][k] = i*j*k;
	}
	}
	}
	
	DC_write(varname,mydata);
	DC_signal(event);
	DC_end_iteration();

	DC_finalize();
	return 0;
}
