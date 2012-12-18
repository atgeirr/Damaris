/* ================================================================================
 * This test is part of the Damaris program.
 * Usage with a dedicated core (space-partitioning):
 * 1) Start the server using
 * 		export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
 *		../server/server --configuration=julia.xml &> server_log.txt &
 * 2) Start the program using
 *		./julia julia.xml
 * This program should generate 100 png images of Julia sets, then exit.
 * The server is killed automatically by the client, except if you kill the client
 * before it ends. In this case, find the pid of the server using ps aux then kill
 * it manually.
 * ================================================================================ */

#include <iostream>
#include <string>
#include <complex>

#include "include/Damaris.hpp"

#define PI 3.14159265358979323846
#define ITERATIONS 100

#define WIDTH  640
#define HEIGHT 480

Damaris::Client* client;

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

// This function call the julia function for all pixels in the image.
int compute(char* data, std::complex<double> c) {
	for(int i=0; i < WIDTH; i++) {
	for(int j=0; j < HEIGHT; j++) {
		float x = ((float)(i-WIDTH/2)*2.2)/((float)WIDTH);
		float y = ((float)(j-HEIGHT/2)*2.2)/((float)HEIGHT);
		data[i*HEIGHT+j] = julia(c,std::complex<double>(x,y));
	}
	}
	return 0;
}

// Main function
int main(int argc, char** argv) 
{
	int id = 0;
	char* fractal = new char[WIDTH*HEIGHT];

	if(argc != 2) {
		std::cout << "Usage: " << argv[0] << " <config.xml>" << std::endl;
		exit(0);
	}

	// Initializes the client
	std::string config(argv[1]);
	std::auto_ptr<Damaris::Model::Simulation> mdl
		= Damaris::Model::BcastXML(MPI_COMM_WORLD,config);	
	client = Damaris::Client::New(mdl,id);
	// Note: the use of Client::New assumes that a server is already running.
	// Thus it cannot work in a time-partitioning mode. You may take a look at
	// the MPI version of this program to see how to properly integrate a client
	// in a MPI application and allow time-partitioning.

	std::complex<double> c(0.0,0.0);

	for(int i = 0; i < ITERATIONS ; i++) {
		c = std::polar<double>(0.3,i*2.0*PI/((float)ITERATIONS)-PI/2.0);
		c += std::complex<double>(0.0,-0.3);
		compute(fractal,c);

		// Writes the array in shared memory and notifies Damaris		
		client->write("images/julia",fractal);
		// Sends some events
		client->signal("say_hello_from_cpp");
		client->signal("draw_from_python");
		client->signal("clean_from_python");

		client->end_iteration();
	}

	// Request the server to exit in a clean way
	client->kill_server();
	delete fractal;
	delete client;

	return 0;
}
