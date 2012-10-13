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
 * \file ChunksSet.hpp
 * \date February 2012
 * \author Matthieu Dorier
 * \version 0.4
 */
#ifndef __DAMARIS_CHUNKS_SET_H
#define __DAMARIS_CHUNKS_SET_H

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/shared_ptr.hpp>

#include "core/Tags.hpp"
#include "data/Chunk.hpp"

namespace Damaris {

namespace bmi = boost::multi_index;

/**
 * ChunksSet is a container based on Boost Multi-Index, it indexes Chunks by
 * source and by iteration within a Variable.
 */
typedef boost::multi_index_container<
        boost::shared_ptr<Chunk>,
        bmi::indexed_by<
                bmi::ordered_non_unique<bmi::tag<by_source>,
                        bmi::const_mem_fun<Chunk,int,&Chunk::getSource> >,
                bmi::ordered_non_unique<bmi::tag<by_iteration>,
                        bmi::const_mem_fun<Chunk,int,&Chunk::getIteration> >,
		bmi::ordered_non_unique<bmi::tag<by_block>,
			bmi::const_mem_fun<Chunk,int,&Chunk::getBlock> >,
		bmi::ordered_unique<bmi::tag<by_any>,
			bmi::composite_key<Chunk,
				bmi::const_mem_fun<Chunk,int,&Chunk::getSource>,
				bmi::const_mem_fun<Chunk,int,&Chunk::getIteration>,
				bmi::const_mem_fun<Chunk,int,&Chunk::getBlock>
			> 
		>
        >
> ChunkSet;

typedef ChunkSet::index<by_source>::type ChunkIndexBySource;
typedef ChunkSet::index<by_iteration>::type ChunkIndexByIteration;
typedef ChunkSet::index<by_block>::type ChunkIndexByBlock;
typedef ChunkSet::index<by_any>::type ChunkIndex;

}

#endif
