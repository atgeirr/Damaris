integer :: ierr
integer*8 :: handle
integer, dimension(3) :: istart, iend
...
call df_chunk_set(3,istart,iend,handle)
call df_chunk_write(handle,"my group/temperature",iteration,mydata,ierr)
call df_chunk_free(handle)