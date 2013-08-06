/* 
 * File:   data.h
 * Author: cata
 *
 * Created on June 6, 2013, 3:26 PM
 */

#ifndef DATA_H
#define	DATA_H
typedef struct {
              //TODO?? remove the id; 
              int id;
              int iteration;
              unsigned int size;  
              int pid;
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
    //uncompressed size
    int uncompressedSize;
    //type size
    int typeSize;
}DifferentialChunk;

#endif	/*DATA_H */

