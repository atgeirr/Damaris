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

#ifndef __DAMARIS_TAGS_H
#define __DAMARIS_TAGS_H

namespace damaris {

/**
 * Tag used in indexes to retrieve objects by their name.
 */
struct by_name {};
/**
 * Tag used in indexes to retrieve objects by their id.
 */
struct by_id {};
/**
 * Tag used in indexes to retrieve objects by their source.
 */
struct by_source {};
/**
 * Tag used in indexes to retrieve objects by their iteration.
 */
struct by_iteration {};
/**
 * Tag used in indexes to retrieve objects by the set of keys of the indexes.
 */
struct by_any {};

/**
 * Structure used as a template parameters in shared-memory related functions.
 */
struct open {};
/**
 * Structure used as a template parameters in shared-memory related functions.
 */
struct create {};
}

#endif
