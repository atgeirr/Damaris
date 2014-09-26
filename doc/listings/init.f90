program sim
        include 'mpif.h'
        integer :: ierr, is_client, comm
        call MPI_INIT(ierr)
        call damaris_initialize_f("config.xml",MPI_COMM_WORLD,ierr)
        call damaris_start_f(is_client,ierr)
        if(is_client.gt.0) then
            call damaris_client_comm_get(comm,ierr)
            call sim_main_loop()
            call damaris_stop_f(ierr)
        endif
        call damaris_finalize_f(ierr)
        call MPI_FINALIZE(ierr)
end program sim

subroutine sim_main_loop(comm)
       integer comm
       integer i, ierr
       do i = 1, 100
         ! do something
         call damaris_end_iteration_f(ierr)
       end do
end subroutine
