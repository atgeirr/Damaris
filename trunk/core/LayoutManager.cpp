/*******************************************************************
This file is part of Damaris.

Damaris is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Damaris is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Damaris.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
/**
 * \file MetadataManager.cpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#include "core/Debug.hpp"
#include "core/LayoutManager.hpp"
#include "data/TypeLayout.hpp"
#include "data/BoxLayout.hpp"

namespace Damaris {

void LayoutManager::Init(const Model::Data& model)
{
    Create<TypeLayout>(Model::Type::short_,"short");
    Create<TypeLayout>(Model::Type::int_,"int");
    Create<TypeLayout>(Model::Type::integer,"integer");
    Create<TypeLayout>(Model::Type::long_,"long");
    Create<TypeLayout>(Model::Type::float_,"float");
    Create<TypeLayout>(Model::Type::real,"real");
    Create<TypeLayout>(Model::Type::double_,"double");
    Create<TypeLayout>(Model::Type::char_,"char");
    Create<TypeLayout>(Model::Type::character,"character");

    Model::Data::layout_const_iterator l(model.layout().begin());
    for(;l != model.layout().end(); l++)
    {
		if(l->type() == "string" or l->type() == "label") {
			if(not (l->dimensions() == "?")) {
				ERROR("string and label -based layouts should have an unlimited dimension "
					<< "descriptor. Layout \"" << l->name() << "\" won't be built.");
				continue;
			}
		}
		Create<BoxLayout>(*l,(std::string)l->name());
    }
}

}
