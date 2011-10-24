#include <stdio.h>
#include <stdlib.h>
#include "client/Client.h"      

int main(int argc, char** argv) 
{
	int id = 0;
	float mydata[64][16][2];
	char* varname = "my variable";
	char* event = "my event";

	if(argc != 2) {
		printf("Usage: ./test_c <config.xml>\n");
		exit(0);
	}

	DC_initialize(argv[1],id);

	int i,j,k;
	for(i = 0; i < 64; i++) {
	for(j = 0; j < 16; j++) {
	for(k = 0; k <  2; k++) {
		mydata[i][j][k] = i*j*k;
	}
	}
	}

	DC_write(varname,0,mydata);
	DC_signal(event,0);

	DC_finalize();	
	return 0;
}
