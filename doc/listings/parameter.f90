integer :: w_in, w_out
integer :: ierr
...
call damaris_parameter_get_f("w",w_in,SIZEOF(w_in),ierr)

w_out = 4
call damaris_parameter_set_f("w",w_out,SIZEOF(w_out),ierr)