#include <stdio.h>
#include "include/Damaris.h"      

int main(int argc, char** argv) 
{
	int id = 0;
	float* mydata = NULL;
	int x = 64;
	int y = 16;
	int z = 4;

	char* varname = "my group/my variable";
	char* event   = "my event";

	if(argc != 2) {
		printf("Usage: ./test_alloc_c <config.xml>\n");
		exit(0);
	}

	DC_initialize(argv[1],id);	

	mydata = (float*)DC_alloc(varname,0);

	if(mydata == NULL) {
		printf("Error when allocating buffer\n");
		exit(0);
	}

	int i,j,k;
	for(i = 0; i < x; i++) {
	for(j = 0; j < y; j++) {
	for(k = 0; k < z; k++) {
		mydata[i*y*z+j*z+k] = (float)(i*j*k);
	}
	}
	}
		
	DC_commit(varname,0);
	DC_signal(event,0);
	DC_finalize();

	return 0;
}
