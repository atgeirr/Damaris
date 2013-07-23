/* 
 * File:   SimpleReader.h
 * Author: cata
 *
 * Created on May 7, 2013, 1:33 AM
 */

#ifndef SIMPLEREADER_H
#define	SIMPLEREADER_H

#include "Reader.hpp"
#include "data/Variable.hpp"
#include "data/DataSpace.hpp"
#include "mpi.h"
#include "StorageManager.hpp"

namespace Damaris{
    class SimpleReader : public Reader {
        public:
            Variable* var;
            MPI_File damarisFile;
            SimpleReader(Variable* v,std::string magicNumber);
            SimpleReader(const SimpleReader& orig);
            virtual ~SimpleReader();
            //std::vector<DataSpace*> Read(int iteration);  
            std::map<int,DataSpace*> Read(int iteration) = 0;
        private:
            std::string getPath(std::string magicNumber);  
            ChunkInfo* jumpBackwards(int iteration);
            ChunkInfo* jumpForward(int iteration);
            int lastIteration; 


        };
}

#endif	/* SIMPLEREADER_H */

