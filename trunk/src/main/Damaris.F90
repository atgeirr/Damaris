!*******************************************************************
! This file is part of Damaris.
!
! Damaris is free software: you can redistribute it and/or modify
! it under the terms of the GNU Lesser General Public License as published by
! the Free Software Foundation, either version 3 of the License, or
! (at your option) any later version.
!
! Damaris is distributed in the hope that it will be useful,
! but WITHOUT ANY WARRANTY; without even the implied warranty of
! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
! GNU General Public License for more details.
!
! You should have received a copy of the GNU Lesser General Public License
! along with Damaris.  If not, see <http://www.gnu.org/licenses/>.
!********************************************************************
      module damaris
      use iso_c_binding
      interface

        type(c_ptr) function damaris_alloc_f(varname,ierr)
          use iso_c_binding
          character(len=*) :: varname
          integer :: it, ierr
        end function damaris_alloc_f

        type(c_ptr) function damaris_alloc_block_f(varname,block,ierr)
          use iso_c_binding
          character(len=*) :: varname
          integer :: block, ierr
        end function damaris_alloc_block_f

      end interface
      end module
