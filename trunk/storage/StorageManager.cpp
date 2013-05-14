/* 
 * File:   StorageManager.cpp
 * Author: cata
 * 
 * Created on May 6, 2013, 1:02 PM
 */

#include "StorageManager.h"
#include "mpi.h"
#include "SimpleWriter.h"

StorageManager::StorageManager(const Damaris::Model::Storage& s)
                : Configurable<Model::Storage>(mdl)
{
    
}

StorageManager::StorageManager(const StorageManager& orig) {
}

StorageManager::~StorageManager() {
}

StorageManager::Init(const Damaris::Model::Storage& s){
    
}

StorageManager::GetReaderFor(Damaris::Variable* v){
    Damaris::Reader reader;
    
    return reader;
    
}

StorageManager::GetWriterFor(Damaris::Variable* v){
    Damaris::SimpleWriter w;
    //w.Write(v);
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