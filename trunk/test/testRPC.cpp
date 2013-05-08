#include "comm/MPILayer.hpp"
#include "comm/CollectiveRPC.hpp"

#define RPC_F 1
#define RPC_G 2

typedef void (*rpc_method)(int);

void f(int i) {
	std::cout << "inside F\n";
}

void g(int i) {
	std::cout << "inside G\n";
}

int main(int argc, char** argv)
{
	MPI_Init(&argc,&argv);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	Damaris::MPILayer<Damaris::CollectiveRPC<rpc_method>::rpc_msg>* layer 
		= Damaris::MPILayer<Damaris::CollectiveRPC<rpc_method>::rpc_msg>::New(MPI_COMM_WORLD);
	Damaris::CollectiveRPC<rpc_method>* crpc = Damaris::CollectiveRPC<rpc_method>::New(layer);

	crpc->RegisterMulti(f,0, RPC_F);
	crpc->RegisterCollective(g,0, RPC_G);

	crpc->Call(0,0,RPC_G);	
	if(rank == 0) {
		crpc->Call(0,0,RPC_F);
	}

	while(true) {
		std::cout << ".\n";
		crpc->Update();
		sleep(1);
	}

	Damaris::CollectiveRPC<rpc_method>::Delete(crpc);
	Damaris::MPILayer<Damaris::CollectiveRPC<rpc_method>::rpc_msg>::Delete(layer);

	MPI_Finalize();
	return 0;
}
