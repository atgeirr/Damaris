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
								const std::string& simName)
{
    if (processor_ == nullptr) {
        vtkMPICommunicatorOpaqueComm vtkMpiComm(&comm);

        processor_ = vtkCPProcessor::New();
        processor_->Initialize(vtkMpiComm);
    } else {
        processor_->RemoveAllPipelines();
    }

    timestep_      = mdl.get().realtime_timestep();    // default is 0.1
    updatefreq_    = mdl.get().update_frequency();     // default is 1, but is this neededas the Catalyst script can set this also?
    end_iteration_ = mdl.get().end_iteration();        // default is 0 - should be tested for

    mdl_ = mdl ;  // I'm not sure how inefficient this copy is
    AddPythonPipeline();
}

void ParaViewAdaptor::AddPythonPipeline()
{
// Add the Python script
    model::ParaViewParam::script_sequence scripts = mdl_.get().script();
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

void ParaViewAdaptor::CoProcess(int iteration)
{
	vtkNew<vtkCPDataDescription> dataDescription;

	bool lastTimeStep = false ;
    // specify the simulation time and time step for Catalyst
    dataDescription->AddInput("input");

	dataDescription->SetTimeData(iteration*timestep_ , iteration);  // What is the difference? Simulation time and simulation iteration

	if (end_iteration_ != 0)
	{
		if (iteration == end_iteration_)
			lastTimeStep = true ;
	}
	// check: end_iteration_ from the xsd model. if not == 0 then use the value
	if (lastTimeStep == true) { // How to know about it?
        dataDescription->ForceOutputOn();
    }

    if (processor_->RequestDataDescription(dataDescription) != 0) {

        int gridCount = MeshManager::GetNumObjects();

        if (gridCount < 1) {
            ERROR("No mesh has been defined in the Xml file!");
            return;
        }

		vtkNew<vtkMultiBlockDataSet> rootGrid;

		if (not FillMultiBlockGrid(iteration , rootGrid)) {
            ERROR("Error in Filling the root multi-block grid in iteration " << iteration);
            return;
		}

        // Catalyst gets the proper input dataset for the pipeline.
		dataDescription->GetInputDescriptionByName("input")->SetGrid(rootGrid);

	    //if ( processor_->GetNumberOfPipelines() == 0)
	    //	AddPythonPipeline();

	    // Call Catalyst to execute the desired pipelines.
	    //  if (iteration > 0)
	    if ( processor_->GetNumberOfPipelines() > 0)
	    	processor_->CoProcess(dataDescription);
    }
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
            if (mdl.mesh() != mesh->GetName()) continue;  // a variable with a different mesh
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
