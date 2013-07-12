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
#include "storage/Reader.hpp"
#include "storage/Writer.hpp"
#include <string.h>


typedef struct {
              //TODO?? remove the id; 
              int id;
              int iteration;
              unsigned int size;                     
}ChunkInfo;

typedef struct {
    //source
    int pid;
    //iteration number;
    int iteration;
    //chunk size;
    int size;
    //block id;
    int bid;
    int uncompressedSize;
}DifferentialChunk;

namespace Damaris {
    class StorageManager {
	private:                              
                StorageManager() {}
                static std::map<int,Writer*> writersMap;
                static std::map<int,Reader*> readersMap;
	public:
                static std::string basename;                
                static void Init(const Model::Storage& s);               
                static Writer* GetWriterFor(Variable* v);
                static Reader* GetReaderFor(Variable* v,std::string magicNumber);
                static long int GetPreviousMagicNumber();

    };

}

#endif	/* STORAGEMANAGER_H */

