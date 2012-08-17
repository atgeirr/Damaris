#include <iostream>
#include <mpi.h>
#include "include/Damaris.hpp"

void hello(const std::string& name, int32_t source, int32_t iteration, const char* args)
{
	std::cout << "Hello world triggered by event \"" << name << "\"" << std::endl;
} 

int main(int argc, char** argv) 
{
	MPI_Comm all = MPI_COMM_WORLD;
	MPI_Comm clients;
	int rank, size;

	if(argc != 2) {
		std::cout << "Usage: mpirun -np 2 ./test_mpi_c <config.xml>\n";
		exit(0);
	}
	
	MPI_Init(&argc,&argv);

	DC_bind_function("my event",&hello);

	if(DC_mpi_init_and_start(argv[1],all)) {
		clients = DC_mpi_get_client_comm();
		MPI_Comm_rank(clients,&rank);
		MPI_Comm_size(clients,&size);
		
		DC_signal("my event",0);

		DC_kill_server();
		MPI_Barrier(clients);
		DC_finalize();
	}

	MPI_Finalize();
	return 0;
}
