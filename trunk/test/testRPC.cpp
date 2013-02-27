#include "comm/MPILayer.hpp"
#include "comm/CollectiveRPC.hpp"

#define RPC_F 1
#define RPC_G 2

typedef void (*rpc_method)(void);

void f(void) {
	std::cout << "inside F\n";
}

void g(void) {
	std::cout << "inside G\n";
}

int main(int argc, char** argv)
{
	MPI_Init(&argc,&argv);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	Damaris::MPILayer<int>* layer = Damaris::MPILayer<int>::New(MPI_COMM_WORLD);
	Damaris::CollectiveRPC<rpc_method>* crpc = Damaris::CollectiveRPC<rpc_method>::New(layer);

	crpc->RegisterMulti(f, RPC_F);
	crpc->RegisterCollective(g, RPC_G);

	crpc->Call(RPC_G);	
	if(rank == 0) {
		crpc->Call(RPC_F);
	}

	while(true) {
		std::cout << ".\n";
		crpc->Update();
		sleep(1);
	}

	Damaris::CollectiveRPC<rpc_method>::Delete(crpc);
	Damaris::MPILayer<int>::Delete(layer);

	MPI_Finalize();
	return 0;
}
