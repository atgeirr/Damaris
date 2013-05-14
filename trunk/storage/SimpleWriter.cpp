/* 
 * File:   SimpleWriter.cpp
 * Author: cata
 * 
 * Created on May 6, 2013, 2:56 PM
 */

#include "SimpleWriter.h"
#include "mpi.h"

SimpleWriter::SimpleWriter() {
}

SimpleWriter::SimpleWriter(const SimpleWriter& orig) {
}

SimpleWriter::~SimpleWriter() {
}

SimpleWriter::Write(Damaris::Variable * v){
    
    
    int typeSize,iteration;
    unsigned int dimensions;    
    MPI_File damarisFile;
    MPI_Status status;
    
    /*Open file*/
    MPI_File_open(MPI_COMM_SELF, "write",MPI_MODE_RDWR | MPI_MODE_CREATE,&damarisFile);
     
    Damaris::Variable::iterator it = v->Begin();
    
    for (;it!= v->End();it++){
        
        Damaris::Chunk *chunk = it->get();
        
        dimensions = chunk->GetDimensions(); 
        iteration = chunk->GetIteration();
        void* data = dataSpace->Data();  
        Damaris::DataSpace* dataSpace= chunk->GetDataSpace();   
        Model::Type t = v->GetLayout()->GetType();
        typeSize = Damaris::Types::basicTypeSize(&t);
        //write the variable ID
        MPI_File_write (damarisFile,&v->GetID(),1,MPI_INT,status);
        //write the iteration number
        MPI_File_write (damarisFile,&iteration,1,MPI_INT,status);
        //write data size
        MPI_File_write(damarisFile, &(dimensions*typeSize),1,MPI_INT, status);
        //write the data
        MPI_File_write(damarisFile, data, dimensions*typeSize, MPI_BYTE ,status);
    }
  
    /* Close the file */
    MPI_File_close(&myfile);
    
    
}

