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

#ifndef STRUCTUREDMESH_HPP
#define STRUCTUREDMESH_HPP

#include "damaris/data/Mesh.hpp"

#ifdef HAVE_PARAVIEW_ENABLED
#include "damaris/paraview/ParaViewHeaders.hpp"
#endif

namespace damaris {

class StructuredMesh : public Mesh
{
public:
    StructuredMesh(const model::Mesh& mdl)
    : Mesh(mdl) {}


#ifdef HAVE_PARAVIEW_ENABLED
protected:

	/**
	 * Sets the grid coordinates based on the passed values.
	 */
	void* GetCoordBuffer(int source , int iteration , int block ,
						 std::shared_ptr<Variable> var);

	/**
	 * Sets the grid coordinates based on the passed values.
	 */
	std::shared_ptr<Block> GetCoordBlock(int source , int iteration , int block ,
										 std::shared_ptr<Variable> var);

    /**
     * Sets the grid coordinates based on the passed values.
     */
	virtual bool SetGridCoords(vtkDataSet* grid , int source , int iteration , int block ,
                               std::shared_ptr<Variable> vx ,
                               std::shared_ptr<Variable> vy ,
							   std::shared_ptr<Variable> vz) = 0;
    /**
     * Sets the grid extents based on the grid type.
     */
	virtual bool SetGridExtents(vtkDataSet* grid , std::shared_ptr<Variable> var,
								int source , int iteration , int block) = 0;

    /**
     * Every derived class, should implement this method and
     * create the appropriate vtkDataSet and return it.
     */
    virtual vtkDataSet* CreateVtkGrid() = 0;

    /**
     * Extracts thw low and high extents of a dimention based
     * on its coordinate variable, i.e. coord.
     */
    void GetGridExtents(int source , int iteration , int block ,
                        const std::shared_ptr<Variable>& coord ,
                        int& lowExtent , int& highExtent);

    /**
     * Creates and returns a typed vtkDataArray object based
     * on the passed type.
     */
    template <typename T>
    vtkDataArray* CreateTypedCoordArray(size_t length ,
									   T* pointer);
    /**
     * Creates and returns a vtkDataArray object based
     * on the passed type.
     */
    vtkDataArray* CreateCoordArray(int source , int iteration , int block ,
                                  std::shared_ptr<Variable> var);


    /**
     * This function returns the low and high extents of the Grid
     * based on the grid topology. It also gets the coordinate variables.
     */
	bool GetGridVariables(std::shared_ptr<Variable>& vx ,
						  std::shared_ptr<Variable>& vy ,
						  std::shared_ptr<Variable>& vz );
public:
    /**
    * Returns the relevant Rectilinear VTK mesh
    *
    */
	virtual vtkDataSet* GetVtkGrid(std::shared_ptr<Variable> var,int source ,
								   int iteration , int block);
#endif
}; // of class StructuredMesh

} // of namespace damaris

#endif // STRUCTUREDMESH_HPP
