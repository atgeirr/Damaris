#include <stdio.h>
#include <stdlib.h>
#include "include/Damaris.h"      

int main(int argc, char** argv) 
{
	int id = 0;

	float mydata[32][8][4];
	int start[] = {0,0,0};
	int end[]   = {31,7,3};

	char* varname = "my group/my variable";
	char* event = "my event";

	if(argc != 2) {
		printf("Usage: ./test_c <config.xml>\n");
		exit(0);
	}

	DC_initialize(argv[1],id);

	DC_chunk_handle_t chunk = DC_chunk_set(3,start,end);

	int i,j,k;
	for(i = 0; i < 32; i++) {
	for(j = 0; j < 8; j++) {
	for(k = 0; k < 4; k++) {
		mydata[i][j][k] = i*j*k;
	}
	}
	}
	
	DC_chunk_write(chunk,varname,0,mydata);
	DC_signal(event,0);

	DC_chunk_free(chunk);

	DC_finalize();
	return 0;
}
