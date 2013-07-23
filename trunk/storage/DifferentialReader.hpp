/* 
 * File:   DifferentialReader.h
 * Author: cata
 *
 * Created on July 5, 2013, 5:36 PM
 */

#ifndef DIFFERENTIALREADER_H
#define	DIFFERENTIALREADER_H

#include "Reader.hpp"
#include "data/Variable.hpp"
#include "data/DataSpace.hpp"
#include "mpi.h"
#include "StorageManager.hpp"
#include "zlib.h"

typedef struct {             
              int size;
              int iteration;
              int pid;
              Bytef* data;
                           
}LastReadChunk;

namespace Damaris {
    class DifferentialReader : public Reader {
        public:
            Variable* var;
            MPI_File damarisFile;
            DifferentialReader(Variable* v,std::string magicNumber);
            DifferentialReader(const DifferentialReader& orig);
            virtual ~DifferentialReader();
            std::map<int,DataSpace*> Read(int iteration);    
            //td::vector<DataSpace*> Read(int iteration);    
        private:
            std::vector<LastReadChunk*> lastChunks;
            std::vector<LastReadChunk*> partialLastChunks;
            std::string getPath(std::string magicNumber);  
            DifferentialChunk* jumpBackwards(int iteration);
            DifferentialChunk* jumpForward(int iteration);
            LastReadChunk* getLastBlock (int pid);
            int lastIteration; 

    };
}
#endif	/* DIFFERENTIALREADER_H */

