use Damaris
type(c_ptr) :: cptr
integer :: iteration, ierr
real,  pointer :: mydata(:,:,:)
    ...
cptr = df_alloc("my group/temperature",iteration,ierr)
call c_f_pointer(cptr,mydata,[64,16,4])
    ...
call df_commit("my group/my variable",iteration,ierr)