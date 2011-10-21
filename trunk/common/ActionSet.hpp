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
 * \file ActionsSet.hpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#ifndef __DAMARIS_ACTIONS_SET_H
#define __DAMARIS_ACTIONS_SET_H

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/shared_ptr.hpp>

#include "Tags.hpp"
#include "Action.hpp"

namespace Damaris {
/**
 * ActionsSet is a container based on Boost Multi-Index, it indexes Actions by
 * ID and by names (each ID appears only one, each name appears only once).
 */
typedef boost::multi_index_container<
        boost::shared_ptr<Action>,
        boost::multi_index::indexed_by<
                boost::multi_index::ordered_unique<boost::multi_index::tag<by_id>,
                        boost::multi_index::const_mem_fun<Action,int,&Action::getID> >,
                boost::multi_index::ordered_unique<boost::multi_index::tag<by_name>,
                        boost::multi_index::const_mem_fun<Action,std::string,&Action::getName> >
        >
> ActionSet;

}

#endif
