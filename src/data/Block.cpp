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
#include "util/Unused.hpp"
#include "data/Block.hpp"
#include "data/Variable.hpp"

namespace damaris {

Block::Block(int source, int iteration, int id, 
		const std::shared_ptr<Variable> & variable)
: source_(source), iteration_(iteration), id_(id), variable_(variable),
  read_only_(false)
{
}

std::shared_ptr<Block> Block::New(int source, int iteration, int id,
				const std::shared_ptr<Variable>& v)
{
	if(not v) {
		ERROR("Invalid Variable when building Block instance");
		return std::shared_ptr<Block>();
	}
	std::shared_ptr<Layout> layout = v->GetLayout();
	if(not layout) {
		ERROR("Invalid Layout when building Block instance");
		return std::shared_ptr<Block>();
	}
	
	std::shared_ptr<Block> block(new Block(source,iteration,id,v),
				 Deleter<Block>());
	
	unsigned int d = layout->GetDimensions();
	block->lower_bounds_.resize(d);
	block->upper_bounds_.resize(d);
	block->global_dims_.resize(d);
	block->ghosts_.resize(d);

	for(unsigned int i = 0; i < d; i++) {
		block->lower_bounds_[i] = 0;
		block->upper_bounds_[i] = layout->GetExtentAlong(i)-1;
		block->global_dims_[i]  = layout->GetGlobalExtentAlong(i);
		block->ghosts_[i] 	= layout->GetGhostAlong(i);
	}

	return block;
}

void Block::SetReadOnly(bool ro)
{
	void* addr = dataspace_.GetData();
	size_t size = dataspace_.GetSize();
	std::shared_ptr<Variable> v = variable_.lock();
	if(not v || addr == NULL) return;
		
	if(ro && not read_only_) {
		v->GetBuffer()->ChangeMode(addr,size,READ_ONLY);
	} else if(not ro && read_only_) {
		v->GetBuffer()->ChangeMode(addr,size,READ_WRITE);
	}
	read_only_ = ro;
}

int Block::GetDimensions() const { 
	if(not variable_.expired()) {
		return variable_.lock()->GetLayout()->GetDimensions();
	} else {
		ERROR("Variable expired for instance of Block");
		return -1;
	}
}

#ifdef HAVE_VISIT_ENABLED
bool Block::FillVisItDataHandle(visit_handle hdl)
{
	if(variable_.expired()) {
		return false;
	}
	std::shared_ptr<Variable> v = variable_.lock();
	DataSpace<Buffer> ds = GetDataSpace();
	
	DBG("source is " << GetSource() << ", iteration is " << GetIteration());
	int nb_items = GetNumberOfItems();
	DBG("Number of items is " << nb_items);
	const model::Type& t = v->GetLayout()->GetType();
	DBG("Type is " << t);

	if(hdl != VISIT_INVALID_HANDLE) {
		if(t == model::Type::int_ or t == model::Type::integer) {
			VisIt_VariableData_setDataI(hdl, 
				VISIT_OWNER_SIM, 1, nb_items, 
				(int*)ds.GetData());
		} else if(t == model::Type::float_ or t == model::Type::real) {
			VisIt_VariableData_setDataF(hdl, 
				VISIT_OWNER_SIM, 1, nb_items, 
				(float*)ds.GetData());
		} else if(t == model::Type::double_) {
			VisIt_VariableData_setDataD(hdl, 
				VISIT_OWNER_SIM, 1, nb_items, 
				(double*)ds.GetData());
		} else if(t == model::Type::char_ or t == model::Type::character) {
			VisIt_VariableData_setDataC(hdl, 
				VISIT_OWNER_SIM, 1, nb_items, 
				(char*)ds.GetData());
		} else {
			ERROR("VisIt cannot accept chunk data of type \"" 
				<< t << "\"");
			return false;
		}
		return true;
	}
	return false;
}
#endif

}
