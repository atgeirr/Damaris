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

#include <set>

#include "util/Unused.hpp"
#include "util/Pointers.hpp"
#include "msg/Signals.hpp"
#include "env/Environment.hpp"
#include "action/ActionManager.hpp"
#include "action/ScriptManager.hpp"
#include "data/VariableManager.hpp"
#include "storage/StorageManager.hpp"
#include "server/Server.hpp"

#ifdef HAVE_VISIT_ENABLED
#include "visit/VisItListener.hpp"
#endif

#ifdef HAVE_PARAVIEW_ENABLED
#include "paraview/ParaViewAdaptor.hpp"
#endif

#ifdef HAVE_PYTHON_ENABLED
// #include "scripts/.hpp"
#endif

enum {
    BCAST_EVENT,
    VISIT_CONNECTED,
    ITERATION_SYNC    
};

namespace damaris {


/**
* Adds a Channel associated to a client, binds tags to
* callbacks and starts listening to it asynchronously.
*/
bool Server::AddChannelToClient(const std::shared_ptr<Channel>& ch) {
    int rk = ch->GetEndPoint();
    if(clients_.count(rk) != 0 || not ch) return false;
    clients_[rk] = ch;
    return true;
}

void Server::Run() {
    
    needStop_ = false;

    // if it's the first call to Run, install other callbacks
    if(firstRun_) {
        std::map<int,std::shared_ptr<Channel> >::iterator ch;
        for(ch = clients_.begin(); ch != clients_.end(); ch++) {
            std::shared_ptr<Channel> c = ch->second;
            headerMsg_[ch->first] = HeaderMessage();
            
            c->AsyncRecv(DAMARIS_SIG_HEADER,
                &(headerMsg_[ch->first]),
                sizeof(HeaderMessage),
                BIND(&Server::OnHeader,this,_1,_2,_3,_4));
        }
        
#ifdef HAVE_VISIT_ENABLED
        
        if(Environment::GetModel()->visit().present()) {

            void (*f)(int, int, const void*, int) 
                = &VisItListener::EnterSyncSection;
            reactor_->Bind(VISIT_CONNECTED,f);

            VisItListener::Init(Environment::GetEntityComm(),
                Environment::GetModel()->visit(),
                Environment::GetSimulationName());
        }
#endif

#ifdef HAVE_PARAVIEW_ENABLED
        if (Environment::GetModel()->paraview().present()) {
            std::shared_ptr<ParaViewAdaptor> paraview = ParaViewAdaptor::GetInstance();
            paraview->Initialize(Environment::GetEntityComm() ,
                                 Environment::GetModel()->paraview() ,
                                 Environment::GetSimulationName());
        }
        else
            ERROR("Damaris is compiled with ParaView (vPARAVIEW_VERSION), but ParaView is not enabled in the XML configuration file. ");
#endif

        reactor_->Bind(ITERATION_SYNC, 
            BIND(&Server::EndOfIterationCallback,this,_1,_2,_3,_4));
        reactor_->Bind(BCAST_EVENT, &Server::BcastEventCallback);
    }
    
#ifdef HAVE_VISIT_ENABLED
    if(Environment::GetModel()->visit().present()) {
        
        while(not needStop_) {
            // try receiving from VisIt (only for rank 0)
            if(Environment::GetEntityProcessID() == 0) {
                if(VisItListener::Connected()) {
                    visitIteration_  = Environment::GetLastIteration();
                    
                    reactor_->Broadcast(VISIT_CONNECTED,
                        &visitIteration_,
                        sizeof(visitIteration_));
                }
            }
            reactor_->PollSome(100);
        }
        
    } else {
        reactor_->Run();
    }
#else
    reactor_->Run();
#endif
    firstRun_ = false;
}

void Server::EndOfIterationCallback(int tag, int source,
    const void* buf, int count) 
{
    static double t_fin = 0.0 ;
    double t_start,  t_server_run, t_server_free ;
    
    int iteration = Environment::GetLastIteration()-1;
    t_start = MPI_Wtime();
    
    int server_pid = Environment::GetEntityProcessID() ;
    
    if (t_fin == 0.0) {
        t_server_free = 0.0 ;     
        Environment::Log("Server::EndOfIterationCallback method started.)", EventLogger::Info);
        Environment::Log("N.B. Server free-time (t_server_free) is the time in seconds that the server core had free (= time at exit of routine - time the successive iteration starts)", EventLogger::Info);
        Environment::Log("N.B. Server run-time (t_server_run) is the time in seconds that the server core took to process I/O routines (= time at exit of routine - time at start of routine)", EventLogger::Info);
        Environment::Log("TIME: server_pid, iteration , t_server_run, t_server_free", EventLogger::Info);
    } else {
        t_server_free =  t_start - t_fin ;
        Environment::Log("Server::EndOfIterationCallback method started.", EventLogger::Info);
    }
     
    
    // sync finished for this iteration, starts listening
    // to clients again
    std::map<int,std::shared_ptr<Channel> >::iterator ch;
    for(ch = clients_.begin(); ch != clients_.end(); ch++) {
        std::shared_ptr<Channel> c = ch->second;
        headerMsg_[ch->first] = HeaderMessage();

        c->AsyncRecv(DAMARIS_SIG_HEADER,
                &(headerMsg_[ch->first]),
                sizeof(HeaderMessage),
                BIND(&Server::OnHeader,this,_1,_2,_3,_4));
    }

   

#ifdef HAVE_VISIT_ENABLED
    if(Environment::GetModel()->visit().present()) {
        int frequency = VisItListener::UpdateFrequency();
        if((frequency > 0) && (iteration % frequency == 0)) {
            VisItListener::Update();
        }
    }
#endif

#ifdef HAVE_PARAVIEW_ENABLED
    if (Environment::GetModel()->paraview().present()) {
        //if (iteration > 0) {
            std::shared_ptr<ParaViewAdaptor> paraview = ParaViewAdaptor::GetInstance();
            paraview->CoProcess(iteration);
        //}
    }
#endif

    StorageManager::Update(iteration);
    
#ifdef HAVE_PYTHON_ENABLED
    // N.B. ActionManager::RunActions(iteration); will run the same code as ScriptManager::RunScripts(iteration)
    //std::cout <<"INFO: Server.cpp HAVE_PYTHON_ENABLED is defined" << std::endl ;
    ScriptManager::RunScripts(iteration);
#else
   // std::cout <<"INFO: Server.cpp HAVE_PYTHON_ENABLED is NOT defined" << std::endl ;
#endif
    
    // N.B. ActionManager::RunActions() *first* calls the garbage collector on block data of all variables
    // And then it calls other actions (such as PyAction) which then have no block data.
    // This is a 
    ActionManager::RunActions(iteration);
    

    
    t_fin = MPI_Wtime();
    t_server_run = t_fin - t_start;
    Environment::Log("Server::EndOfIterationCallback method finished.", EventLogger::Info);
    Environment::Log("TIME:  " + std::to_string(server_pid) + ", "  + std::to_string(iteration) + ", " +std::to_string(t_server_run)+ ", " +std::to_string(t_server_free), EventLogger::Info);
    if (Environment::GetModel()->log().present()) {
        if (Environment::GetModel()->log().get().Flush()) {
            Environment::FlushLog();
        }
    }
    
    

}

void Server::OnHeader(int UNUSED(tag), int rk,
    const void* data, int UNUSED(s)) 
{
    std::shared_ptr<Channel> ch = clients_[rk];
    
    const HeaderMessage* h = (HeaderMessage*)data;
    SignalType type = h->type_;
    int source = h->source_;
    

    switch(type) {
        
    case DAMARIS_SIG_CONNECT:
        OnConnect(source); break;
    
    case DAMARIS_SIG_STOP:
        OnStop(source); break;
    
    case DAMARIS_SIG_NEXT_ITERATION:
        OnNextIteration(source); break;
    
    case DAMARIS_SIG_NEXT_ITERATION_ERR:
        OnNextIterationWithErrors(source); break;
    
    case DAMARIS_SIG_CALL_ACTION:
        OnCallAction(ch,source); break;

    case DAMARIS_SIG_BCAST_ACTION:
        OnBcastAction(ch,source); break;

    case DAMARIS_SIG_WRITE:
        OnWrite(ch,source); break;

    //orc:remote write added for dedicated nodes
    case DAMARIS_SIG_REMOTE_WRITE:
         OnRemoteWrite(ch,source);break;

    case DAMARIS_SIG_COMMIT:
        OnCommit(ch,source); break;
    
    case DAMARIS_SIG_CLEAR:
        OnClear(ch,source); break;
    
    }

    // if the message was an end iteration,
    // we need to wait for the Sync phase to
    // complete before restarting to listen to clients
    if(type != DAMARIS_SIG_NEXT_ITERATION) {    
        ch->AsyncRecv(DAMARIS_SIG_HEADER,
                &(headerMsg_[rk]),
                sizeof(HeaderMessage),
                BIND(&Server::OnHeader,this,_1,_2,_3,_4));
    }
}

void Server::OnConnect(int source)
{
    Environment::AddConnectedClient(source);
}

void Server::OnStop(int source)
{
    static std::set<int> stopRequests;
    stopRequests.insert(source);
    
    if((int)stopRequests.size() == 
        (int)(Environment::ClientsPerNode()/Environment::ServersPerNode())) 
    {
        Stop();
        stopRequests.clear();

#ifdef HAVE_PARAVIEW_ENABLED
        if (Environment::GetModel()->paraview().present()) {
            std::shared_ptr<ParaViewAdaptor> paraview = ParaViewAdaptor::GetInstance();
            paraview->Finalize();
        }

#endif
    }
}

void Server::OnNextIteration(int UNUSED(source)) 
{
    if(Environment::StartNextIteration()) {
        reactor_->Sync(ITERATION_SYNC);
    }
}

void Server::OnNextIterationWithErrors(int source) 
{
    INFO("Error in next iteration, cleaning");
    if(Environment::StartNextIteration()) {
        std::shared_ptr<Action> gc = ActionManager::Search("#error");
        if(gc) (*gc)(source,Environment::GetLastIteration()-1);
    }
}

void Server::OnCallAction(const std::shared_ptr<Channel>& ch, int UNUSED(source)) 
{
    EventMessage ev;
    ch->Recv(DAMARIS_SIG_BODY,&ev,sizeof(ev));
    std::shared_ptr<Action> a = ActionManager::Search(ev.id_);
    if(a) (*a)(ev.source_,ev.iteration_);
}

void Server::OnBcastAction(const std::shared_ptr<Channel>& ch, int UNUSED(source))
{
    EventMessage ev;
    ch->Recv(DAMARIS_SIG_BODY,&ev,sizeof(ev));
    std::shared_ptr<Action> a = ActionManager::Search(ev.id_);
    if(a) reactor_->Broadcast(BCAST_EVENT,&ev,sizeof(ev));
}

void Server::OnWrite(const std::shared_ptr<Channel>& ch, int source) 
{
    WriteMessage wr;
    ch->Recv(DAMARIS_SIG_BODY,&wr,sizeof(wr));
    std::shared_ptr<Variable> v = VariableManager::Search(wr.id_);
    if(not v) {
        ERROR("Client " << ch->GetEndPoint() << " tried to write "
        << "an unknown variable (bad id).");
        return;
    }
    
    int iteration = wr.iteration_;
    int bid = wr.block_;
    Handle h = wr.handle_;
    std::shared_ptr<Block> b = v->Retrieve(source, iteration, bid, 
            std::vector<int64_t>(wr.lbounds_,wr.lbounds_+wr.dim_),
            std::vector<int64_t>(wr.ubounds_,wr.ubounds_+wr.dim_),
            std::vector<int64_t>(wr.gbounds_,wr.gbounds_+wr.dim_),
            std::vector<size_t>(wr.ghosts_,wr.ghosts_+(2*wr.dim_)),
            h);
    if(b) { 
        b->GainDataOwnership(); 
    } else {
        ERROR("Failed at retrieving data written by client.");
    }
}
//orc:Retrieval is not possible since different node, allocate a new block for written variable and receive it via MPI.
void Server::OnRemoteWrite(const std::shared_ptr<Channel>& ch, int source)
{
    RemoteWriteMessage rwm;
    ch->Recv(DAMARIS_SIG_BODY,&rwm,sizeof(rwm));

    std::shared_ptr<Variable> v = VariableManager::Search(rwm.id_);
    if(not v) {
        ERROR("Client " << ch->GetEndPoint() << " tried to write "
        << "an unknown variable (bad id).");
        return;
    }
    int iteration = rwm.iteration_;
    int block = rwm.block_;
    bool blocking = true ;
    std::shared_ptr<Block> b 
        = v->AllocateFixedSize(source, iteration, block,
                std::vector<int64_t>(rwm.lbounds_,rwm.lbounds_+rwm.dim_), 
                std::vector<int64_t>(rwm.ubounds_,rwm.ubounds_+rwm.dim_), 
                std::vector<int64_t>(rwm.gbounds_,rwm.gbounds_+rwm.dim_),
                std::vector<size_t>(rwm.ghosts_,rwm.ghosts_+(2*rwm.dim_)),
                blocking);

    if(not b) {
        //errorOccured_ = 1;
        ERROR("Could not allocated block for variable on dedicated node "<< v->GetName());
        return ;
    }

    DataSpace<Buffer> ds = b->GetDataSpace();

    void* buffer = ds.GetData();
    if(buffer == NULL) {
        ERROR("Invalid dataspace on dedicated node side");
        return;// DAMARIS_DATASPACE_ERROR;
    }
    //size will be equal to rwm.size_ since we already allocated that much
    //see AllocateFixedSize above.
    size_t size = ds.GetSize();

    ch->Recv(DAMARIS_SIG_DATA,buffer,size);

}
void Server::OnCommit(const std::shared_ptr<Channel>& ch, int source) 
{
    CommitMessage wr;
    ch->Recv(DAMARIS_SIG_BODY,&wr,sizeof(wr));
    std::shared_ptr<Variable> v = VariableManager::Search(wr.id_);
    if(not v) {
        ERROR("Client " << ch->GetEndPoint() << " tried to commit "
        << "an unknown variable (bad id).");
        return;
    }
    
    int iteration = wr.iteration_;
    int bid = wr.block_;
    Handle h = wr.handle_;
    std::shared_ptr<Block> b = v->Retrieve(source, iteration, bid, 
            std::vector<int64_t>(wr.lbounds_,wr.lbounds_+wr.dim_),
            std::vector<int64_t>(wr.ubounds_,wr.ubounds_+wr.dim_),
            std::vector<int64_t>(wr.gbounds_,wr.gbounds_+wr.dim_),
            std::vector<size_t>(wr.ghosts_,wr.ghosts_+(2*wr.dim_)),
            h);
    
    if(not b) {
        ERROR("Failed at retrieving data committed by client.");
    }
    
    b->SetReadOnly(true);
}

void Server::OnClear(const std::shared_ptr<Channel>& ch, int source) 
{
    ClearMessage m;
    ch->Recv(DAMARIS_SIG_BODY,&m,sizeof(m));
    std::shared_ptr<Variable> v = VariableManager::Search(m.id_);
    if(not v) {
        ERROR("Client " << ch->GetEndPoint() << " tried to clear "
        << "an unknown variable (bad id).");
        return;
    }
    
    int iteration = m.iteration_;
    int bid = m.block_;
    std::shared_ptr<Block> b = v->GetBlock(source, iteration, bid);
    
    if(not b) {
        ERROR("Failed at retrieving block cleared by client.");
    }
    
    b->SetReadOnly(false);
    b->GainDataOwnership();
}

void Server::BcastEventCallback(int tag, int source, const void* buf, int count)
{
    const EventMessage* ev = (const EventMessage*)buf;
        std::shared_ptr<Action> a = ActionManager::Search(ev->id_);
        if(a) (*a)(ev->source_,ev->iteration_);    
}

} 
