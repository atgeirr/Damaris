/* 
 * File:   SimpleWriter.h
 * Author: cata
 *
 * Created on May 6, 2013, 2:56 PM
 */

#ifndef SIMPLEWRITER_H
#define	SIMPLEWRITER_H
namespace Damaris{
    class SimpleWriter : public Writer{ 
        public:
            SimpleWriter();
            SimpleWriter(const SimpleWriter& orig);
            virtual ~SimpleWriter();
            bool Write(Damaris::Variable *v);
       private:

    };
}
#endif	/* SIMPLEWRITER_H */

