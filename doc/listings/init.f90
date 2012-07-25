program sim
        include 'mpif.h'
        integer :: ierr,id
        id = 0
        call df_initialize("config.xml",id,ierr)
        call sim_main_loop()
        call df_finalize(ierr)
end program sim

subroutine sim_main_loop()
       integer i, ierr
       do i = 1, 100
         ! do something
         call df_end_iteration(i,ierr)
       end do
end subroutine
