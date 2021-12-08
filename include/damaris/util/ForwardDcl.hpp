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

#ifndef __DAMARIS_CLASS_LIST_H
#define __DAMARIS_CLASS_LIST_H

namespace damaris {
	
	// Data
	class Layout;
	class TypeLayout;
	class BoxLayout;
	class Variable;
	class Curve;
	class Mesh;
	class PointMesh;
	class RectilinearMesh;
	class CurvilinearMesh;
	class Parameter;
	class Block;
	
	// Actions
	class Action;
	class DynamicAction;
	class ScriptAction;
	class BoundAction;
	class GarbageCollector;
    class PyAction;
	template<class BASE, typename MODEL> class NodeAction;
	
	// Utility
	class Observer;
	class Observable;
	
	// Communication
	class Callback;
	class Reactor;
	class MPI2Reactor;
	class Channel;
	
	// Buffers
	class Buffer;
	class SharedMemoryBuffer;
	
	// Managers
	template<typename T> class Manager;
	class VariableManager;
	class LayoutManager;
	class ParameterManager;
	class ActionManager;
	
	// Clients and Servers
	class Client;
	class StandaloneClient;
	class Server;

}

#endif
