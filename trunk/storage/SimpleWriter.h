/* 
 * File:   SimpleWriter.h
 * Author: Cata
 *
 * Created on May 6, 2013, 2:56 PM
 */



#ifndef SIMPLEWRITER_H
#define	SIMPLEWRITER_H  

#include "data/Variable.hpp"
#include "Writer.h"
#include "stdio.h"
#include "mpi.h"
#include "StorageManager.h"

namespace Damaris{
 
    class SimpleWriter : public Writer { 
     public:              
          Damaris::Variable* var;  
          MPI_File damarisFile;
          SimpleWriter(Variable* v);
          SimpleWriter(const SimpleWriter& orig);
          virtual ~SimpleWriter();        
          bool Write(); 
          bool Write(int iteration);               
      private:    
          int lastIteration; 
          std::string getPath();       
          void createChunkStructure(ChunkInfo &chunkInfo,int id,int iteration,int size);

    };
}
#endif	/* SIMPLEWRITER_H */

