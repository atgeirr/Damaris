/* 
 * File:   SimpleReader.h
 * Author: cata
 *
 * Created on May 7, 2013, 1:33 AM
 */

#ifndef SIMPLEREADER_H
#define	SIMPLEREADER_H
#include "Reader.h"
namespace Damaris{
    class SimpleReader : public Reader {
    public:
        SimpleReader();
        SimpleReader(const SimpleReader& orig);
        virtual ~SimpleReader();
        //bool Read(Damaris::Variable *v);
    private:

    };
}

#endif	/* SIMPLEREADER_H */

