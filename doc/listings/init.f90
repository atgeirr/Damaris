program sim
        include 'mpif.h'
        integer :: ierr
        call df_initialize("config.xml",MPI_COMM_WORLD,ierr)
        call sim_main_loop()
        call df_finalize(ierr)
end program sim

subroutine sim_main_loop()
       integer i, ierr
       do i = 1, 100
         ! do something
         call df_end_iteration(ierr)
       end do
end subroutine
