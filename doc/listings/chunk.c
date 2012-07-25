int start[] = {2,4,32};
int end[] = {4,5,64};
...
chunk_h handle = DC_chunk_set(3,start,end);
int err = DC_chunk_write(handle,"my group/temperature",iteration,data);
DC_chunk_free(handle);