#include <iostream>
#include <string>
#include <complex>

#include "include/Damaris.hpp"

#define PI 3.14159265358979323846
#define ITERATIONS 10

#define WIDTH  640
#define HEIGHT 480

Damaris::Client* client;

char julia(std::complex<double> c, std::complex<double> z0) {
	std::complex<double> zi = z0;
	for(int i = 0; i < 256; i++) {
		zi = (zi*zi) + c;
		if(std::abs<double>(zi) >= 4.0) return (char)i;
	}
	return 255;
}

int compute(char* data, std::complex<double> c) {
	for(int i=0; i < WIDTH; i++) {
	for(int j=0; j < HEIGHT; j++) {
		float x = ((float)(i-WIDTH/2))/((float)WIDTH);
		float y = ((float)(j-HEIGHT/2))/((float)HEIGHT);
		data[i*HEIGHT+j] = julia(c,std::complex<double>(x,y));
	}
	}
	return 0;
}

int main(int argc, char** argv) 
{
	int id = 0;
	char* fractal = NULL;

	if(argc != 2) {
		std::cout << "Usage: ./test <config.xml>" << std::endl;
		exit(0);
	}

	std::string config(argv[1]);
	client = new Damaris::Client(config,id);
	
	std::complex<double> c(0.0,0.0);

	for(int i = 0; i < ITERATIONS ; i++) {
		fractal = (char*)client->alloc("images/julia",i);
		if(fractal == NULL) {
			std::cout << "An error occured while allocating buffer\n";
			break;
		}

		c = std::polar<double>(i*2.0*PI/((float)ITERATIONS)-PI/2.0,0.5);
		c += std::complex<double>(0.0,0.5);
		compute(fractal,c);
		
		client->commit("images/julia",i);
		client->signal("draw",i);
	}

	client->kill_server();
	delete client;

	return 0;
}


