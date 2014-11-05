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

#ifndef __DAMARIS_BLOCK_H
#define __DAMARIS_BLOCK_H

#include "Damaris.h"

#include "damaris/util/Pointers.hpp"
#include "damaris/util/TypeWrapper.hpp"
#include "damaris/util/ForwardDcl.hpp"
#include "damaris/util/Deleter.hpp"

#include "damaris/buffer/SharedMemory.hpp"
#include "damaris/buffer/DataSpace.hpp"

#include "damaris/model/Model.hpp"

#ifdef HAVE_VISIT_ENABLED
	#include <VisItDataInterface_V2.h>
#endif

namespace damaris {
	
USING_POINTERS;

/**
 * A Block is the main entry point for data; it is characterized by
 * a source, and iteration number and a domain id.
 */
class Block //: public ENABLE_SHARED_FROM_THIS(Block)
{
	friend class Deleter<Block>;
	friend class Variable;
	
	private:

	int source_; /*! source that generated the block */
	int iteration_; /*!< iteration at which the block has been generated */
	int id_; /*!< domain id of the block */
	
	weak_ptr<Variable> variable_; 	/*!< variable owning this block */
	std::vector<int64_t> lower_bounds_;	/*!< list of lower bounds */
	std::vector<int64_t> upper_bounds_; /*!< list of upper bounds */
	DataSpace<Buffer> dataspace_;	/*!< Dataspace holding the data */
	Handle handle_;			/*!< Handle to the data */
	bool read_only_;		/*!< true if the Block should not be
					modified, for some reason. */
	
	/**
	 * Constructor.
	 * 
	 * \param[in] source : source of the block.
	 * \param[in] id : domain id.
	 * \param[in] variable : owner of the block.
	 */
	Block(	int source, int iteration, int id,
		const shared_ptr<Variable> & variable);

	/**
	 * Destructor.
	 */
	virtual ~Block() {}
	
	/**
	 * Sets an upper bound of the block.
	 * 
	 * \param[in] i : index of the upper-bound.
	 * \param[in] val : new value.
	 */
	virtual void SetEndIndex(unsigned int i, int64_t val) {
		if(i < upper_bounds_.size()) {
			upper_bounds_[i] = val;
		} else {
			WARN("Accessing index out of dimensions bound");
		}
	}
	
	/**
	 * Sets an lower bound of the block.
	 * 
	 * \param[in] i : index of the lower-bound.
	 * \param[in] val : new value.
	 */
	virtual void SetStartIndex(unsigned int i, int64_t val) {
		if(i < lower_bounds_.size()) {
			lower_bounds_[i] = val;
		} else {
			WARN("Accessing index out of dimensions bound");
		}
	}
	
	/**
	 * Static method to build a new Block. Constructs a shared_ptr.
	 * See contructor fot the meaning of the parameters.
	 */
	static shared_ptr<Block> New(int source, int iteration, int id,
					const shared_ptr<Variable>& variable);
	
	public:
	
	/**
	 * Gets the ID of the process that has written the block.
	 */
	virtual int GetSource() const { 
		return source_; 
	}

	/**
	 * Gets the iteration at which the chunk has been written.
	 */
	virtual int GetIteration() const { 
		return iteration_; 
	}

	/**
	* Get the domain id of the block.
	*/
	virtual int GetID() const { 
		return id_; 
	}
	
	/**
	 * Sets/unsets the block as read-only.
	 * This should be done only by the Client of Servers, not by a user.
	 * If the block is read-only, it's for a reason!
	 *
	 * \param[in] ro : true if read-only, false if read-write.
	 */
	virtual void SetReadOnly(bool ro);
	
	/**
	 * Returns true if the block is read-only.
	 */
	virtual bool IsReadOnly() const {
		return read_only_;
	}

	/**
	 * Gets the number of dimensions.
	 */
	virtual int GetDimensions() const;

	/**
	 * Gets a lower-bound of the block along a specific dimension.
	 */
	virtual int64_t GetStartIndex(unsigned int i) const {
		if(i < lower_bounds_.size()) {
			return lower_bounds_[i];
		} else {
			WARN("Accessing index out of dimensions bound");
			return 0;
		}
	}
	
	/**
	 * Gets an upper-bound of the block along a specific dimension.
	 */
	virtual int64_t GetEndIndex(unsigned int i) const {
		if(i < upper_bounds_.size()) {
			return upper_bounds_[i];
		} else {
			WARN("Accessing index out of dimensions bound");
			return 0;
		}
	}

	/**
	 * Gives the number of items contained in the Block. 
	 */
	virtual int NbrOfItems() const {
		if(upper_bounds_.size() == 0) return 0;
		int result = 1;
		for(unsigned int i = 0; i < upper_bounds_.size(); i++) {
			result *= (upper_bounds_[i] - lower_bounds_[i] + 1);
		}
		return result;
	}
	
	virtual int Move(const std::vector<int64_t>& p) {
		if((int)p.size() != GetDimensions()) {
			return DAMARIS_INVALID_DIMENSIONS;
		}
		
		for(int i=0; i < GetDimensions(); i++) {
			upper_bounds_[i] += (p[i] - lower_bounds_[i]);
			lower_bounds_[i] = p[i];
		}
		
		return DAMARIS_OK;
	}
	
	/**
	 * Returns the internal DataSpace.
	 */
	virtual const DataSpace<Buffer>& GetDataSpace() const {
		return dataspace_;
	}
	
	/**
	 * Sets the DataSpace. If another DataSpace was already attached to
	 * this Block, this may trigger the Release function for the previous
	 * data. Note that this does not link the dataspace to the block.
	 * If the dataspace was linked to another block before, this other
	 * block will be the one responsible for the release of its data.
	 *
	 * \param[in] ds : new dataspace to link with this block.
	 */
	virtual void SetDataSpace(const DataSpace<Buffer>& ds) {
		dataspace_ = ds;
	}
	
	/**
	 * \see DataSpace::GetDataOwnership()
	 */
	virtual bool GetDataOwnership() const {
		return dataspace_.GetDataOwnership();
	}
	
	/**
	 * \see DataSpace::LoseDataOwnership()
	 */
	virtual void LoseDataOwnership() {
		dataspace_.LoseDataOwnership();
	}
	
	/**
	 * \see DataSpace::GainDataOwnership()
	 */
	virtual void GainDataOwnership() {
		dataspace_.GainDataOwnership();
	}

#ifdef HAVE_VISIT_ENABLED
	/**
	 * Fills a VisIt data handle (already allocated) to expose the data to 
	 * VisIt. Return true if it managed to expose the data.
	 * 
	 * \param[in] hdl : visit handle to fill.
	 */
	bool FillVisItDataHandle(visit_handle hdl);
#endif
	
};

}

#endif
