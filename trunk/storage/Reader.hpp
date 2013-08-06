/* 
 * File:   Reader.h
 * Author: cata
 *
 * Created on June 18, 2013, 5:56 PM
 */

#ifndef READER_H
#define	READER_H
#include "data/DataSpace.hpp"
#include "data/Variable.hpp"
#include <vector>
#include <map>
#include <mpi.h>
#include "data.hpp"

namespace Damaris{
    class Reader {
        public:
            Variable* var;
            MPI_File damarisFile;   
            Reader(){};
            virtual ~Reader(){}              
            virtual std::map<int,DataSpace*> Read(int iteration) = 0;            
        protected:
            std::string getPath(std::string magicNumber);                          
            int lastIteration; 
            void initFile(std::string magicNumber);              

   };
}


#endif	/* READER_H */

