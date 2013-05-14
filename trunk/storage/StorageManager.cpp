/* 
 * File:   StorageManager.cpp
 * Author: cata
 * 
 * Created on May 6, 2013, 1:02 PM
 */

#include "StorageManager.h"
#include "mpi.h"
#include "SimpleWriter.h"

namespace Damaris {

void StorageManager::Init(const Model::Storage& s)
{
    
}

Reader* StorageManager::GetReaderFor(Variable* v)
{
    Reader* reader = NULL; // TODO create an instance of SimpleReader
    
    return reader;
    
}

Writer* StorageManager::GetWriterFor(Variable* v)
{
    Writer* w = NULL; // TODO create an instance of SimpleWriter
    return w;
}

/*static MPI_Datatype StorageManager::DamTypeToMPI(const Damaris::Model::Type& t) 
{
	switch(v) {
		case Model::Type::int :
			return MPI_INT;
                case Model::Type::short:
			return MPI_SHORT;
                case Model::Type::float :
			return MPI_FLOAT;
		case Model::Type::long :
			return MPI_LONG;
		case Model::Type::char :
			return MPI_CHAR;
		
	}
	return VISIT_VARTYPE_SCALAR;
}*/

}
