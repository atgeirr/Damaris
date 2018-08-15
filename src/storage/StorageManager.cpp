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
#include <boost/algorithm/string.hpp>

#include "util/Debug.hpp"
#include "storage/StorageManager.hpp"
#include "data/VariableManager.hpp"
#include "storage/Store.hpp"
#include "storage/NullStore.hpp"
#ifdef HAVE_HDF5_ENABLED
#include "storage/HDF5Store.hpp"
#endif

namespace damaris {

void StorageManager::Init(const model::Storage& mdl)
{
	model::Storage::store_const_iterator s(mdl.store().begin());
	for(; s != mdl.store().end(); s++) {
		switch(s->type()) {
		case model::StoreType::null :
			Create<NullStore>(*s); break;
		case model::StoreType::HDF5 :
#ifdef HAVE_HDF5_ENABLED
			Create<HDF5Store>(*s); break;
#else
			CFGERROR("Requesting the creation of HDF5 store \""
			<< s->name() << "\" but HDF5 is not enabled");
#endif
		default: break;
		}
	}
	
	VariableManager::iterator v = VariableManager::Begin();
	for(; v != VariableManager::End(); v++) {
		const model::Variable& mdl = (*v)->GetModel();
		if(mdl.store() == "#") continue;
		
		std::vector<std::string> stores;
		boost::split(stores, mdl.store(), boost::is_any_of(",; "));
		
		std::vector<std::string>::iterator s = stores.begin();
		for(; s != stores.end(); s++) {
			std::shared_ptr<Store> st = Search(*s);
			if(st) {
				st->AddVariable(*v);
			} else {
				CFGERROR("Unknown store \""
				<< *s << "\" for variable \"" 
				<< (*v)->GetName() << "\"");
			}
		}
	}
}

void StorageManager::Update(int32_t iteration)
{
	iterator s = Begin();
	for(; s != End(); s++) {
		(*s)->Output(iteration);
	}
}

}
