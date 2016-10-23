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

#ifndef DAMARIS_SIGNALS_H
#define DAMARIS_SIGNALS_H

#include "damaris/buffer/SharedMemory.hpp"

namespace damaris {

/**
 * Type of signal. HEADER defines a small signal carying the type of the
 * body, BODY is the tag of a data signal, i.e. carying more information.
 */
typedef enum {
	DAMARIS_SIG_HEADER,
	DAMARIS_SIG_BODY,
	DAMARIS_SIG_DATA
} SignalPart;

/**
 * Signal types for bodies.
 */
typedef enum {
	DAMARIS_SIG_CONNECT,
	DAMARIS_SIG_STOP,
	DAMARIS_SIG_NEXT_ITERATION,
	DAMARIS_SIG_NEXT_ITERATION_ERR,
	DAMARIS_SIG_CALL_ACTION,
	DAMARIS_SIG_BCAST_ACTION,
	DAMARIS_SIG_WRITE,
	DAMARIS_SIG_REMOTE_WRITE,
	DAMARIS_SIG_COMMIT,
	DAMARIS_SIG_CLEAR
} SignalType;

/**
 * HeaderMessage is the structure used to carry the type and size of a message
 * that follows the header.
 */
struct HeaderMessage {
	int32_t source_;
	SignalType type_;
	size_t size_;
	
	HeaderMessage();
};

/**
 * EventMessage are used to send events from clients to servers.
 */
struct EventMessage {
	int32_t id_; /*!< ID of the action to call. */
	int32_t source_; /*!< source that triggered the action. */
	int32_t iteration_; /*!< iteration at which the action is triggered. */
};

#ifndef MAX_DIMENSIONS
#define MAX_DIMENSIONS 16
#endif

/**
 * WriteMessage are used to notify a server that some data has been written
 * in shared memory.
 */
struct WriteMessage {
	int32_t id_; /*!< ID of the variable that was written. */
	int32_t source_; /*!< source that wrote the variable. */
	int32_t iteration_; /*!< iteration at which the variable was written.*/
	int32_t block_; /*!< domain id that was written. */
	int64_t dim_; /*!< number of dimensions. */
	int64_t ubounds_[MAX_DIMENSIONS]; /*!< upper bounds of the block. */
	int64_t lbounds_[MAX_DIMENSIONS]; /*!< lower bounds of the block. */
	int64_t gbounds_[MAX_DIMENSIONS]; /*!< global bounds at time of creation. */
	size_t  ghosts_[2*MAX_DIMENSIONS]; /*!< ghost zones */
	Handle handle_; /*!< Handle in shared memory where the data is. */
};

typedef WriteMessage CommitMessage;
/**
 * RemoteWriteMessage are used in dedicated node mode.
 */
struct RemoteWriteMessage{
	int32_t id_; /*!< ID of the variable that was written. */
	int32_t source_; /*!< source that wrote the variable. */
	int32_t iteration_; /*!< iteration at which the variable was written.*/
	int32_t block_; /*!< domain id that was written. */
	int64_t dim_; /*!< number of dimensions. */
	int64_t ubounds_[MAX_DIMENSIONS]; /*!< upper bounds of the block. */
	int64_t lbounds_[MAX_DIMENSIONS]; /*!< lower bounds of the block. */
	int64_t gbounds_[MAX_DIMENSIONS]; /*!< global bounds at time of creation. */
	size_t  ghosts_[2*MAX_DIMENSIONS]; /*!< ghost zones */
	size_t size_; //size of the data that will sent to dedicated node
};

/**
 * ClearMessage are sent by a client to notify a server that the data can be
 * cleared safely.
 */
struct ClearMessage {
	int32_t id_; /*!< Id of the variable that has to be cleared. */
	int32_t source_; /*!< source that sent the message. */
	int32_t iteration_; /*!< iteration of the block to clear. */
	int32_t block_; /*!< domain id to clear. */
};

}

#endif
