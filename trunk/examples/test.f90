      program test
     
      integer :: ierr,id
      real, dimension(0:63,16:31,0:3) :: mydata
      integer :: i,j,k
 
      id     = 42 

      do i =  0, 63
      do j = 16, 31
      do k =  2,  3
          mydata(i,j,k) = i*j*k
      end do
      end do
      end do
 
      call df_initialize("config.xml",id,ierr)
      call df_write("my group/my variable",1,mydata,ierr)
      call df_signal("my event",1,ierr)
      call df_finalize(ierr)

      end program test
