real, dimension(0:3,:) :: mydata
integer :: i, ierr
...
call damaris_write_block_f("my group/temperature",i,mydata(i),ierr)