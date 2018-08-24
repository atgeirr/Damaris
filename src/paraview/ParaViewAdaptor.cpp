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
    if (processor_ == nullptr) {
        vtkMPICommunicatorOpaqueComm vtkMpiComm(&comm);

        processor_ = vtkCPProcessor::New();
        processor_->Initialize(vtkMpiComm);
    } else {
        processor_->RemoveAllPipelines();
    }

	// Add the Python script
    model::ParaViewParam::script_sequence scripts = mdl.get().script();
    model::ParaViewParam::script_const_iterator it ;

    for(it = scripts.begin(); it != scripts.end(); ++it) {
        std::string script_path = *it;

        vtkCPPythonScriptPipeline* pipeline = vtkCPPythonScriptPipeline::New();
        pipeline->Initialize(script_path.c_str());
        processor_->AddPipeline(pipeline);

        pipeline->Delete();
    }
}

void ParaViewAdaptor::Finalize()
{
    if (processor_ != nullptr) {
        processor_->Delete();
        processor_ = nullptr;
    }
}

void ParaViewAdaptor::CoProcess(int iteration , bool lastTimeStep)
{
    vtkCPDataDescription* dataDescription = vtkCPDataDescription::New();

    // specify the simulation time and time step for Catalyst
    dataDescription->AddInput("input");
	dataDescription->SetTimeData(iteration*0.1 , iteration);  // What is the difference?

	if (lastTimeStep == true) { // Who to know about it?
        dataDescription->ForceOutputOn();
    }

    if (processor_->RequestDataDescription(dataDescription) != 0) {

        int gridCount = MeshManager::GetNumObjects();

        if (gridCount < 1) {
            ERROR("No mesh has been defined in the Xml file!");
            return;
        }

		vtkMultiBlockDataSet* rootGrid = vtkMultiBlockDataSet::New();

		if (not FillMultiBlockGrid(iteration , rootGrid)) {
            ERROR("Error in Filling the root multi-block grid in iteration " << iteration);
            return;
        }

        // Catalyst gets the proper input dataset for the pipeline.
		dataDescription->GetInputDescriptionByName("input")->SetGrid(rootGrid);

        // Call Catalyst to execute the desired pipelines.
        processor_->CoProcess(dataDescription);

		rootGrid->Delete();
    }

    dataDescription->Delete();
}

bool ParaViewAdaptor::FillMultiBlockGrid(int iteration , vtkMultiBlockDataSet* rootGrid)
{
    int index = 0;
    int serverCount = Environment::CountTotalServers();
    int meshCount = MeshManager::GetNumObjects();
    int serverRank = Environment::GetEntityProcessID();

	rootGrid->SetNumberOfBlocks(meshCount);

    auto meshItr = MeshManager::Begin();
    for(; meshItr != MeshManager::End(); meshItr++) {
        std::shared_ptr<Mesh> mesh = *meshItr;

        vtkMultiPieceDataSet* vtkMPGrid = vtkMultiPieceDataSet::New();
		rootGrid->SetBlock(index , vtkMPGrid);
        index++;

        auto varItr = VariableManager::Begin();
        for(; varItr != VariableManager::End(); varItr++) {

            std::shared_ptr<Variable> var = *varItr;
            const model::Variable& mdl = var->GetModel();

            if (mdl.mesh() == "#") continue;    // a variable with empty mesh
            if (mdl.mesh() != mesh->GetName()) continue;  // a variable with a differetn mesh
            if (mdl.visualizable() == false) continue;  // non-visualizable variable

            if (not (*varItr)->AddBlocksToVtkGrid(vtkMPGrid , iteration)) {
                ERROR("Error in adding blocks to variable " << var->GetName());
                return false;
            }
        }
    }
    return true;
}


} // end of namespace damaris
