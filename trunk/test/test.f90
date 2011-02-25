      program test
      
      integer :: ierr,id
      integer*8 :: layout
      character*10 :: var
      character*4 :: poke
      character*12 :: config
      real, dimension(0:63,16:31,2:3) :: mydata
      integer :: i,j,k
 
      id     = 42 
      var    = "myvariable"
      poke   = "test"
      config = "myconfig.xml"
      layout = 0

      do i =  0, 63
      do j = 16, 31
      do k =  2,  3
          mydata(i,j,k) = i*j*k
      end do
      end do
      end do
 
      call dc_initialize("myconfig.xml",id,ierr)
      call dc_define_A3D_layout(layout,4,0,63,16,31,2,3)
      call dc_write("myvar",1,mydata,layout,ierr)
      call dc_poke(poke,1,ierr)
      call dc_free_layout(layout);
      call dc_finalize(ierr)

      end program test
