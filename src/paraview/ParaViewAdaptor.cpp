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
#include "paraview/ParaViewAdaptor.hpp"
#include "damaris/data/MeshManager.hpp"
#include "damaris/data/VariableManager.hpp"




namespace damaris {

std::shared_ptr<ParaViewAdaptor> ParaViewAdaptor::instance_;


void ParaViewAdaptor::Initialize(MPI_Comm comm,
                                const model::Simulation::paraview_optional& mdl,
                                const std::string& simname)
{
    std::cout << "ParaView adaptor initialized ... " << std::endl;
    if (processor_ == nullptr) {
        vtkMPICommunicatorOpaqueComm vtkMpiComm(&comm);

        processor_ = vtkCPProcessor::New();
        processor_->Initialize(vtkMpiComm);
    } else {
        processor_->RemoveAllPipelines();
    }

    // Add in the Python script
    model::ParaViewParam::script_sequence scripts = mdl.get().script();
    model::ParaViewParam::script_const_iterator it ;

    for(it = scripts.begin(); it != scripts.end(); ++it) {
        std::string script_path = *it;

        vtkCPPythonScriptPipeline* pipeline = vtkCPPythonScriptPipeline::New();
        pipeline->Initialize(script_path.c_str());
        processor_->AddPipeline(pipeline);

        pipeline->Delete();
    }

    //create the root multi-block grid
    if (rootGrid_ == nullptr)
        rootGrid_ = vtkMultiBlockDataSet::New();
}

void ParaViewAdaptor::Finalize()
{
    std::cout << "ParaView adaptor finalized ... " << std::endl;

    if (processor_ != nullptr) {
        processor_->Delete();
        processor_ = nullptr;
    }

    if (rootGrid_ != nullptr) {
        rootGrid_->Delete();
        rootGrid_ = nullptr;
    }
}

void ParaViewAdaptor::CoProcess(int iteration , bool lastTimeStep)
{
    std::cout << "------------ ParaView Adaptor CoProcess Called: Iteration: " << iteration << endl;

    vtkCPDataDescription* dataDescription = vtkCPDataDescription::New();

    // specify the simulation time and time step for Catalyst
    dataDescription->AddInput("input");
    dataDescription->SetTimeData(iteration*0.1 , iteration);  // What is the difference???????

    if (lastTimeStep == true) { // Execute all pipelines
        dataDescription->ForceOutputOn();
    }

    if (processor_->RequestDataDescription(dataDescription) != 0) {

        int gridCount = MeshManager::GetNumObjects();

        if (gridCount < 1) {
            ERROR("No mesh has been defined in the Xml file!");
            return;
        }

        if (not FillMultiBlockGrid(iteration)) {
            ERROR("Error in Filling the root multi-block grid in iteration " << iteration);
            return;
        }

        // Catalyst gets the proper input dataset for the pipeline.
        dataDescription->GetInputDescriptionByName("input")->SetGrid(rootGrid_);

        // Call Catalyst to execute the desired pipelines.
        processor_->CoProcess(dataDescription);
    }

    dataDescription->Delete();
}

bool ParaViewAdaptor::FillMultiBlockGrid(int iteration)
{
    int index = 0;
    int serverCount = Environment::CountTotalServers();
    int meshCount = MeshManager::GetNumObjects();
    int serverRank = Environment::GetEntityProcessID();

    ERROR("Muti-block element count: " << meshCount);

    rootGrid_->SetNumberOfBlocks(meshCount);

    auto meshItr = MeshManager::Begin();
    for(; meshItr != MeshManager::End(); meshItr++) {
        std::shared_ptr<Mesh> mesh = *meshItr;

        vtkMultiPieceDataSet* vtkMPGrid = vtkMultiPieceDataSet::New();
        rootGrid_->SetBlock(index , vtkMPGrid);
        ERROR("rootGrid_->SetBlock(index , vtkMPGrid) in index: " << index);
        index++;

        auto varItr = VariableManager::Begin();
        for(; varItr != VariableManager::End(); varItr++) {

            std::shared_ptr<Variable> var = *varItr;
            const model::Variable& mdl = var->GetModel();

            if (mdl.mesh() == "#") continue;    // a variable with empty mesh
            if (mdl.mesh() != mesh->GetName()) continue;  // a variable with a differetn mesh
            if (mdl.visualizable() == false) continue;  // non visualizable variable

            ERROR("Varieble to write is: " << var->GetName());

            if (not (*varItr)->AddBlocksToVtkGrid(vtkMPGrid , iteration)) {
                ERROR("Error in adding blocks to variable " << var->GetName());
                return false;
            }
        }
    }
    return true;
}

/*std::shared_ptr<vtkDataSet> ParaViewAdaptor::CoProcessSingleGrid(int iteration)
{
    std::shared_ptr<Mesh> grid = MeshManager::Search(0); // Search by Id, fix later ?????????????

    if (grid == nullptr) {
        ERROR("The only available mesh in the system is not valid!");
        return nullptr;
    }

    std::shared_ptr<vtkDataSet> vtkGrid = grid->GetVtkGrid(iteration);

    auto it = VariableManager::Begin();
    for(; it != VariableManager::End(); it++) {
        const model::Variable& mdl = (*it)->GetModel();

        if (mdl.mesh() == "#") continue;    // a variable with empty mesh
        if (mdl.visualizable() == false) continue;  // non visualizable variable

        ERROR("Varieble to write is: " << (*it)->GetName());

        (*it)->AddBlocksToSingleVtkGrid(vtkGrid , iteration);
    }

    return vtkGrid;
}*/

} // end of namespace damaris
