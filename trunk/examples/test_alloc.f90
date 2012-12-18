      program test
      use Damaris

      type(c_ptr) :: cptr
      integer :: ierr,id
      integer :: i,j,k
      real,  pointer :: mydata(:,:,:)
      
 
      id     = 42 

      call df_initialize("config.xml",MPI_COMM_WORLD,ierr)

      cptr = df_alloc("my group/my variable",ierr)
      call c_f_pointer(cptr,mydata,[64,16,4])
 
      do i =  1, 64
      do j =  1, 16
      do k =  1, 4
         mydata(i,j,k) = i*j*k
      end do
      end do
      end do 
 
      call df_commit("my group/my variable",1,ierr);

      call df_signal("my event",ierr);
 
      call df_finalize(ierr)

      end program test
