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
#include "data.hpp"


/*typedef struct {
              //TODO?? remove the id; 
              int id;
              int iteration;
              unsigned int size;  
              int pid;
}ChunkInfo;*/

/*typedef struct {
    //source
    int pid;
    //iteration number;
    int iteration;
    //chunk size;
    int size;
    //block id;
    int bid;
    //uncompressed size
    int uncompressedSize;
    //type size
    int typeSize;
}DifferentialChunk;*/

namespace Damaris {
    class StorageManager {
	private:                              
                StorageManager() {}
                static std::map<int,Writer*> writersMap;                
                static std::map<std::string,Reader*>readersMap;
	public:
                static std::string basename;                
                static void Init(const Model::Storage& s);               
                static Writer* GetWriterFor(Variable* v);
                static Reader* GetReaderFor(Variable* v,std::string magicNumber);
                static long int GetPreviousMagicNumber();
                //static std::string getPath(std::string magicNumber);
                //static void init(std::string magicNumber, MPI_File damarisFile);

    };

}

#endif	/* STORAGEMANAGER_H */

