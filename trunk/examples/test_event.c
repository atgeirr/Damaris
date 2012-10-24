#include <stdio.h>
#include <stdlib.h>
#include "include/Damaris.h"      

int main(int argc, char** argv) 
{
	int id = 0;
	char* script = "my script";
	char* event = "my event";

	if(argc != 2) {
		printf("Usage: ./test_c <config.xml>\n");
		exit(0);
	}

	DC_initialize(argv[1],id);

	DC_signal(script);
	DC_signal(event);

	DC_end_iteration();

	DC_finalize();
	return 0;
}
