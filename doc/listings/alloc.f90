use Damaris
type(c_ptr) :: cptr
integer :: ierr
real,  pointer :: mydata(:,:,:)
    ...
cptr = damaris_alloc_f("my group/temperature",ierr)
call c_f_pointer(cptr,mydata,[64,16,4])
    ...
call damaris_commit_f("my group/my variable",ierr)
    ...
call damaris_clear_f("my group/my variable",ierr)