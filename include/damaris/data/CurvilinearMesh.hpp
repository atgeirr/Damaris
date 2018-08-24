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

#ifndef __DAMARIS_CURVILINEAR_MESH_H
#define __DAMARIS_CURVILINEAR_MESH_H

#include "Damaris.h"

#include "damaris/model/Model.hpp"
#include "damaris/data/StructuredMesh.hpp"

namespace damaris {

/**
 * CurvilinearMesh is a particular type of Mesh.
 */
class CurvilinearMesh : public StructuredMesh {

	friend class Deleter<CurvilinearMesh>;
	
private:
	/**
	 * Constructor, takes a model and a name as parameter.
	 * The constructor is private, the "New" static function should
	 * be used to create Curvilinear Meshes.
	 */
	CurvilinearMesh(const model::Mesh& mdl)
    : StructuredMesh(mdl) {}
	
	/**
	 * Destructor.
	 */
	~CurvilinearMesh() {}
	
public:

	/**
	 * Creates an instance of CurvilinearMesh given a model.
	 * Do some additional checking before creating it. 
	 * If these verifications fail, return a null pointer.
	 * 
	 * \param[in] mdl : model from which to create the instance.
	 */
	template <typename SUPER>
	static std::shared_ptr<SUPER> New(const model::Mesh& mdl,
				     const std::string& name) 
	{
		if(mdl.type() != model::MeshType::curvilinear) {
			return std::shared_ptr<SUPER>();
		}

		if(mdl.name().find("/") != std::string::npos) {
			CFGERROR("Mesh " << mdl.name() << " cannot have a '/' character.");
			return std::shared_ptr<SUPER>();
		}

		std::shared_ptr<CurvilinearMesh> m = std::shared_ptr<CurvilinearMesh>(
			new CurvilinearMesh(mdl),
			Deleter<CurvilinearMesh>());
		m->name_ = name;
		return m;
	}

#ifdef HAVE_VISIT_ENABLED
	/**
	 * Fills the visit handle with metadata related to this Mesh.
	 * Returns true if successful.
	 *
	 * \param[in] md : visit handle to fill.
	 */
	bool ExposeVisItMetaData(visit_handle md);
		
	/**
	 * Fills the visit handle with the data related to this Mesh.
	 * Returns true if successful.
	 * 
	 * \param[in] h : visit handle to fill.
	 * \param[in] source : source of the block to expose to VisIt.
	 * \param[in] iteration : iteration of the block to expose to VisIt.
	 * \param[in] block : domain id of the block to expose to VisIt.
	 */
	bool ExposeVisItData(visit_handle* h, 
		int source, int iteration, int block);
#endif


#ifdef HAVE_PARAVIEW_ENABLED
protected:
	template<typename T>
    vtkDataArray* GetPointsArray(int source , int iteration , int block ,
                                 std::shared_ptr<Variable> vx ,
                                 std::shared_ptr<Variable> vy ,
                                 std::shared_ptr<Variable> vz );
    /**
     * Sets the grid coordinates based on the passed values.
     */
	virtual bool SetGridCoords(vtkDataSet* grid , int source , int iteration , int block ,
                               std::shared_ptr<Variable> vx ,
                               std::shared_ptr<Variable> vy ,
							   std::shared_ptr<Variable> vz);
    /**
     * Sets the curvilinear grid extetns.
     */
	bool SetGridExtents(vtkDataSet* grid , std::shared_ptr<Variable> var, int source , int iteration , int block);
    /**
     * This method creates an instance of Structured mesh and returns it.
     */
	virtual vtkDataSet* CreateVtkGrid() { return vtkStructuredGrid::New();  }
#endif  // of HAVE_PARAVIEW_ENABLED
};

}
#endif
