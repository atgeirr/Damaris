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

#ifndef __DAMARIS_BLOCK_INDEX_H
#define __DAMARIS_BLOCK_INDEX_H

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>

#include "damaris/util/Tags.hpp"
#include "damaris/data/Block.hpp"

namespace damaris {

namespace bmi = boost::multi_index;

/**
 * BlockIndex is a container based on Boost Multi-Index, it indexes Blocks by
 * source, by iteration and by id within a Variable object.
 */
typedef boost::multi_index_container<
        boost::shared_ptr<Block>,
        bmi::indexed_by<
                bmi::ordered_non_unique<bmi::tag<by_source>,
                        bmi::const_mem_fun<Block,int,&Block::GetSource> >,
                bmi::ordered_non_unique<bmi::tag<by_iteration>,
                        bmi::const_mem_fun<Block,int,&Block::GetIteration> >,
		bmi::ordered_non_unique<bmi::tag<by_id>,
			bmi::const_mem_fun<Block,int,&Block::GetID> >,
		bmi::ordered_unique<bmi::tag<by_any>,
			bmi::composite_key<Block,
				bmi::const_mem_fun<Block,int,&Block::GetSource>,
				bmi::const_mem_fun<Block,int,&Block::GetIteration>,
				bmi::const_mem_fun<Block,int,&Block::GetID>
			> 
		>
        >
> BlockIndex;

typedef BlockIndex::index<by_source>::type 	BlocksBySource;
typedef BlockIndex::index<by_iteration>::type 	BlocksByIteration;
typedef BlockIndex::index<by_id>::type 		BlocksById;
typedef BlockIndex::index<by_any>::type 	Blocks;

}

#endif
