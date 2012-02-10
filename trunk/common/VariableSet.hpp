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
 * \file VariableSet.hpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_VARIABLES_SET_H
#define __DAMARIS_VARIABLES_SET_H

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/shared_ptr.hpp>

#include "Tags.hpp"
#include "Variable.hpp"

namespace Damaris {
/**
 * VariablesSet is a container based on Boost Multi-Index, it indexes Variables by
 * ID and by names (each ID appears only one, each name appears only once).
 */
typedef boost::multi_index_container<
        boost::shared_ptr<Variable>,
        boost::multi_index::indexed_by<
                boost::multi_index::ordered_unique<boost::multi_index::tag<by_id>,
                        boost::multi_index::const_mem_fun<Variable,int,&Variable::getID> >,
                boost::multi_index::ordered_unique<boost::multi_index::tag<by_name>,
                        boost::multi_index::const_mem_fun<Variable,const std::string&,&Variable::getName> >,
		bmi::ordered_non_unique<bmi::tag<by_any>,
			bmi::composite_key<Variable,
				bmi::const_mem_fun<Variable,const std::string&, &Variable::getName>,
				bmi::const_mem_fun<Variable,int, &Variable::getID>
			>
		>
        >
> VariableSet;

typedef VariableSet::index<by_name>::type VariableIndexByName;
typedef VariableSet::index<by_id>::type VariableIndexById;
typedef VariableSet::index<by_any>::type VariableIndex;

}

#endif
