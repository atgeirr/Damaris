/* 
 * File:   DifferentialWriter.h
 * Author: cata
 *
 * Created on July 5, 2013, 5:36 PM
 */

#ifndef DIFFERENTIALWRITER_H
#define	DIFFERENTIALWRITER_H

#include "data/Variable.hpp"
#include "Writer.hpp"
#include "stdio.h"
#include "mpi.h"
#include "StorageManager.hpp"
#include "zlib.h"

typedef struct {             
              int size;
              int iteration;
              int pid;
              void* data;
                           
}LastWrittenChunk;


namespace Damaris{
    class DifferentialWriter : public Writer{
        public:
            Variable *var;
            MPI_File damarisFile;
            DifferentialWriter(Variable* v);
            DifferentialWriter(const DifferentialWriter& orig);
            virtual ~DifferentialWriter();
            bool Write();
            bool Write(int iteration);
        private:
            std::vector<LastWrittenChunk*> lastChunks;
            int lastIteration;
            std::string getPath();
            LastWrittenChunk* getLastBlock (int pid);
            void writeXorData (Chunk* chunk, int pid, uLongf sizeDest);
            void createChunkStructure(DifferentialChunk &chunkInfo,int pid,int iteration,int size, int bid,int uncompressedSize);
    };
}

#endif	/* DIFFERENTIALWRITER_H */

