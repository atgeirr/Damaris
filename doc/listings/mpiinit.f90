program sim
      include 'mpif.h'
      integer :: ierr,id
      integer :: comm
      integer :: is_client
      call MPI_INIT(ierr)
      comm = MPI_COMM_WORLD
      call df_mpi_init_and_start("config.xml",comm,is_client)
      if(is_client.GT.0) then
         call df_mpi_get_client_comm(comm)
         call sim_main_loop(comm)
         call df_kill_server(ierr)
         call df_finalize(ierr)
      endif
      call MPI_FINALIZE(ierr)
end program sim

subroutine sim_main_loop(comm)
       integer :: comm
       integer :: i, ierr
       do i = 1, 100
         ! do something
         call df_end_iteration(i,ierr)
       end do
end subroutine
