/* 
 * File:   Reader.h
 * Author: cata
 *
 * Created on May 6, 2013, 1:03 PM
 */

#ifndef READER_H
#define	READER_H


namespace Damaris{
    class Reader {
        public:
            Reader();
            virtual ~Reader();
            virtual bool Read() = 0;
	    virtual bool Read(int iteration) = 0;
        private:

    };
}
#endif	/* READER_H */

