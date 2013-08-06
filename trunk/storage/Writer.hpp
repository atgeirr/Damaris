/* 
 * File:   Writer.h
 * Author: cata
 *
 * Created on May 6, 2013, 1:03 PM
 */

#ifndef WRITER_H
#define	WRITER_H

#include "data/Variable.hpp"
#include <mpi.h>
#include "data.hpp"

namespace Damaris{
    class Writer {
        public:
            Variable* var;
            MPI_File damarisFile; 
            Writer(){}
            virtual ~Writer(){}
            virtual bool Write()=0;
            virtual bool Write(int iteration) = 0;
        protected:
            std::string getPath();                          
            int lastIteration; 
            void initFile(); 
    };
}
#endif	/* WRITER_H */

