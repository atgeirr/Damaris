#include <iostream>
#include "Model.hpp"
#include "BcastXML.hpp"

using namespace std;
using namespace Damaris::Model;

int main (int argc, char* argv[])
{
	MPI_Init(&argc,&argv);
	auto_ptr<Simulation> sim = BcastXML(MPI_COMM_WORLD,argv[1]);
	std::cerr << sim->name() << std::endl;
	MPI_Finalize();
}
