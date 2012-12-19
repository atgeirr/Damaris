real, dimension(0:3,:) :: mydata
integer :: i, ierr
...
call df_write_block("my group/temperature",i,mydata(i),ierr)