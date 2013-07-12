/* 
 * File:   Writer.h
 * Author: cata
 *
 * Created on May 6, 2013, 1:03 PM
 */

#ifndef WRITER_H
#define	WRITER_H

namespace Damaris{
    class Writer {
    public:
        Writer(){}
        virtual ~Writer(){}
        virtual bool Write()=0;
	virtual bool Write(int iteration) = 0;
    private:

    };
}
#endif	/* WRITER_H */

