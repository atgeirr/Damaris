      program test
     
      use iso_c_binding

      interface
        type(c_ptr) function df_alloc(varname,it,ierr)
          use iso_c_binding
          character(len=*) :: varname
          integer :: it, ierr
        end function df_alloc
      end interface

      type(c_ptr) :: cptr
      integer :: ierr,id
      integer :: i,j,k
      real,  pointer :: mydata(:,:,:)
      
 
      id     = 42 

      call df_initialize("config.xml",id,ierr)

      cptr = df_alloc("my variable",1,ierr)
      call c_f_pointer(cptr,mydata,[64,16,2])
 
      do i =  1, 64
      do j =  1, 16
      do k =  1, 2
         mydata(i,j,k) = i*j*k
      end do
      end do
      end do 
 
      call df_commit("my variable",1,ierr);

      call df_signal("my event",1,ierr);
 
      call df_finalize(ierr)

      end program test
