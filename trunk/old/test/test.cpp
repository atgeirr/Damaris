#include <iostream>
#include <string>
#include <complex>

#include "common/Debug.hpp"
#include "client/Client.hpp"

#define PI 3.14159265358979323846
#define ITERATIONS 10

Damaris::Client* client;

char julia(std::complex<double> c, std::complex<double> z0) {
	std::complex<double> zi = z0;
	for(int i = 0; i < 256; i++) {
		zi = (zi*zi) + c;
		if(std::abs<double>(zi) >= 4.0) return (char)i;
	}
	return 255;
}

int compute(char* data, int w, int h, std::complex<double> c) {
	TIMER_START(computation);
	for(int i=0; i < w; i++) {
	for(int j=0; j < h; j++) {
		float x = ((float)(i-w/2))/((float)w);
		float y = ((float)(j-h/2))/((float)h);
		data[i*h+j] = julia(c,std::complex<double>(x,y));
	}
	}
	TIMER_STOP(computation,"computation done");
	return 0;
}

int main(int argc, char** argv) 
{
	int id = 0;
	char* fractal = new char[640*480];

	if(argc != 2) {
		std::cout << "Usage: ./test <config.xml>" << std::endl;
		exit(0);
	}

	std::string config(argv[1]);
	client = new Damaris::Client(config,id);

	std::complex<double> c(0.0,0.0);

	for(int i = 0; i < ITERATIONS ; i++) {
		c = std::polar<double>(i*2.0*PI/((float)ITERATIONS)-PI/2.0,0.5);
		c += std::complex<double>(0.0,0.5);
		compute(fractal,640,480,c);
		
		TIMER_START(write);
		client->write("images/julia",i,fractal);
		client->signal("draw",i);
		TIMER_STOP(write,"write done");
	}

	client->killServer();
	delete fractal;
	delete client;

	return 0;
}


