#include <iostream>
#include <string>
#include <complex>
#include <mpi.h>

#include "include/Damaris.hpp"

#define PI 3.14159265358979323846
#define ITERATIONS 100

#define WIDTH  640
#define HEIGHT 480

char julia(std::complex<double> c, std::complex<double> z0) {
	std::complex<double> zi = z0;
	for(int i = 0; i < 256; i++) {
		zi = (zi*zi) + c;
		if(std::abs<double>(zi) >= 4.0) return (char)i;
	}
	return 255;
}

int compute(char* data, std::complex<double> c, int offset_x, int offset_y) {
	for(int i=0; i < WIDTH/2; i++) {
	for(int j=0; j < HEIGHT/2; j++) {
		float x = ((float)(offset_x+i-WIDTH/2))/((float)WIDTH);
		float y = ((float)(offset_y+j-HEIGHT/2))/((float)HEIGHT);
		data[i*HEIGHT/2+j] = julia(c,std::complex<double>(x,y));
	}
	}
	return 0;
}

int main(int argc, char** argv) 
{
	int rank, size;
	MPI_Comm global = MPI_COMM_WORLD;
        MPI_Comm comm;

        MPI_Init (&argc, &argv);

	if(argc != 2) {
		std::cout << "Usage: mpirun -np <np> ./test <config.xml>" << std::endl;
		exit(0);
	}

	std::string config(argv[1]);

	Damaris::Client* client = Damaris::Initiator::start(config,global);

	if(client != NULL) {
		comm = client->get_clients_communicator();
		MPI_Comm_rank(comm,&rank);
		MPI_Comm_size(comm,&size);

		if(size != 4) {
			std::cout << "This program is an example working only with 4 clients.\n";
			client->kill_server();
			delete client;
			MPI_Finalize();
			return 0;
		}

		char* fractal = new char[WIDTH*HEIGHT/4];
		std::complex<double> c;
	
		std::vector<int> start(2); 
			start[0] = (rank/2)*WIDTH/2;
			start[1] = (rank%2)*HEIGHT/2;
		std::vector<int> end(2); 
			end[0] = start[0] + WIDTH/2 - 1;
			end[1] = start[1] + HEIGHT/2 - 1;
	
		Damaris::chunk_h chunk_handle = client->chunk_set(2, start, end);
	
		for(int i = 0; i < ITERATIONS ; i++) {
			c = std::polar<double>(i*2.0*PI/((float)ITERATIONS)-PI/2.0,0.5);
			c += std::complex<double>(0.0,0.5);
			compute(fractal,c,start[0],start[1]);
			
			MPI_Barrier(comm);
			client->chunk_write(chunk_handle,"images/julia",i,fractal);
			client->signal("draw",i);
		}
	
		client->chunk_free(chunk_handle);
	
		client->kill_server();
	
		delete client;
		delete fractal;
	}

	MPI_Finalize();
	return 0;
}


