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
#include "common/Debug.hpp"
#include "common/LayoutManager.hpp"

namespace Damaris {

void LayoutManager::Init(const Model::Data& model)
{
    Model::Data::layout_const_iterator l(model.layout().begin());
    for(;l != model.layout().end(); l++)
    {
		Create<Layout>(*l,(std::string)l->name());
    }
}

}
