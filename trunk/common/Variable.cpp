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
 * \file Variable.cpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */

#include "common/Variable.hpp"

namespace Damaris {
	
Variable::Variable(int i, const std::string& n, Layout* l)
{
	id = i;
	name = n;
	layout = l;
}

Variable::Variable()
{
}

bool Variable::operator==(const Variable &another)
{
	bool c = true;
	c = c && another.name.compare(name) == 0;
	c = c && another.id == id;
	return c;
}

void Variable::attachChunk(Chunk* chunk)
{
	chunks.insert(boost::shared_ptr<Chunk>(chunk));
}

ChunkIndexBySource::iterator Variable::getChunksBySource(int source,
	ChunkIndexBySource::iterator& end)
{
	ChunkIndexBySource::iterator it = chunks.get<by_source>().find(source);
	end = chunks.get<by_source>().end();
	return it;
}

ChunkIndexByIteration::iterator Variable::getChunksByIteration(int iteration,
	ChunkIndexByIteration::iterator& end) 
{
        ChunkIndexByIteration::iterator it = chunks.get<by_iteration>().find(iteration);
	end = chunks.get<by_iteration>().end();
        return it;
}

ChunkIndex::iterator Variable::getChunks(ChunkIndex::iterator &end)
{
	end = chunks.get<by_iteration>().end();
	return chunks.get<by_iteration>().begin();
}

void Variable::eraseChunk(ChunkIndexByIteration::iterator &it)
{
	ChunkIndexByIteration::iterator end = chunks.get<by_iteration>().end();
	ChunkIndexByIteration::iterator j(it);
	while(j != end) {
		j->get()->remove();
		j++;
	}
        chunks.get<by_iteration>().erase(it);
}

void Variable::eraseChunk(ChunkIndexBySource::iterator &it)
{
	ChunkIndexBySource::iterator end = chunks.get<by_source>().end();
        ChunkIndexBySource::iterator j(it);
        while(j != end) {
                j->get()->remove();
                j++;
        }
	chunks.get<by_source>().erase(it);
}

}
