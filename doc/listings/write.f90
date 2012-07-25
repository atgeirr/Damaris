real, dimension(:) :: mydata
integer :: iteration
integer :: ierr
...
call df_write("my group/temperature",iteration,mydata,ierr)