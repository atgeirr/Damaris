integer :: w_in, w_out
integer :: ierr
...
call df_parameter_get("w",w_in,ierr)

w_out = 4
call df_parameter_set("w",w_out,ierr)