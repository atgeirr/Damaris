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
//#include "StorageManager.hpp"
#include "data.hpp"
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
            DifferentialReader(Variable* v,std::string magicNumber);
            virtual ~DifferentialReader();
            std::map<int,DataSpace*> Read(int iteration);             
        private:
            std::vector<LastReadChunk*> lastChunks;
            std::vector<LastReadChunk*> partialLastChunks;           
            DifferentialChunk* jumpBackwards(int iteration);
            DifferentialChunk* jumpForward(int iteration);
            LastReadChunk* getLastBlock (int pid);
    };
}
#endif	/* DIFFERENTIALREADER_H */

