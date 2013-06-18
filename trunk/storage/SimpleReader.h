/* 
 * File:   SimpleReader.h
 * Author: cata
 *
 * Created on May 7, 2013, 1:33 AM
 */

#ifndef SIMPLEREADER_H
#define	SIMPLEREADER_H

#include "Reader.h"
#include "data/Variable.hpp"
#include "data/DataSpace.hpp"
#include "mpi.h"
#include "StorageManager.h"

namespace Damaris{
    class SimpleReader : public Reader {
    public:
        Variable* var;
        MPI_File damarisFile;
        SimpleReader(Variable* v);
        SimpleReader(const SimpleReader& orig);
        virtual ~SimpleReader();
        DataSpace* Read(int iteration);  
      
    private:
        std::string getPath();  
        ChunkInfo* jumpBackwards(int iteration);
        ChunkInfo* jumpForward(int iteration);
        int lastIteration; 
       

    };
}

#endif	/* SIMPLEREADER_H */

