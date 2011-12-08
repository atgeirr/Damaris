      program test
     
      integer :: ierr,id
      integer*8 :: chunk_handle
      real, dimension(0:31,8:15,0:3) :: mydata      
      integer, dimension(3) :: istart, iend
      integer :: i,j,k
 
      id     = 42
      istart = (/0,8,0/)
      iend   = (/31,15,3/)

      do i =  0, 31
      do j =  8, 15
      do k =  0,  3
          mydata(i,j,k) = i*j*k
      end do
      end do
      end do
 
      call df_initialize("config.xml",id,ierr)
      call df_chunk_set(3,istart,iend,chunk_handle)
      call df_chunk_write(chunk_handle,"my group/my variable",1, &
                          mydata,ierr)
      call df_signal("my event",1,ierr)
      call df_chunk_free(chunk_handle)
      call df_finalize(ierr)

      end program test
