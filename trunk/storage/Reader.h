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
            Reader(const Reader& orig);
            virtual ~Reader();
            virtual bool Read(Damaris::Chunk* c)=0;
        private:

    };
}
#endif	/* READER_H */

