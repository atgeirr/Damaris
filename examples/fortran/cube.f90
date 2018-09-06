!**************************************************************************
!This file is part of Damaris.

!Damaris is free software: you can redistribute it and/or modify
!it under the terms of the GNU Lesser General Public License as published by
!the Free Software Foundation, either version 3 of the License, or
!(at your option) any later version.

!Damaris is distributed in the hope that it will be useful,
!but WITHOUT ANY WARRANTY; without even the implied warranty of
!MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
!GNU Lesser General Public License for more details.

!You should have received a copy of the GNU Lesser General Public License
!along with Damaris.  If not, see <http://www.gnu.org/licenses/>.
!***************************************************************************/

program sim
    use mpi
    implicit none
    integer ::  ierr , is_client , comm


    call MPI_Init(ierr)
    call damaris_initialize_f ("cube.xml" ,MPI_COMM_WORLD, ierr )
    call damaris_start_f ( is_client ,  ierr )

    if ( is_client.gt.0 ) then
        !call damaris_client_comm_get (comm, ierr )
        call sim_main_loop()
        call damaris_stop_f (ierr)
    endif

    call damaris_finalize_f (ierr)
    call MPI_FINALIZE( ierr )
end program sim

subroutine sim_main_loop ()
    integer i , ierr
    integer m,n,p
    real,dimension(:,:,:),allocatable :: u1
    integer(4), parameter :: iterations = 50

    allocate(u1(1:100,                           &
                1:100,                           &
                1:100),STAT=ierr)

    ! man simulation loop
    do i = 1 , iterations
        do m=1 , 100
            do n=1 , 100
                do p=1 , 100
                    u1(m,n,p) = (m + n*10 + P*100)*i*10
                    if (n.eq.i) then
                        u1(m,n,p)=-1000
                    endif
                end do
            end do
         end do

        if (i.eq.1) then
            call write_coordinates()
        end if

        call damaris_write_f("space" , u1 , ierr)

        call damaris_end_iteration_f (ierr )
        print *, 'Cube Fortran Simulator - Iteration:',i,'/',iterations

        call sleep(2)
    end do

    deallocate(u1)
end subroutine

subroutine write_coordinates()
    !integer , save :: first_call =1
    integer mpi_err, counter

    !! coordinates
    real,dimension(:),allocatable :: cordx
    real,dimension(:),allocatable :: cordy
    real,dimension(:),allocatable :: cordz

    !if (first_call.eq.1) then
    !    first_call=0

        allocate(cordx(0:100),STAT=ierr)
        allocate(cordy(0:100),STAT=ierr)
        allocate(cordz(0:100),STAT=ierr)
        if (ierr.ne.0) return

        do counter=0,100
            cordx(counter)=counter
        enddo

        do counter=0,100
            cordy(counter)=counter
        enddo

        do counter=0,100
            cordz(counter)=counter
        enddo

        !write(6,'(a)') 'This is the first call to expose_data_to_damaris()'
        !print *, 'The values of cordx[0,10,n1]: ',cordx(0),cordx(10),cordx(n1)

        call damaris_write_f("coord/x" , cordx , mpi_err)
        call damaris_write_f("coord/y" , cordy , mpi_err)
        call damaris_write_f("coord/z" , cordz , mpi_err)

        deallocate(cordx)
        deallocate(cordy)
        deallocate(cordz)
    !endif
end subroutine
