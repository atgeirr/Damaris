      program test
     
      use iso_c_binding

      interface
        type(c_ptr) function df_alloc(varname,it,ierr)
          use iso_c_binding
          character*64 :: varname
          integer :: it, ierr
        end function df_alloc
      end interface

      type(c_ptr) :: cptr
      integer :: ierr,id
      integer :: length
      character*128,  pointer :: mydata(:)
      
 
      id     = 42 
      length = 128

      call df_initialize("config.xml",id,ierr)

      cptr = df_alloc("my string",1,ierr)
      call c_f_pointer(cptr,mydata,[length])

      mydata = "bonjour"
 
      call df_commit("my string",1,ierr);
 
      call df_finalize(ierr)

      end program test
