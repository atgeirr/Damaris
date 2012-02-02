#include <iostream>
#include <string>
#include <complex>

#include "include/Damaris.hpp"

#define PI 3.14159265358979323846
#define ITERATIONS 100

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
	char* fractal = new char[640*480];

	if(argc != 2) {
		std::cout << "Usage: ./test <config.xml>" << std::endl;
		exit(0);
	}

	std::string config(argv[1]);
	client = Damaris::Client::New(config,id);

	std::complex<double> c(0.0,0.0);

	for(int i = 0; i < ITERATIONS ; i++) {
		c = std::polar<double>(i*2.0*PI/((float)ITERATIONS)-PI/2.0,0.5);
		c += std::complex<double>(0.0,0.5);
		compute(fractal,c);
		
		client->write("images/julia",i,fractal);
		client->signal("call_python",i);
	}

	client->kill_server();
	delete fractal;
	delete client;

	return 0;
}


