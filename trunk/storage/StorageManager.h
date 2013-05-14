/* 
 * File:   StorageManager.h
 * Author: cata
 *
 * Created on May 6, 2013, 1:02 PM
 */

#ifndef STORAGEMANAGER_H
#define	STORAGEMANAGER_H
#include "Reader.h"
#include "Writer.h"

namespace Damaris {
    class StorageManager : public Configurable<Model::Storage> {
        
    public:
        StorageManager(const Damaris::Model::Storage& s);
        StorageManager(const StorageManager& orig);
        virtual ~StorageManager();
        static void Init(const Damaris::Model::Storage& s);
        static Damaris::Writer GetWriterFor(Damaris::Variable* v);
        static Damaris::Reader GetReaderFor(Damaris::Variable* v);
    private:

    };

}

#endif	/* STORAGEMANAGER_H */

