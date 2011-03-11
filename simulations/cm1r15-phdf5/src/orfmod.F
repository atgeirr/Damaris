module orfiomod

implicit none
!ORF only ranks and comms are needed here
!Groups do not need to be here, can be defined
!in param.F only
integer &
        niv,njv,nkv,ibv,iev,jbv,jev,kbv,kev,                            &
        myroot,mysubgroupcomm,mcmrootcomm, mcmrootrank, &
        mybxgroupcomm, mytxgroupcomm,                                   &
        mybxgrouprank, mytxgrouprank,                                   &
        mylygroupcomm, myrygroupcomm,                                   &
        mylygrouprank, myrygrouprank,                                   &
        mybxgroup,mytxgroup,mylygroup,myrygroup,                        &
        cores_per_mcm,corex,corey,mymcmi,mymcmj,mymcmid,nmcm,           &
        x1dgroup,x1drank,x1dcomm,x1dtype, &
        y1dgroup,y1drank,y1dcomm,y1dtype, &
        rootonlyrank, rootonlygroup, &
        mygroupoffseti,mygroupoffsetj, &
        myfulloffseti,myfulloffsetj, &
        mygroup,mcmrootgroup,                       &
        mysubgrouprank,mygroupi,mygroupj,                                    &
        corextype,coreytype,corexytype,corexyztype,                         &
        corexztype,coreyztype, &
        corexyzutype,corexyzvtype,corexyzwtype,                            &
        ioid,iocomm,iorank,                                             &
        iomyi,iomyj,ionumi,ionumj,ionodex,ionodey,                      &
        iox0,iox1,ioy0,ioy1

character(len=100) filedir
integer, parameter :: ctrlunit = 77
integer :: ctrldropslice,ctrldrophist,ctrldroprestart,ctrlexit
logical :: wr2d,wr3d
logical, parameter :: iodebug = .false.
logical, parameter :: iodebug2 = .false.


contains

subroutine reset_control_file
         open(unit=ctrlunit,file='cm1control.txt',status='unknown')
         write(ctrlunit,*) "CM1 CONTROL FILE. Changing this file will signal to CM1 that you want to"
         write(ctrlunit,*) "do something including: (1) drop a slice file (2) drop a full history"
         write(ctrlunit,*) "(3) drop a restart file (4) exit gracefully. You can mix and match."
         write(ctrlunit,*) "File is read at the beginning of each time step. 0=no 1=yes"
         write(ctrlunit,*) "---slice---history---restart---exit---"
         write(ctrlunit,*) "     0        0         0       0"
         close(ctrlunit)
end subroutine reset_control_file

subroutine read_control_file
         open(unit=ctrlunit,file='cm1control.txt',status='old')
         read(ctrlunit,*)
         read(ctrlunit,*)
         read(ctrlunit,*)
         read(ctrlunit,*)
         read(ctrlunit,*)
         read(ctrlunit,*) ctrldropslice,ctrldrophist,ctrldroprestart,ctrlexit
         close(ctrlunit)
end subroutine read_control_file

end module orfiomod

module orfswathsmod
      integer slcfrq,swindex
      integer, parameter :: norfswaths = 5
      real, dimension (:,:,:,:), allocatable :: orfswaths
end module orfswathsmod
