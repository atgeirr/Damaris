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

#ifndef PARAVIEWADAPTOR_H
#define PARAVIEWADAPTOR_H


#include "damaris/util/Deleter.hpp"
#include "damaris/util/Pointers.hpp"
#include "damaris/model/Model.hpp"

#include "mpi.h"
#include "damaris/paraview/ParaViewHeaders.hpp"



namespace damaris {

/**
 * The ParaViewAdaptor class allows only one instance of it to be created. It acts as a mediator between Damaris classes and ParaView Catalyst
 * functionality. At the end of each iteration, this class will be responsible for updating the live visualizatio of the data collected at the
 * servers.
 */
class ParaViewAdaptor
{
    friend class Deleter<ParaViewAdaptor>;

    static std::shared_ptr<ParaViewAdaptor> instance_;

    vtkCPProcessor* processor_;

    vtkMultiBlockDataSet* rootGrid_;


protected:
    /**
    * Constructor.
    */
    ParaViewAdaptor() {
        rootGrid_ = nullptr;
        processor_ = nullptr;
    }

    /**
    * Destructor.
    */
    virtual ~ParaViewAdaptor() {


    }

    bool FillMultiBlockGrid(int iteration);

public:

    /**
    * creates or returns the singleton object.
    */
    static std::shared_ptr<ParaViewAdaptor> GetInstance()
    {
        if (instance_ == NULL)
            instance_ = std::shared_ptr<ParaViewAdaptor> (new ParaViewAdaptor() , Deleter<ParaViewAdaptor>());

        return instance_;
    }

    /**
    * Initiates the singleton object.
    */
    void Initialize(MPI_Comm comm,
                    const model::Simulation::paraview_optional& mdl,
                    const std::string& simname);
    /**
    * Finalizes the CoProcessors of the singleton object.
    */
    void Finalize();

    /**
    * Updates ParaView Catalyst filters at the end of each iteration.
    */
    void CoProcess(int iteration , bool lastTimeStep=false);

    /**
    * return a ParaView adaptor object based on the grid type defined in XML file
    */
    std::shared_ptr<ParaViewAdaptor> GetAdaptor(const model::Simulation::paraview_optional& mdl);
};


}
#endif // PARAVIEWADAPTOR_H
