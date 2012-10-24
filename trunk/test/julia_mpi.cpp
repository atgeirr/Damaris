/* ================================================================================
 * This test is part of the Damaris program.
 * Usage: (it only works with 5 processes, including 1 dedicated core)
 *		export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
 * 		mpirun -np 5 ./julia_mpi julia_mpi.xml
 * Usage without dedicated core (4 processes only):
 *		export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
 *		mpirun -np 4 ./julia_mpi julia_mpi_no_server.xml
 * This program should generate 100 png images of Julia sets, then exit.
 * The server is killed automatically by the client, except if you kill the client
 * before it ends. In this case, find the pid of the server using ps aux then kill
 * it manually.
 * ================================================================================ */

#include <iostream>
#include <string>
#include <complex>
#include <mpi.h>

#include "include/Damaris.hpp"

#define PI 3.14159265358979323846
#define ITERATIONS 100

int HEIGHT = 0;
int WIDTH  = 0;

// Main function that checks for convergence of the series
// given an initial term z0 and a parameter c.
// The series is defined by z(n+1) <-- z(n)^2 + c
char julia(std::complex<double> c, std::complex<double> z0) {
	std::complex<double> zi = z0;
	for(int i = 0; i < 256; i++) {
		zi = (zi*zi) + c;
		if(std::abs<double>(zi) >= 2.0) return (char)i;
	}
	return 255;
}

// This function calls the julia function for all pixels
// handled by the process (from offset_x to offset_x+WIDTH/2 and
// from offset_y to offset_y+HEIGHT/2).
int compute(char* data, std::complex<double> c, int offset_x, int offset_y) {
	for(int i=0; i < WIDTH/2; i++) {
	for(int j=0; j < HEIGHT/2; j++) {
		float x = ((float)(offset_x+i-WIDTH/2)*2.2)/((float)WIDTH);
		float y = ((float)(offset_y+j-HEIGHT/2)*2.2)/((float)HEIGHT);
		data[i*HEIGHT/2+j] = julia(c,std::complex<double>(x,y));
	}
	}
	return 0;
}

// Main function
int main(int argc, char** argv) 
{
	int rank, size;
	MPI_Comm global = MPI_COMM_WORLD;
	MPI_Comm comm; // this will be the communicator gathering clients

	// Initialize MPI
	MPI_Init (&argc, &argv);

	if(argc != 2) {
		std::cout << "Usage: mpirun -np <np> " << argv[0] << " <config.xml>" << std::endl;
		exit(0);
	}

	// Initialize Damaris
	// The servers will block on this function call. When they return,
	// they return NULL.
	// Clients will return valid Client pointers.
	std::string config(argv[1]);
	Damaris::Client* client = Damaris::Initiator::mpi_init_and_start(config,global);

	// If the server was not started in this process...
	if(not (client == NULL)) {
		// get the communicator that has been build by Damaris
		// to communicate between clients
		comm = client->mpi_get_client_comm();
		// get ranks and size.
		MPI_Comm_rank(comm,&rank);
		MPI_Comm_size(comm,&size);

		// This program only works for 5 processes (4 clients).
		if(size != 4) {
			std::cerr << "This program is an example working only with 4 clients.\n";
			client->kill_server();
			delete client;
			MPI_Finalize();
			return 0;
		}

		client->get_parameter("h",&HEIGHT,sizeof(int));
		client->get_parameter("w",&WIDTH,sizeof(int));

		char* fractal = new char[WIDTH*HEIGHT/4];
		std::complex<double> c;

		int offset_x = WIDTH*(rank%2);
		int offset_y = HEIGHT*(rank/2);
	
		for(int i = 0; i < ITERATIONS ; i++) {
			c = std::polar<double>(0.3,i*2.0*PI/((float)ITERATIONS)-PI/2.0);
			c += std::complex<double>(0.0,-0.3);
			compute(fractal,c,offset_x,offset_y);
			
			MPI_Barrier(comm);
			client->write("images/julia",fractal);
			client->signal("say_hello_from_cpp");
			client->signal("draw_from_python");
			client->signal("clean_from_python");
			client->end_iteration();
		}
	
		client->kill_server();
	
		delete client;
		delete fractal;
	}

	MPI_Finalize();
	return 0;
}


