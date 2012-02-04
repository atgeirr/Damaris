      program test
      include 'mpif.h'
      integer :: ierr
      integer :: comm
      integer :: is_client

      call MPI_INIT(ierr)
      comm = MPI_COMM_WORLD
      call df_mpi_start("mpi.xml",comm,is_client)
      if(is_client.GT.0) then
         call client_code()
         call df_kill_server(ierr)
         call df_finalize(ierr)
      endif
      call MPI_FINALIZE(ierr)

      end program test

      subroutine client_code()
      integer :: ierr
      real, dimension(0:63,16:31,0:3) :: mydata
      integer :: i,j,k,step

      do step = 0, 10

      do i =  0, 63
      do j = 16, 31
      do k =  0,  3
          mydata(i,j,k) = i*j*k
      end do
      end do
      end do

      call df_write("my group/my variable",1,mydata,ierr)
      call df_signal("my event",1,ierr)
   
      end do

      end subroutine
