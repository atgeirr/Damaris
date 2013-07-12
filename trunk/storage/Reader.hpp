/* 
 * File:   Reader.h
 * Author: cata
 *
 * Created on June 18, 2013, 5:56 PM
 */

#ifndef READER_H
#define	READER_H
#include "data/DataSpace.hpp"
#include <vector>

namespace Damaris{
    class Reader {
        public:
            Reader(){}
            virtual ~Reader(){}
            //virtual bool Read()=0;
            virtual std::vector<DataSpace*> Read(int iteration) = 0;
        private:

        };
}


#endif	/* READER_H */

