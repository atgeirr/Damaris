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

#ifndef __DAMARIS_VARIABLE_H
#define __DAMARIS_VARIABLE_H

#include <boost/algorithm/string.hpp>

#include "Damaris.h"

#include "damaris/util/Pointers.hpp"
//#include "damaris/util/TypeWrapper.hpp"
#include "damaris/util/ForwardDcl.hpp"
#include "damaris/util/Deleter.hpp"
#include "damaris/util/Configurable.hpp"

#include "damaris/env/Environment.hpp"
#include "damaris/buffer/Buffer.hpp"
#include "damaris/model/Model.hpp"
#include "damaris/data/LayoutManager.hpp"
#include "damaris/data/Layout.hpp"
#include "damaris/data/BlockIndex.hpp"
#include "damaris/data/Block.hpp"
#include "damaris/data/MeshManager.hpp"

#ifdef HAVE_PARAVIEW_ENABLED
#include "damaris/paraview/ParaViewHeaders.hpp"
#endif

namespace damaris {


/**
 * The Variable object is used for describing a variable within a metadata 
 * structure. It holds an index of Blocks and additional informations. 
 * A Variable is identified by a name and an ID. The ID is either -1 if the
 * Variable has been created manually by the user, or positive or nul integer
 * if it has been created by the VariableManager.
 */
class Variable : public ENABLE_SHARED_FROM_THIS(Variable),
		 public Configurable<model::Variable>
{
	friend class Deleter<Variable>;
	friend class Manager<Variable>;
	
	private:
	
	int id_; /*!< ID of the Variable as initialized 
			by the VariableManager.*/
	std::string name_; /*!< name of the Variable. 
			(full name, including the groups) */
	BlockIndex blocks_; /*!< Blocks container. */
	std::shared_ptr<Layout> layout_; /*!< Layout of the variable. */
	std::shared_ptr<Buffer> buffer_; /*!< Buffer in which to allocate 
					blocks of the variable. */
	std::map<int32_t, std::vector<int64_t> > positions_; /*!< Positions of
		each domain within a global description of the Variable.*/
	//std::shared_ptr<Storage> storage_;
	
	/**
	 * Constructor.
	 * 
	 * \param[in] mdl : model from which to initialize the Variable.
	 */
	Variable(const model::Variable& mdl)
	: Configurable<model::Variable>(mdl), id_(-1), name_(mdl.name()) {
		DBG("Creating variable " << name_);
	}

	/**
	 * Destructor.
	 */
	virtual ~Variable() {
		DBG("Destroying variable " << name_);
	}
	
	public:
		
	typedef BlockIndex::iterator iterator;
	
	/**
	 * Returns the name of the variable.
	 */
	virtual const std::string& GetModelName() const {
		return GetModel().name();
	}
	
	/**
	 * Returns the absolute name of the variable.
	 */
	virtual const std::string& GetName() const {
		return name_;
	}
	
	/**
	 * Returns the id of the variable within the VariableManager.
	 */
	virtual int GetID() const {
		return id_;
	}
	
	/**
	 * Returns the description of the variable.
	 */
	std::string GetDescription() const { 
		return (std::string)GetModel(); 
	}

	/**
	 * Returns the unit of the variable.
	 */
	std::string GetUnit() const { 
		return GetModel().unit(); 
	}

	/**
	 * Returns true if the variable is time-varying.
	 */
	bool IsTimeVarying() const { 
		return GetModel().time_varying();
	}
		
	/**
	 * Returns the Layout of this variable.
	 */
	virtual std::shared_ptr<Layout> GetLayout() const {
		return layout_;
	}
	
	/**
	 * Returns the buffer associated to the variable.
	 */
	virtual std::shared_ptr<Buffer> GetBuffer() {
		if(not buffer_) {
			buffer_ = Environment::GetDefaultBuffer();
		}
		return buffer_;
	}
	
	/**
	 * Sets the position of a domain.
	 *
	 * \param[in] block : domain id.
	 * \param[in] p : vector of lower bounds. Should have the same dimension
	 * as the dimension of the layout.
	 */
	virtual int SetPosition(int32_t block, const std::vector<int64_t>& p)
	{
		if(p.size() != GetLayout()->GetDimensions())
			return DAMARIS_INVALID_DIMENSIONS;
		positions_[block] = p;
		return DAMARIS_OK;
	}
	
	/**
	 * Attach a Block already created. Since only a Variable can
	 * create a Block, either this function will be used internally,
	 * or it can be used externally to transfer a block from a Variable
	 * to another.
	 * 
	 * \param[in] block : block to attach.
	 */
	bool AttachBlock(const std::shared_ptr<Block>& block);
	
	/**
	 * Removes a block from the Variable. Will NOT delete the data
	 * until all instances of the std::shared_ptr have disappeared and
	 * all the instances of dataspace.
	 * 
	 * \param[in] block : block to detach.
	 */
	bool DetachBlock(const std::shared_ptr<Block>& block);
	
	/**
	 * Creates a Block, allocates memory for it in the Variable's buffer
	 * and return the allocated block. The block is also attached to the
	 * Variable.
	 *
	 * \param[in] source : source that creates the block.
	 * \param[in] iteration : iteration at which the block is created.
	 * \param[in] bid : domain id.
	 * \param[in] blocking : whether or not to block if the memory is full.
	 */
	std::shared_ptr<Block> Allocate(int source, int iteration, int bid, 
					bool blocking=false);
			

	/**
	 * Same as Allocate but tries to allocate a block that has
	 * fixed size (not according to layout). Intended use for dedicated nodes.
	 * \see Allocate
	 */

	std::shared_ptr<Block> AllocateFixedSize(int source, int iteration, int bid,
						const std::vector<int64_t>& lbounds, 
						const std::vector<int64_t>& ubounds,
						const std::vector<int64_t>& gbounds,
						const std::vector<size_t>& ghosts,
						bool blocking=false);
	/**
	 * Same as Allocate but tries to allocate a memory region that is
	 * aligned with the memory page size.
	 * \see Allocate
	 */
	std::shared_ptr<Block> AllocateAligned(int source, int iteration, int bid, 
					bool blocking=false);
	
	/**
	 * Re-build a Block from a handle and known source, iteration, domain
	 * id and data size. The block will be added to the Variable.
	 * Works only if there is not yet any block associated to these 
	 * metadata.
	 * The block doesn't own the data. To give data ownership to the block,
	 * call b->GainDataOwnership() on the returned block.
	 *
	 * \param[in] source : source that created the block.
	 * \param[in] iteration : iteration at which the block was created.
	 * \param[in] bid : domain id of the block.
	 * \param[in] h : handle to get the address in memory.
	 * \param[in] size : size of the data to retrieve.
	 */
	std::shared_ptr<Block> Retrieve(int source, int iteration, int bid,
					const Handle& h, size_t size);
	
	/**
	 * Re-build a Block from a handle and known source, iteration, domain
	 * id and upper/lower bounds.
	 * Works only if there is not yet any block associated to these 
	 * metadata.
	 * The block doesn't own the data. To give data ownership to the block,
	 * call b->GainDataOwnership() on the returned block.
	 *
	 * \param[in] source : source that created the block.
	 * \param[in] iteration : iteration at which the block was created.
	 * \param[in] bid : domain id of the block.
	 * \param[in] lbounds : lower bounds of the block.
	 * \param[in] ubounds : upper bounds of the block.
	 * \param[in] gbounds : global bounds of the block.
	 * \param[in] ghosts : ghost zones of the block.
	 * \param[in] h : handle to get the address in memory.
	 */
	std::shared_ptr<Block> Retrieve(int source, int iteration, int bid,
					const std::vector<int64_t>& lbounds,
					const std::vector<int64_t>& ubounds,
					const std::vector<int64_t>& gbounds,
					const std::vector<size_t>& ghosts,
					const Handle& h);
	
	/**
	 * Remove all blocks from the Variable.
	 */
	void ClearAll();
	
	/**
	 * Clear all blocks with a given iteration.
	 *
	 * \param[in] iteration : iteration to clear.
	 */
	void ClearIteration(int iteration);

	/**
	 * Clear all blocks with an iteration less or equal to the given.
	 *
	 * \param[in] iteration : maximum iteration to clean.
	 */
	void ClearUpToIteration(int iteration);
	
	/**
	 * Clear all blocks with a given source.
	 *
	 * \param[in] source : source to clear.
	 */
	void ClearSource(int source);
	
	/**
	 * Clear all blocks with a given id.
	 *
	 * \param[in] id : domain id to clear.
	 */
	void ClearId(int id);
	
	/**
	 * Counts the number of blocks stored locally on this process for
	 * a given iteration.
	 *
	 * \param[in] iteration : iteration for which to count the blocks.
	 */
	int CountLocalBlocks(int iteration) const;
	
	/**
	 * Gives the number of blocks existing globally for the specified
	 * iteration. Currently this is an estimation only and may not
	 * reflect the true state of the system 
	 *
	 * \param[in] iteration : iteration for which to count the blocks.
	 */
	int CountTotalBlocks(int iteration) const;
	
	/**
	 * Get a Block given a source, iteration and id.
	 *
	 * \param[in] source : source of the block to find.
	 * \param[in] iteration : iteration of the block to find.
	 * \param[in] id : domain id of the block to find.
	 */
	std::shared_ptr<Block> GetBlock(int source, int iteration, int id) const;
	
	/**
	 * Returns an iterator over the set of Blocks.
	 */
	iterator Begin() {
		return blocks_.begin();
	}

	/**
	 * Returns an iterator pointing to the end of the set of Blocks.
	 */
	iterator End() {
		return blocks_.end();
	}
	
	/**
	 * For a given iteration, get begin and end iterators over
	 * all chunks of this iteration.
	 * 
	 * \param[in] iteration : iteration of the blocks to get.
	 * \param[out] begin : begin iterator.
	 * \param[out] end : end iterator.
	 */
	void GetBlocksByIteration(int iteration,
		BlocksByIteration::iterator& begin,
		BlocksByIteration::iterator& end) const;
	
	/**
	 * For a given source, get begin and end iterators over
	 * all chunks of this given source.
	 *
	 * \param[in] source : source of the blocks to get.
	 * \param[out] begin : begin iterator.
	 * \param[out] end : end iterator.
	 */
	void GetBlocksBySource(int source,
		BlocksBySource::iterator& begin,
		BlocksBySource::iterator& end) const;
	
	/**
	 * For a given id, get begin and end iterators over
	 * all chunks of this id.
	 *
	 * \param[in] id : domain id of the blocks to get.
	 * \param[out] begin : begin iterator.
	 * \param[out] end : end iterator.
	 */
	void GetBlocksById(int bid,
		BlocksById::iterator& begin,
		BlocksById::iterator& end) const;
	
	/**
	 * Gets the range of iterations stored.
	 *
	 * \param[out] lowest : lowest iteration number (inclusive).
	 * \param[out] biggest : biggest iteration number (inclusive).
	 */
	bool GetIterationRange(int& lowest, int& biggest) const;
	
	/**
	 * Gets the range of sources stored.
	 *
	 * \param[out] lowest : lowest source number (inclusive).
	 * \param[out] biggest : biggest source number (inclusive).
	 */
	bool GetSourceRange(int& lowest, int& biggest) const;
	
	/**
	 * Gets the range of domain ids stored.
	 *
	 * \param[out] lowest : lowest id number (inclusive).
	 * \param[out] biggest : biggest id number (inclusive).
	 */
	bool GetIDRange(int& lowest, int& biggest) const;
	
	/**
	 * Static method to build a new Variable. Constructs a std::shared_ptr.
	 *
	 * \param[in] mdl : model from which to build the Variable.
	 * \param[in] name : name for the object.
	 */
	template<typename SUPER>
	static std::shared_ptr<SUPER> New(const model::Variable& mdl, 
				     const std::string& name)
	{

		if(mdl.name().find("/") != std::string::npos) {                 
			CFGERROR("Variable " << mdl.name() << " cannot have a '/' character.");
			return std::shared_ptr<SUPER>();                
		}
	
		std::string layoutName = mdl.layout();

		std::shared_ptr<Layout> l;
		bool layoutIsAbsolute = (layoutName.find("/") != std::string::npos);

		if(layoutIsAbsolute) {
			l = LayoutManager::Search(layoutName);
		} else {

			std::vector<std::string> groups;
			boost::split(groups, name, boost::is_any_of("/"));

			while(not l && groups.size() != 0) {
				groups.pop_back();
				std::string absoluteName;
				if(groups.size() != 0) {
					std::vector<std::string>::iterator it = 
						groups.begin();
					absoluteName = *it;
					it++;
					for(; it != groups.end(); it++) {
						absoluteName += "/" + (*it);
					}
					absoluteName += "/" + layoutName;
				} else {
					absoluteName = layoutName;
				}
				l = LayoutManager::Search(absoluteName);
			}
		}

		if(not l) {
			CFGERROR("Layout \"" << mdl.layout() 
				<< "\" not found for variable \"" 
				<< mdl.name() << "\"");
			return std::shared_ptr<SUPER>();
		}

		std::shared_ptr<SUPER> v(new Variable(mdl), Deleter<Variable>());
		v->layout_ = l;
		v->name_ = name;
		return v;
	}

	/**
	 * Static method to build a new Variable. Constructs a std::shared_ptr.
	 * 
	 * \param[in] mdl : model from which to build the Variable.
	 */
	template<typename SUPER>
	static std::shared_ptr<SUPER> New(const model::Variable& mdl)
	{
		return New<SUPER>(mdl,mdl.name());
	}

    /**
     * Static method to return the mesh associated to the variable.
     */
    std::shared_ptr<Mesh> GetMesh()
    {
        std::string meshName = GetModel().mesh();
        return MeshManager::Search(meshName);
    }
	
#ifdef HAVE_VISIT_ENABLED
	public:
	
	/**
	 * Fills VisIt's metadata handle with information related to 
	 * the variable.
	 * 
	 * \param[in] md : visit handle to fill.
	 * \param[in] iteration : iteration to expose to VisIt.
	 */
	bool ExposeVisItMetaData(visit_handle md, int iteration);

	/**
	 * Fills VisIt's data handle with the proper data.
	 * 
	 * \param[out] md : visit handle to create and fill.
	 * \param[in] source : source to expose to VisIt.
	 * \param[in] iteration : iteration to expose to VisIt.
	 * \param[in] block : domain to expose to VisIt.
	 */
	bool ExposeVisItData(visit_handle *md, 
		int source, int iteration, int block);

	/**
	 * Fills a VisIt handle to expose the domain list.
	 * 
	 * \param[in] md : visit handle to create and fill.
	 * \param[in] iteration : iteration to expose.
	 */
	bool ExposeVisItDomainList(visit_handle *md, int iteration);

	private:
		
	/**
	 * Converts a Model::VarCentering enum to a VisIt_VarCentering enum.
	 *
	 * \param[in] vc : VarCentering enum to convert.
	 */
	static VisIt_VarCentering VarCenteringToVisIt(
		const model::VarCentering& vc);
		
	/**
	 * Converts a Model::VarType enum to a VisIt_VarType enum.
	 *
	 * \param[in] vt : VarType to convert into a VisIt_VarType.
	 */
	static VisIt_VarType VarTypeToVisIt(const model::VarType& vt);
#endif

#ifdef HAVE_PARAVIEW_ENABLED

    public:

    /**
	* Adds different blocks of a variable into the passed multi piece grid
	*
	* \param[in,out] vtkMPGrid : the root multi piece grid for an specific mesh
	* \param[in] iteration : the Damaris iteration
	*/
    bool AddBlocksToVtkGrid(vtkMultiPieceDataSet* vtkMPGrid , int iteration);

	/**
	* Adds the variable's main data as a grid field data
	*
	* \param[in,out] grid : the Damaris iteration
	* \param[in] buffer : the buffer of type T* to be added to the grid as the field data
	*/
	template <typename T>
    bool AddBufferToVtkGrid(vtkDataSet* grid , T* buffer);
#endif

};

}

#endif
