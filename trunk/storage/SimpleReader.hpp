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


namespace Damaris{
    class SimpleReader : public Reader {
        public:
            SimpleReader(Variable* v,std::string magicNumber) ;            
            virtual ~SimpleReader();             
            std::map<int,DataSpace*> Read(int iteration);
        private:             
            ChunkInfo* jumpBackwards(int iteration);
            ChunkInfo* jumpForward(int iteration);
            //int lastIteration;

        };
}

#endif	/* SIMPLEREADER_H */

