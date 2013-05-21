/* 
 * File:   StorageManager.h
 * Author: cata
 *
 * Created on May 6, 2013, 1:02 PM
 */

#ifndef STORAGEMANAGER_H
#define	STORAGEMANAGER_H

#include "xml/Model.hpp"
#include "data/Variable.hpp"
#include "storage/Reader.h"
#include "storage/Writer.h"

namespace Damaris {
    class StorageManager {
	private:
	StorageManager() {}
	
	public:
        static void Init(const Model::Storage& s);
        static Writer* GetWriterFor(Variable* v);
        static Reader* GetReaderFor(Variable* v);

    };

}

#endif	/* STORAGEMANAGER_H */
