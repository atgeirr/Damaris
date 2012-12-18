use Damaris
type(c_ptr) :: cptr
integer :: ierr
real,  pointer :: mydata(:,:,:)
    ...
cptr = df_alloc("my group/temperature",ierr)
call c_f_pointer(cptr,mydata,[64,16,4])
    ...
call df_commit("my group/my variable",ierr)