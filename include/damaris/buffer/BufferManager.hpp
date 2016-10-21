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

#ifndef __DAMARIS_BUFFER_MANAGER_H
#define __DAMARIS_BUFFER_MANAGER_H

#include <string>

#include "damaris/model/Model.hpp"
#include "damaris/util/Manager.hpp"
#include "damaris/buffer/Buffer.hpp"

namespace damaris {

/**
 * BufferManager holds pointers to all instances of Buffer.
 * These Buffers can be retrieved by their name or by a unique ID.
 */
class BufferManager : public Manager<Buffer> {

};

}

#endif
