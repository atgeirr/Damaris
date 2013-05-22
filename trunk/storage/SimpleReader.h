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
#include "StorageManager.h"

namespace Damaris{
    class SimpleReader : public Reader {
    public:
        Variable* var;
        SimpleReader(Variable* v);
        SimpleReader(const SimpleReader& orig);
        virtual ~SimpleReader();
        bool Read(int iteration,Chunk* chunk);        
    private:
        std::string getPath();       
       

    };
}

#endif	/* SIMPLEREADER_H */

