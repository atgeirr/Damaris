#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <png.h>

#include "common/ChunkSet.hpp"
#include "common/Variable.hpp"
#include "common/MetadataManager.hpp"

extern "C" {

static void copy_chunk(int w, int h, Damaris::Chunk* chunk, png_bytep* rows);
static void draw_png(int width, int height, int32_t step,
                Damaris::ChunkIndexByIteration::iterator it,
                Damaris::ChunkIndexByIteration::iterator& end);

void call_visit(const std::string event, int32_t step, int32_t src)
{
	Damaris::MetadataManager* db = Damaris::MetadataManager::getInstance();
	// open the variable
	Damaris::Variable* v = db->getVariable("images/julia");
	if(v != NULL) {
		Damaris::ChunkIndexByIteration::iterator end;
		Damaris::ChunkIndexByIteration::iterator it = v->getChunksByIteration(step,end);

		int w = v->getLayout()->getExtentAlongDimension(0);
		int h = v->getLayout()->getExtentAlongDimension(1);

		draw_png(w,h,step,it,end);
		v->eraseChunk(it);
	}
}

static void draw_png(int width, int height, int32_t step, 
		Damaris::ChunkIndexByIteration::iterator it,
		Damaris::ChunkIndexByIteration::iterator& end)
{
	char* filename = (char*)malloc(16*sizeof(char));
	sprintf(filename,"julia%03d.png",step);

	FILE* fp = fopen(filename,"wb");
	
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	png_byte color_type = 2;
	png_byte bit_depth  = 8;	

	png_init_io(png_ptr, fp);
	
	png_set_IHDR(png_ptr, info_ptr, width, height,
                     bit_depth, color_type, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
        for (int y=0; y < height; y++) {
		int size = png_get_rowbytes(png_ptr,info_ptr);
                row_pointers[y] = (png_byte*)malloc(size);
	}

	while(it != end) {
		copy_chunk(width,height,it->get(),row_pointers);	
		it++;
	}

	png_write_info(png_ptr, info_ptr);
	png_write_image(png_ptr, row_pointers);

	png_write_end(png_ptr, NULL);

	fclose(fp);
	for(int y=0; y < height; y++) {
		free(row_pointers[y]);
	}
	free(row_pointers);
}

static void copy_chunk(int w, int h, Damaris::Chunk* chunk, png_bytep* rows) 
{
	if(chunk == NULL) return;

	int wc = chunk->getEndIndex(0) - chunk->getStartIndex(0) + 1;
	int hc = chunk->getEndIndex(1) - chunk->getStartIndex(1) + 1;

	int offset_x = chunk->getStartIndex(0);
	int offset_y = chunk->getStartIndex(1);

	for(int j = 0; j < hc; j++) {
		png_byte* row = rows[j+offset_y];
		for(int i = 0; i < wc; i++) {
			png_byte* b = &(row[(i+offset_x)*3]);
			memset(b,(int)(*((char*)(chunk->data()) + i*hc+j)),3);
	}
	}
}

}
