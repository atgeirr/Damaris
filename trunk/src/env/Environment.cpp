/**************************************************************************
This file is part of Damaris.

Damaris is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Damaris is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Damaris.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

#include "util/Debug.hpp"
#include "buffer/SharedMemorySegment.hpp"
#include "buffer/LocalMemorySegment.hpp"
#include "buffer/BufferManager.hpp"
#include "env/Environment.hpp"
#include "env/Hardware.hpp"
#include "model/BcastXML.hpp"
#include "data/ParameterManager.hpp"
#include "data/LayoutManager.hpp"
#include "data/VariableManager.hpp"
#include "data/MeshManager.hpp"
#include "data/CurveManager.hpp"
#include "action/ActionManager.hpp"
#include "client/StandaloneClient.hpp"
#include "client/RemoteClient.hpp"

namespace damaris {

bool				Environment::_initialized_;
shared_ptr<model::Simulation> 	Environment::_baseModel_;
MPI_Comm 			Environment::_entityComm_;
MPI_Comm 			Environment::_globalComm_;
MPI_Comm 			Environment::_nodeComm_;
std::list<int> 			Environment::_knownClients_;
int 				Environment::_lastIteration_;
bool 				Environment::_isClient_;
bool				Environment::_isDedicatedCore_;
bool				Environment::_isDedicatedNode_;
int 				Environment::_globalProcessID_;
int				Environment::_entityProcessID_;
int				Environment::_clientsPerNode_;
int				Environment::_coresPerNode_;
int 				Environment::_serversPerNode_;
std::string 			Environment::_magicNumber_;
shared_ptr<Buffer> 		Environment::_defaultBuffer_;
shared_ptr<Client>		Environment::_client_;
shared_ptr<Server>		Environment::_server_;
bool				Environment::_sharedStructuresOwner_;

bool Environment::Init(const std::string& configFile,
			MPI_Comm world) {

	if(_initialized_) return false;

	int size, rank;
	MPI_Comm_size(world,&size);
	MPI_Comm_rank(world,&rank);

	_baseModel_ = model::BcastXML(world,configFile);
	_globalComm_ = world;
	_lastIteration_ = 0;
	_globalProcessID_ = rank;
	_initialized_ = true;

	/* Compute the magic number of the simulation */
	time_t mgnbr;
	time(&mgnbr);
	MPI_Bcast(&mgnbr,sizeof(time_t),MPI_BYTE,0,_globalComm_);
	std::ostringstream oss;
	oss << mgnbr;
	_magicNumber_ = oss.str();

	/* Create a new communicator gathering processes of the same node */
	int node_id = Hardware::GetNodeID();
	MPI_Comm_split(_globalComm_,node_id,rank,&_nodeComm_);

	/* Create a global communicator with ordered ranks */
	MPI_Comm orderedComm;
	MPI_Comm_split(_globalComm_,0,node_id,&orderedComm);
	_globalComm_ = orderedComm;

	/* Get the size of the node */
	MPI_Comm_size(_nodeComm_,&_coresPerNode_);

	/* If there are dedicated nodes */
	if(_baseModel_->architecture().dedicated().nodes() > 0) {
		/* There are dedicated nodes */
		return InitDedicatedNodes();
	} else if(_baseModel_->architecture().dedicated().cores() > 0) {
		return InitDedicatedCores(_globalComm_);
	} else {
		return InitStandalone(_globalComm_);
	}
}

bool Environment::InitDedicatedCores(MPI_Comm global)
{
	// NOTE: in the future we may call this function from InitDedicatedNodes
	// and pass a communicator that is different from _globalComm_;

	/* Get the size and rank in the node */
	int rankInNode;
	MPI_Comm_rank(_nodeComm_,&rankInNode);

	int dcpn = _baseModel_->architecture().dedicated().cores();

	_isDedicatedNode_ = false;

	_clientsPerNode_ = _coresPerNode_ - dcpn;
	//orc:added for compatibility with dedicated nodes
	_serversPerNode_ =  dcpn ;
	/* Check that there are enough cores remaining for clients */
	if(_clientsPerNode_ <= 0) {
		ERROR("Cannot place clients on nodes: all cores are dedicated!");
		MPI_Abort(MPI_COMM_WORLD,-1);
	}

	/* Check that the number of dedicated cores divides the number of
	   clients per node */
	if(not (_clientsPerNode_ % dcpn == 0)) {
		ERROR("The number of dedicated cores in a node does not divide"
		<< " the number of clients, aborting.");
		MPI_Abort(MPI_COMM_WORLD,-1);
	}

	/* Compute the communicator for clients and servers */
	_isClient_ = (rankInNode >= _clientsPerNode_) ? 0 : 1;
	int rank;
	MPI_Comm_rank(global,&rank);
	MPI_Comm_split(global,_isClient_,rank,&_entityComm_);
	
	/* Get rank and size in the entity communicator */
	MPI_Comm_rank(_entityComm_,&_entityProcessID_);

	/* check if the buffer is enabled. Otherwise we will use a RemoteClient */
	bool bufEnabled = _baseModel_->architecture().buffer().enabled();

	// dedicated core mode : the number of servers to create 
	// is strictly positive
	if(_isClient_) {
		// the following barrier ensures that the client 
		// won't be created before the servers are started. 
		MPI_Barrier(_globalComm_); 

		if(bufEnabled) OpenSharedStructures();
		else CreateLocalStructures();

		InitManagers();

		if(bufEnabled) _client_ = Client::New(_entityComm_);
		else _client_ = RemoteClient::New(_entityComm_);

		shared_ptr<Reactor> reactor = _client_->GetReactor();
		int servID = _clientsPerNode_ + (rankInNode/(_clientsPerNode_/dcpn));
		shared_ptr<Channel> channel2server 
			= Channel::New(reactor,_nodeComm_,servID,
				_baseModel_->architecture().queue().size());
		_client_->SetChannelToServer(channel2server);
		_client_->Connect();
	} else {
		if(bufEnabled) {
			if(rankInNode == _clientsPerNode_) { // first server on node only
				CreateSharedStructures();
				MPI_Barrier(_entityComm_);
			} else {
				MPI_Barrier(_entityComm_);
				OpenSharedStructures();
			}
		} else {
			CreateLocalStructures();
		}

		InitManagers();
		_server_ = Server::New(_entityComm_);
		shared_ptr<Reactor> reactor = _server_->GetReactor();
		for(int k=0; k < (_clientsPerNode_/dcpn); k++) {
			int cID = (rankInNode-_clientsPerNode_)*(_clientsPerNode_/dcpn) + k;
			shared_ptr<Channel> channel2client
				= Channel::New(reactor,_nodeComm_,cID,
				_baseModel_->architecture().queue().size());
			_server_->AddChannelToClient(channel2client);
		}
		_isDedicatedCore_ = true;
		MPI_Barrier(_globalComm_);
	}

	_initialized_ = true;
	return true;
}

bool Environment::InitDedicatedNodes()
{
	/* Get the number of dedicated nodes */
	int dn = _baseModel_->architecture().dedicated().nodes();
	// Get the size of the global communicator and the rank of this process
	int rank, size;
	MPI_Comm_rank(_globalComm_,&rank);
	MPI_Comm_size(_globalComm_,&size);


	/* Get the size and rank in the node */
	int rankInNode;
	MPI_Comm_rank(_nodeComm_,&rankInNode);

	// number of cores per node is _coresPerNode_, so the number of nodes is
	int nb_nodes = size/_coresPerNode_;
	//orc:mapping ratio between servers and clients:
	int ratio = (nb_nodes - dn ) / dn ;
	//orc:to make it compatible with other funcs , actually gives total compute node number
	_clientsPerNode_ = nb_nodes - dn;
	_serversPerNode_ = dn;
	// make sure we don't have only dedicated nodes
	if(nb_nodes - dn <= 0) {
		ERROR("Too many dedicated nodes, aborting.");
		MPI_Abort(MPI_COMM_WORLD,-1);
	}

	// make sure the number of dedicated nodes divides the number of client nodes
	if(not ((nb_nodes - dn) % dn == 0)) {
		ERROR("Number of dedicated nodes does not divide the"
		" number of client nodes, aborting.");
		MPI_Abort(MPI_COMM_WORLD,-1);
	}

	//orc:Assuming global comm is ordered
	_isClient_ = (rank >= (dn * _coresPerNode_ )) ? 1 : 0;

	MPI_Comm_split(_globalComm_, _isClient_,rank,&_entityComm_);
	//orc:Get entity rank
	MPI_Comm_rank(_entityComm_,&_entityProcessID_);

	if(_isClient_) {
		// the following barrier ensures that the client
		// won't be created before the servers are started.
		MPI_Barrier(_globalComm_);
		CreateLocalStructures();
		InitManagers();
		_client_ = RemoteClient::New(_entityComm_);
		shared_ptr<Reactor> reactor = _client_->GetReactor();
		//orc:client to server mapping
		int servID = (rank - (dn * _coresPerNode_ ))/ ratio;
		//orc:comm will be the global one
		shared_ptr<Channel> channel2server
			= Channel::New(reactor,_globalComm_,servID,
				_baseModel_->architecture().queue().size());
		//orc:setting channel to server and connecting
		_client_->SetChannelToServer(channel2server);
		_client_->Connect();
	} else { //orc:if server
		CreateLocalStructures();
		InitManagers();
		_server_ = Server::New(_entityComm_);
		shared_ptr<Reactor> reactor = _server_->GetReactor();
		//debugging:
                //std::cout << "Node id is " << Hardware::GetNodeID() << "rank for the server is" << rank << std::endl  ;
		//orc:server to client mapping
		for(int k=0; k < ratio; k++) {
			int cID = (rank * ratio) + k + (dn * _coresPerNode_ );

			//orc:comm will be the global one
			shared_ptr<Channel> channel2client
				= Channel::New(reactor,_globalComm_,cID,
				_baseModel_->architecture().queue().size());

			_server_->AddChannelToClient(channel2client);
		}
		_isDedicatedNode_ = true;
		MPI_Barrier(_globalComm_);
	}

	_initialized_ = true;
	return true;
}

bool Environment::InitStandalone(MPI_Comm global) 
{
	int rankInNode;
        MPI_Comm_rank(_nodeComm_,&rankInNode);

	/* Compute the communicator for clients and servers */
	_isClient_ = (rankInNode >= _clientsPerNode_) ? 0 : 1;
	int rank;
	MPI_Comm_rank(global,&rank);
	MPI_Comm_split(global,_isClient_,rank,&_entityComm_);
	/* Get rank and size in the entity communicator */
	MPI_Comm_rank(_entityComm_,&_entityProcessID_);

	// synchronous mode : the servers are attached to each client
	_entityProcessID_ = _globalProcessID_;
        //orc:added for compatibility with dedicated nodes
        _serversPerNode_ =  0 ;

	_isClient_ = true;
	CreateLocalStructures();
	InitManagers();
	_client_ = StandaloneClient::New();

	_client_->Connect();
	_initialized_   = true;
	
	return true;
}

bool Environment::CreateSharedStructures()
{
	_sharedStructuresOwner_ = true;
	if(_defaultBuffer_) {
		ERROR("Shared structures already created");
		return false;
	}
	
	try {
		SharedMemorySegment::Remove(
			_baseModel_->architecture().buffer());
		_defaultBuffer_ 
			= SharedMemorySegment::Create(
				_baseModel_->architecture().buffer());
	}
	catch(interprocess_exception &ex) {
		ERROR("While initializing shared memory objects: " 
			<< ex.what());
		exit(-1);
	}
	return true;
}

bool Environment::OpenSharedStructures()
{
	_sharedStructuresOwner_ = false;
	if(_defaultBuffer_ != NULL) {
		ERROR("Shared structures already opened");
		return false;;
	}
	try {
		_defaultBuffer_ 
			= SharedMemorySegment::Open(
				_baseModel_->architecture().buffer());
	}
	catch(interprocess_exception &ex) {
		ERROR("While initializing shared memory objects: " 
			<< ex.what());
		exit(-1);
	}
	return true;
}

bool Environment::CreateLocalStructures()
{
	_defaultBuffer_
		= LocalMemorySegment::Create(
				_baseModel_->architecture().buffer());
	_sharedStructuresOwner_ = false;
	return true;
}

bool Environment::InitManagers()
{
	ParameterManager::Init(_baseModel_->data());
	LayoutManager::Init(_baseModel_->data());
	VariableManager::Init(_baseModel_->data());
	MeshManager::Init(_baseModel_->data());
	CurveManager::Init(_baseModel_->data());
	ActionManager::Init(_baseModel_->actions());
	return true;
}

bool Environment::Finalize()
{
	if(not _initialized_) return -1;
	
	MeshManager::DeleteAll();
	VariableManager::DeleteAll();
	LayoutManager::DeleteAll();
	ParameterManager::DeleteAll();
	
	if(_defaultBuffer_ && _sharedStructuresOwner_) {
		SharedMemorySegment::Remove(
				_baseModel_->architecture().buffer());
	}
	_defaultBuffer_.reset();
	
	BufferManager::DeleteAll();
	
	_baseModel_.reset();
	_initialized_ = false;
	_magicNumber_ = "";
	_knownClients_.clear();
	return true;
}

void Environment::AddConnectedClient(int id)
{
	if(not _initialized_) return;
	std::list<int>::iterator it = _knownClients_.begin();
	while(it != _knownClients_.end() && (*it) != id) {
		it++;
	}
	if(it == _knownClients_.end()) {
		_knownClients_.push_back(id);
	}
}

bool Environment::StartNextIteration()
{
	
	if(not _initialized_) return false;
	// for clients
	if((not HasServer()) || (IsClient())) {
		_lastIteration_++;
		return true;
	}

	// for servers

	static int locks = 0;
	locks += 1;
	if(locks == (ClientsPerNode()/ServersPerNode())) { 
		_lastIteration_++;
		locks = 0;
		return true;
	}
	return false;
}
}