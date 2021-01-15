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
	/**
	 * main constructor for building the object based on the passed model
	 */
    StructuredMesh(const model::Mesh& mdl)
    : Mesh(mdl) {}

    virtual void SetSourceRange(int source_range_low, int source_range_high) {}
    virtual int GetTotalMeshSections(int iteration) {}

#ifdef HAVE_PARAVIEW_ENABLED
protected:

	/**
	 * returns the buffer of a coordinate variable at an exact source, iteration and block
	 *
	 * \param[in] source : the source of the coordinate block
	 * \param[in] iteration : the Damaris iteration
	 * \param[in] block : the block id
	 * \param[in] var : the coordinate variable
	 */
	void* GetCoordBuffer(int source , int iteration , int block ,
						 const std::shared_ptr<Variable>& var);

	/**
	 * returns the block of a coordinate variable at an exact source, iteration and block
	 *
	 * \param[in] source : the source of the coordinate block
	 * \param[in] iteration : the Damaris iteration
	 * \param[in] block : the block id
	 * \param[in] var : the coordinate variable
	 */
	std::shared_ptr<Block> GetCoordBlock(int source , int iteration , int block ,
										 const std::shared_ptr<Variable>& var);

    /**
	 * Sets the grid coordinate variables based on the passed values.
	 *
	 * \param[in,out] grid : the expected grid
	 * \param[in] source : the source of the expected block
	 * \param[in] iteration : the Damaris iteration
	 * \param[in] block : the block id of the expected block
	 * \param[in] vx : the coordinate variable for X direction
	 * \param[in] vy : the coordinate variable for Y direction
	 * \param[in] vz : the coordinate variable for Z direction
     */
	virtual bool SetGridCoords(vtkDataSet* grid , int source , int iteration , int block ,
							   const std::shared_ptr<Variable>& vx ,
							   const std::shared_ptr<Variable>& vy ,
							   const std::shared_ptr<Variable>& vz) = 0;
    /**
     * Sets the grid extents based on the grid type.
	 *
	 * \param[in,out] grid : the expected grid for setting the extents
	 * \param[in] source : the source of the expected block
	 * \param[in] iteration : the Damaris iteration
	 * \param[in] block : the block id of the expected block
	 * \param[in] var : the variable
     */
	virtual bool SetGridExtents(vtkDataSet* grid , int source , int iteration , int block ,
								const std::shared_ptr<Variable>& var) = 0;



    /**
     * Creates and returns a typed vtkDataArray object based
	 * on the passed data pointer and type T.
	 *
	 * \param[in] length : number of elements in the pointer
	 * \param[in] pointer : pointer to data
     */
    template <typename T>
	vtkDataArray* CreateTypedCoordArray(size_t length , T* pointer);

    /**
	 * Creates and returns a vtkDataArray object for coordinat data
	 *
	 * \param[in] source : the source of the expected coordinate block
	 * \param[in] iteration : damaris iteration
	 * \param[in] block : the id of the expected coordinate block
	 * \param[int] var : desired coordinate variable
     */
    vtkDataArray* CreateCoordArray(int source , int iteration , int block ,
								  const std::shared_ptr<Variable>& var);

    /**
	 * This returns the three variable coordinates of the grid. Due to the
	 * dimention of the grid, some variables may be nullptr.
	 *
	 * \param[in,out] vx : the coordinate variable for X direction
	 * \param[in,out] vy : the coordinate variable for Y direction
	 * \param[in,out] vz : the coordinate variable for Z direction
     */
	bool GetGridVariables(std::shared_ptr<Variable>& vx ,
						  std::shared_ptr<Variable>& vy ,
						  std::shared_ptr<Variable>& vz );
public:
    /**
	* creates and returns the expected VTK grid object for a block
    *
	* \param[in] source : source of the block
	* \param[in] iteration : iteration of the block
	* \param[in] block : id of the block
	* \param[in] var : the variable owning the block
    */
	virtual vtkDataSet* GetVtkGrid(int source , int iteration , int block ,
								   const std::shared_ptr<Variable>& var) final;
#endif
}; // of class StructuredMesh

} // of namespace damaris

#endif // STRUCTUREDMESH_HPP
