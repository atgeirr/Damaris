! parallel HDF5 output written by Leigh Orf <leigh.orf@cmich.edu>
!
! In namelist.input, you can choose which of the 2D and 3D variables are written.
!
! Three output options are available, set in namelist.input:
! output_format = 3 uses scale-offset lossy compression followed by gzip compression. (smallest files)
! output_format = 4 uses gzip compression (level 1) on floating point data and is lossless.
! output_format = 5 uses no compression whatsoever, just floating point (largest files)
!
! These compression options apply to 3D data. See comments in subroutine
! writefloat_hdf_unlimited for how to to add gzip compression to
! unlimited dimension arrays in parcel and stats hdf5 files. For more
! information on compression, please refer to the HDF5 user's guide.
!
!
! The modular nature of HDF5 makes it so that you do not need to worry
! about how the data was compressed when you read it - filters are
! activated automatically and your data appears as 32 bit floating point
! data regardless of the compression choice.
!
! Note that with the scaleoffset filter, you need to a scaling parameter
! which is related to absolute magnitude of your data. The values I have
! chosen work fine for me, but you may wish to tweak them. See hdf5
! docs.
!
! I have written this code to work with MPI only. If you wish to run
! with hdf5 output on a single core, you still need to compile with
! MPI and run with 1 MPI process.
!
! Each MPI rank is matched to one hdf5 file for history data. A
! directory is created which houses all of the hdf5 files at a given
! model dump time. Software has been developed to simplify dealing with
! this format where model domain is split amongst many files. Conversion
! code to create netcdf or vis5d format has also been created and is
! available. If you are interested in exploring additional compression
! options and/or conversion code contact Leigh Orf <leigh.orf@cmich.edu>
! or post to the cm1 CM1 google group.
!
! All hdf5 datasets now contain description and units attributes. New
! code has been added which writes both parcel (trajectory) data to hdf5
! format, as well as statistics data. Both are stored in a single file
! with an unlimited dimension in time, growing as the model progresses.
! This is similar to the netcdf code, which I used as a guide.
!
! Leigh Orf 1/11/11

!--------------------------------------------------------------------------------------------MARK


subroutine writeout_mult_hdf5(rtime,qname,sigma,sigmaf,xh,xf,uf,yh,yf,vf,mh,zh,mf,zf,    &
                pi0,prs0,rho0,th0,thv0,qv0,u0,v0,                        &
                zs,rain,sws,svs,sps,srs,sgs,sus,shs,thflux,qvflux,cdu,cdv,ce,dum1,dum2,dum3,dum4, &
                rho,prs,dbz,ua,dumu,va,dumv,wa,dumw,ppi,tha,        &
                qa,kmh,kmv,khh,khv,tkea,pta,num_soil_layers,   &
                lu_index,xland,mavail,tsk,tmn,tml,hml,huml,hvml,hfx,qfx,gsw,glw,tslb,   &
                radsw,rnflx,radswnet,radlwin,u10,v10,t2,q2,znt,ust,hpbl,zol,mol,br,  &
                dissten,thpten,qvpten,qcpten,qipten,upten,vpten,swten,lwten)
#ifdef HDFOUT
      use HDF5
      use orfiomod
#endif
#ifdef ORFSWATHS
      use orfswathsmod
#endif
      implicit none

      include 'input.incl'
      include 'constants.incl'
#ifdef MPI
      include 'mpif.h'
#endif



      real :: rtime
      character*3, dimension(maxq) :: qname
      real, dimension(kb:ke) :: sigma
      real, dimension(kb:ke+1) :: sigmaf
      real, dimension(ib:ie)   :: xh
      real, dimension(ib:ie+1) :: xf,uf
      real, dimension(jb:je)   :: yh
      real, dimension(jb:je+1) :: yf,vf
      real, dimension(ib:ie,jb:je,kb:ke) :: mh,zh,pi0,prs0,rho0,th0,thv0,qv0
      real, dimension(ib:ie,jb:je,kb:ke+1) :: mf,zf
      real, dimension(itb:ite,jtb:jte) :: zs
      real, dimension(ib:ie,jb:je,nrain) :: rain,sws,svs,sps,srs,sgs,sus,shs
      real, dimension(ib:ie,jb:je) :: thflux,qvflux,cdu,cdv,ce
      real, dimension(ib:ie,jb:je,kb:ke) :: dum1,dum2,dum3,dum4,rho,prs,dbz
      real, dimension(ib:ie+1,jb:je,kb:ke) :: u0,ua,dumu
      real, dimension(ib:ie,jb:je+1,kb:ke) :: v0,va,dumv
      real, dimension(ib:ie,jb:je,kb:ke+1) :: wa,dumw
      real, dimension(ib:ie,jb:je,kb:ke) :: ppi,tha
      real, dimension(ibm:iem,jbm:jem,kbm:kem,numq) :: qa
      real, dimension(ibc:iec,jbc:jec,kbc:kec) :: kmh,kmv,khh,khv
      real, dimension(ibt:iet,jbt:jet,kbt:ket) :: tkea
      real, dimension(ibp:iep,jbp:jep,kbp:kep,npt) :: pta
      integer, intent(in) :: num_soil_layers
      integer, dimension(ibl:iel,jbl:jel), intent(in) :: lu_index
      real, dimension(ib:ie,jb:je), intent(in) :: tsk
      real, dimension(ibl:iel,jbl:jel), intent(in) :: xland,mavail,tmn,tml,hml,huml,hvml,hfx,qfx,gsw,glw
      real, dimension(ibl:iel,jbl:jel,num_soil_layers), intent(in) :: tslb
      real, dimension(ni,nj), intent(in) :: radsw,rnflx,radswnet,radlwin
      real, dimension(ibl:iel,jbl:jel), intent(in) :: u10,v10,t2,q2,znt,ust,hpbl,zol,mol,br
      real, dimension(ib:ie,jb:je,kb:ke), intent(in) :: dissten
      real, dimension(ibb:ieb,jbb:jeb,kbb:keb), intent(in) :: thpten,qvpten,qcpten,qipten,upten,vpten
      real, dimension(ibr:ier,jbr:jer,kbr:ker), intent(in) :: swten,lwten


#ifdef HDFOUT
! We don't need these, can use temp arrays that are passed to this routine
! Just use dum1(1:ni,1:nj,1:nk)
      real, dimension(ni,nj,nk) :: ds
      real, dimension(ni,nj) :: ds2
      real, dimension(1:nk) :: zhcol,u0col,v0col,th0col,pi0col,rho0col,pres0col,qv0col
      real, dimension(1:nk+1) :: zfcol
      real, dimension(1:ni) :: xfcol,xhcol !Note we will lose last point
      real, dimension(1:nj) :: yfcol,yhcol !Note we will lose last point

      integer i,j,k,n
      character(len=120)  description
      character(len=20)   units
      character(len=20)   varname
      character(len=160)  filename,filename_2d
      character(LEN=20) :: qvarname
      character(len=8) chid
      character(len=4) cid
      character(len=5) ctime

!     character(len=100) filedir
!     character(len=109) dircmd

      integer ierror
      integer(HID_T) :: file_id,file_2d_id,plist_id
      integer(HID_T) :: grid_group_id, mesh_group_id, basestate_group_id, twod_group_id, threed_group_id,threed_basestate_group_id
      integer(HID_T) :: twodfull_id,twodfull_mesh_id,twodfull_grid_id,twodfull_shear_id,twodfull_base_id;
      integer :: info
      character(LEN=4),  parameter :: grid_group_name = 'grid'
      character(LEN=4),  parameter :: mesh_group_name = 'mesh'
      character(LEN=9),  parameter :: basestate_group_name = 'basestate'
      character(LEN=12),  parameter :: threed_basestate_group_name = '3d_basestate'
      character(LEN=2),  parameter :: twod_group_name = '2d'
      character(LEN=6),  parameter :: twodfull_group_name = '2dfull'
      character(LEN=2),  parameter :: threed_group_name = '3d'
      character(LEN=5),  parameter :: shear_group_name = 'shear'
      integer sd_id, sds_id, status, dim_id, data_type
      integer ix,iy
      integer rank,tag
      integer itime,lt,flag
      integer source, dest
      integer, dimension(MPI_STATUS_SIZE) :: mpi_status

      real, dimension(ionumi) :: ioxfcol,ioxhcol,ibuf
      real, dimension(ionumj) :: ioyfcol,ioyhcol,jbuf
      ! ORF pass these to 1d write, pain in the ass
      integer(HSIZE_T), DIMENSION(1) :: dims,count
      integer(HSSIZE_T), DIMENSION(1) :: offset

! ORF for Lou's buoyancy calculation (at 1 km)
      integer ki10m,ki50m,ki1,ki2,ki3,ki4,ki5
      real, dimension(1:ni,1:nj) :: b,the,qt,thv
      real :: tx,cpm,qvs
      real :: rslf

      write(ctime,102)int(rtime)
      write(cid,103)ioid
102   format(i5.5)
103   format(i4.4)
#ifdef EATPOO
            if (len_trim(output_path) .eq. 0) then
                  if(myid.eq.0)then
                     write(outfile,*) 'empty output_path'
                     write(outfile,*) 'defaulting to current working directory'
                  endif
                  output_path='.'
            endif
            filedir = trim(output_path)//'/'//trim(output_basename)//'.'//ctime//'.cdir/'
            dircmd = 'mkdir -p '// trim(filedir)
            if (myid.eq.0) call system(dircmd)
            call MPI_BARRIER (mysubgroupcomm,ierr)
#endif
            filename = trim(filedir)//trim(output_basename)//'.'//ctime//'_'//cid//'.cm1hdf5'
            write(cid,103) 0
            filename_2d = trim(filedir)//trim(output_basename)//'.'//ctime//'_'//cid//'.2Dcm1hdf5'
            if(myid.eq.0)write(outfile,*) 'myid,filename=',ioid,'   ',trim(filename)
            if(myid.eq.0)call FFLUSH(outfile)

            call h5open_f (ierror)

            if (wr3d) then
                  CALL h5pcreate_f(H5P_FILE_ACCESS_F, plist_id, ierror)
                  info = MPI_INFO_NULL !WTF?
                  CALL h5pset_fapl_mpio_f(plist_id, mysubgroupcomm, info, ierror) !ORF iocomm != mysubgroupcomm
                  call h5fcreate_f(trim(filename),H5F_ACC_TRUNC_F,file_id,ierror,access_prp=plist_id)
                  CALL h5pclose_f(plist_id, ierror)

                  call h5gcreate_f(file_id,grid_group_name,grid_group_id,ierror)      
                  call h5gcreate_f(file_id,mesh_group_name,mesh_group_id,ierror)      
                  call h5gcreate_f(file_id,basestate_group_name,basestate_group_id,ierror)      
                  call h5gcreate_f(file_id,threed_basestate_group_name,threed_basestate_group_id,ierror)      
                  call h5gcreate_f(file_id,twod_group_name,twod_group_id,ierror)      
                  call h5gcreate_f(file_id,threed_group_name,threed_group_id,ierror)      
            endif
!Slice file
            if (wr2d) then
                  CALL h5pcreate_f(H5P_FILE_ACCESS_F, plist_id, ierror)
                  info = MPI_INFO_NULL !WTF?
                  CALL h5pset_fapl_mpio_f(plist_id, MPI_COMM_WORLD, info, ierror)
                  call h5fcreate_f(trim(filename_2d),H5F_ACC_TRUNC_F,file_2d_id,ierror,access_prp=plist_id)
                  CALL h5pclose_f(plist_id, ierror)

                  call h5gcreate_f(file_2d_id,twodfull_group_name,twodfull_id,ierror)      
                  call h5gcreate_f(file_2d_id,mesh_group_name,twodfull_mesh_id,ierror)      
                  call h5gcreate_f(file_2d_id,grid_group_name,twodfull_grid_id,ierror)      
                  call h5gcreate_f(file_2d_id,basestate_group_name,twodfull_base_id,ierror)      
                  call h5gcreate_f(file_2d_id,shear_group_name,twodfull_shear_id,ierror)      
            endif

! 3d and 2d writes are hopelessly intertwined; however this saves a bit
! on redundancy (and lines of code) and you have the option of writing
! one or the other (or both at the same time - but I think performance
! will suffer if you do that)

            if (wr3d) then
                  varname='time'
                  description='model time'
                  units='seconds'
                  call h5_write_float(file_id,varname,description,units,rtime,.false.)

                  varname='dx'
                  description='grid spacing in the x direction. If E/W grid stretching, this should be somehwat representative'
                  units='meters'
                  if(wr3d)call h5_write_float(mesh_group_id,varname,description,units,dx,.false.)

                  varname='dy'
                  description='grid spacing in the y direction. If N/S grid stretching, this should be somehwat representative'
                  units='meters'
                  call h5_write_float(mesh_group_id,varname,description,units,dy,.false.)

                  varname='nx'
                  description='number of gridpoints spanning the east/west direction of full the model domain'
                  units='index'
                  call h5_write_int  (grid_group_id,varname,description,units,nx,.false.)

                  varname='ny'
                  description='number of gridpoints spanning the north/south direction of full the model domain'
                  units='index'
                  call h5_write_int  (grid_group_id,varname,description,units,ny,.false.)

                  varname='nz'
                  description='number of gridpoints spanning the vertical extent of the full the model domain'
                  units='index'
                  call h5_write_int  (grid_group_id,varname,description,units,nz,.false.)

                  varname='x0'
                  description='index of westmost point of 3d field in this hdf5 file with respect to full model domain'
                  units='index'
                  call h5_write_int  (grid_group_id,varname,description,units,iox0,.false.)

                  varname='x1'
                  description='index of eastmost point of 3d field in this hdf5 file with respect to full model domain'
                  units='index'
                  call h5_write_int  (grid_group_id,varname,description,units,iox1,.false.)

                  varname='y0'
                  description='index of southmost point of 3d field in this hdf5 file with respect to full model domain'
                  units='index'
                  call h5_write_int  (grid_group_id,varname,description,units,ioy0,.false.)

                  varname='y1'
                  description='index of northmost point of 3d field in this hdf5 file with respect to full model domain'
                  units='index'
                  call h5_write_int  (grid_group_id,varname,description,units,ioy1,.false.)

                  varname='myi'
                  description='east/west index of hdf5 file in 2d decomposition. (1 < myi <= nodex)'
                  units='index'
                  call h5_write_int  (grid_group_id,varname,description,units,iomyi,.false.)

                  varname='myj'
                  description='north/south index of hdf5 file in 2d decomposition. (1 < myj <= nodej)'
                  units='index'
                  call h5_write_int  (grid_group_id,varname,description,units,iomyj,.false.)

                  varname='ni'
                  description='number of gridpoints in east/west direction in this hdf5 file for 2d and 3d data'
                  units='index'
                  call h5_write_int  (grid_group_id,varname,description,units,ionumi,.false.)

                  varname='nj'
                  description='number of gridpoints in north/south direction in this hdf5 file for 2d and 3d data'
                  units='index'
                  call h5_write_int  (grid_group_id,varname,description,units,ionumj,.false.)

                  varname='nodex'
                  description='number of hdf5 files spanning the full east/west extent of the model domain'
                  units='index'
                  call h5_write_int  (grid_group_id,varname,description,units,ionodex,.false.)

                  varname='nodey'
                  description='number of hdf5 files spanning the full north/south extent of the model domain'
                  units='index'
                  call h5_write_int  (grid_group_id,varname,description,units,ionodey,.false.)

            endif !if wr3d

            do k=1,nk
                zhcol(k)=zh(1,1,k)
                u0col(k) = u0(1,1,k)
                v0col(k) = v0(1,1,k)
                th0col(k) = th0(1,1,k)
                pi0col(k) = pi0(1,1,k)
                rho0col(k) = rho0(1,1,k)
                pres0col(k) = p00*(pi0(1,1,k))**cpdrd
                qv0col(k) = qv0(1,1,k)
            enddo

            do k=1,nk+1
                zfcol(k)=zf(1,1,k)
            enddo

! ORF 2/1/11
! The following are specifically written to 2D slice/swath files for shear study
! Look for ORFSWATHS below for additional 2D variables

! -----------------------------begin only 2D file stuff----------------------------------------

              i=1
              j=1
              k=1
            do while( zf(i,j,k).lt.10.0 )
              k = k + 1
            enddo
            ki10m=k
            k=1
            do while( zf(i,j,k).lt.50.0 )
              k = k + 1
            enddo
            ki50m=k
            k=1
            do while( zf(i,j,k).lt.1000.0 )
              k = k + 1
            enddo
            ki1=k
            k=1
            do while( zf(i,j,k).lt.2000.0 )
              k = k + 1
            enddo
            ki2=k
            k=1
            do while( zf(i,j,k).lt.3000.0 )
              k = k + 1
            enddo
            ki3=k
            k=1
            do while( zf(i,j,k).lt.4000.0 )
              k = k + 1
            enddo
            ki4=k
            k=1
            do while( zf(i,j,k).lt.5000.0 )
              k = k + 1
            enddo
            ki5=k

      if (wr2d) then

            varname='time'
            description='model time'
            units='seconds'
            call h5_write_float(file_2d_id,varname,description,units,rtime,.true.)

            varname='dx'
            description='grid spacing in the x direction. If E/W grid stretching, this should be somehwat representative'
            units='meters'
            call h5_write_float(twodfull_mesh_id,varname,description,units,dx,.true.)

            varname='dy'
            description='grid spacing in the y direction. If N/S grid stretching, this should be somehwat representative'
            units='meters'
            call h5_write_float(twodfull_mesh_id,varname,description,units,dy,.true.)

            varname='nx'
            description='number of gridpoints spanning the east/west direction of full the model domain'
            units='index'
            call h5_write_int  (twodfull_grid_id,varname,description,units,nx,.true.)

            varname='ny'
            description='number of gridpoints spanning the north/south direction of full the model domain'
            units='index'
            call h5_write_int  (twodfull_grid_id,varname,description,units,ny,.true.)

            varname='nz'
            description='number of gridpoints spanning the vertical extent of the full the model domain'
            units='index'
            call h5_write_int  (twodfull_grid_id,varname,description,units,nz,.true.)

            varname='sl1'
            description='shear in layer 1'
            units='s^{-1}'
            call h5_write_float(twodfull_shear_id,varname,description,units,var1,.true.)

            varname='sl2'
            description='shear in layer 2'
            units='s^{-1}'
            call h5_write_float(twodfull_shear_id,varname,description,units,var2,.true.)

            varname='dl1'
            description='depth of layar 1'
            units='m'
            call h5_write_float(twodfull_shear_id,varname,description,units,var3,.true.)

            varname='dl2'
            description='depth of layar 2'
            units='m'
            call h5_write_float(twodfull_shear_id,varname,description,units,var4,.true.)

            varname='angle'
            description='angle of hodograph L'
            units='degrees'
            call h5_write_float(twodfull_shear_id,varname,description,units,var5,.true.)

            varname='u0'
            description='U (E/W wind) base state, (i.e., from sounding)'
            units='m/s'
            dims(1)=nz;count(1)=nz;offset(1)=0
            call h5_write_1d_float(twodfull_base_id,varname,description,units,u0col,nz,dims,count,offset,rootonlyrank)

            varname='v0'
            description='V (N/S wind) base state, (i.e., from sounding)'
            units='m/s'
            dims(1)=nz;count(1)=nz;offset(1)=0
            call h5_write_1d_float(twodfull_base_id,varname,description,units,v0col,nz,dims,count,offset,rootonlyrank)

            varname='pi0'
            description='Exner base state, (i.e., from sounding)'
            units='dimensionless'
            dims(1)=nz;count(1)=nz;offset(1)=0
            call h5_write_1d_float(twodfull_base_id,varname,description,units,pi0col,nz,dims,count,offset,rootonlyrank)

            varname='pres0'
            description='Pressure base state, (i.e., from sounding)'
            units='dimensionless'
            dims(1)=nz;count(1)=nz;offset(1)=0
            call h5_write_1d_float(twodfull_base_id,varname,description,units,pres0col,nz,dims,count,offset,rootonlyrank)

            varname='th0'
            description='potential temperature base state, (i.e., from sounding)'
            units='K'
            dims(1)=nz;count(1)=nz;offset(1)=0
            call h5_write_1d_float(twodfull_base_id,varname,description,units,th0col,nz,dims,count,offset,rootonlyrank)

            varname='rh0'
            description='density base state, (i.e., from sounding)'
            units='kg/m^3'
            dims(1)=nz;count(1)=nz;offset(1)=0
            call h5_write_1d_float(twodfull_base_id,varname,description,units,rho0col,nz,dims,count,offset,rootonlyrank)

            varname='qv0'
            description='mixing ratio base state, (i.e., from sounding)'
            units='kg/kg'
            dims(1)=nz;count(1)=nz;offset(1)=0
            call h5_write_1d_float(twodfull_base_id,varname,description,units,qv0col,nz,dims,count,offset,rootonlyrank)

            varname='zh'
            description='height AGL of scalars'
            units='m'
            dims(1)=nz;count(1)=nz;offset(1)=0
            call h5_write_1d_float(twodfull_mesh_id,varname,description,units,zhcol,nz,dims,count,offset,rootonlyrank)

            varname='zf'
            description='height AGL of w (on staggered C grid)'
            units='m'
            dims(1)=nz+1;count(1)=nz+1;offset(1)=0
            call h5_write_1d_float(twodfull_mesh_id,varname,description,units,zfcol,nz+1,dims,count,offset,rootonlyrank)

            varname='yh'
            description='N/S location of scalar points (model uses Arakawa C grid)'
            units='meters'
            dims(1)=ny; count(1)=nj; offset(1)=myfulloffsetj
            call h5_write_1d_float(twodfull_mesh_id,varname,description,units,yh(1:nj),nj,dims,count,offset,y1drank)

            varname='yf'
            description='N/S location of V points (model uses Arakawa C grid)'
            units='meters'
!Last y point will be missing in 3d file
!but not in 2d file. We Gather to root and tack on last point
!2D file full mesh
            dims(1)=ny+1; count(1)=ny+1; offset(1)=0
            call h5_write_1d_float_uv(twodfull_mesh_id,varname,description,units,yf(1:nj),nj,dims,count,offset,y1dcomm,y1drank,(nodex)*(nodey-1))

            varname='xh'
            description='E/W location of scalar points (model uses Arakawa C grid)'
            units='meters'
            dims(1)=nx; count(1)=ni; offset(1)=myfulloffseti
            call h5_write_1d_float(twodfull_mesh_id,varname,description,units,xh(1:ni),ni,dims,count,offset,x1drank)

            varname='xf'
            description='E/W location of U points (model uses Arakawa C grid)'
            units='meters'
!Last x point will be missing in 3d file
!but not in 2d file. We Gather to root and tack on last point
!2D file full mesh
            dims(1)=nx+1; count(1)=nx+1; offset(1)=0
            call h5_write_1d_float_uv(twodfull_mesh_id,varname,description,units,xf(1:ni),ni,dims,count,offset,x1dcomm,x1drank,nodex-1)
      endif !if wr2d

! -----------------------------end only 2D file stuff----------------------------------------

! NOTE xf and yf for 2D slices below, need different routine


      if(wr3d) then
            varname='u0'
            description='U (E/W wind) base state, (i.e., from sounding)'
            units='m/s'
            dims(1)=nz;count(1)=nz;offset(1)=0

! ORF 2/22/11 FIX NUKE
! I'm pretty sure all ranks should NOT be writing these nz arrays (which are
! identical for all ranks) and
! 'mysubgrouprank' should be '0' - mask out everything else

! AH! Just use mcmrootrank insteady of mysubgrouprank!!!!
! We need rootonly rank but on EACH MCM

            call h5_write_1d_float(basestate_group_id,varname,description,units,u0col,nz,dims,count,offset,mcmrootrank)

            varname='v0'
            description='V (N/S wind) base state, (i.e., from sounding)'
            units='m/s'
            dims(1)=nz;count(1)=nz;offset(1)=0
            call h5_write_1d_float(basestate_group_id,varname,description,units,v0col,nz,dims,count,offset,mcmrootrank)

            varname='th0'
            description='Potential temperature base state, (i.e., from sounding)'
            units='K'
            dims(1)=nz;count(1)=nz;offset(1)=0
            call h5_write_1d_float(basestate_group_id,varname,description,units,th0col,nz,dims,count,offset,mcmrootrank)

            varname='pi0'
            description='Dimensionless pressure (Exner function) base state, (i.e., from sounding)'
            units='dimensionless'
            dims(1)=nz;count(1)=nz;offset(1)=0
            call h5_write_1d_float(basestate_group_id,varname,description,units,pi0col,nz,dims,count,offset,mcmrootrank)

            varname='pres0'
            description='Pressure base state, (i.e., from sounding)'
            units='Pa'
            dims(1)=nz;count(1)=nz;offset(1)=0
            call h5_write_1d_float(basestate_group_id,varname,description,units,pres0col,nz,dims,count,offset,mcmrootrank)

            varname='rh0'
            description='Density base state, (i.e., from sounding)'
            units='kg/m^3'
            dims(1)=nz;count(1)=nz;offset(1)=0
            call h5_write_1d_float(basestate_group_id,varname,description,units,rho0col,nz,dims,count,offset,mcmrootrank)

            varname='qv0'
            description='Water vapor base state, (i.e., from sounding)'
            units='kg/kg'
            dims(1)=nz;count(1)=nz;offset(1)=0
            call h5_write_1d_float(basestate_group_id,varname,description,units,qv0col,nz,dims,count,offset,mcmrootrank)

            varname='zh'
            description='Height above ground of scalar points (model uses Arakawa C grid)'
            units='meters'
            dims(1)=nz;count(1)=nz;offset(1)=0
            call h5_write_1d_float(mesh_group_id,varname,description,units,zhcol,nz,dims,count,offset,mcmrootrank)

            varname='zf'
            description='Height above ground of W points (model uses Arakawa C grid)'
            units='meters'
            dims(1)=nz+1;count(1)=nz+1;offset(1)=0
            call h5_write_1d_float(mesh_group_id,varname,description,units,zfcol,nz+1,dims,count,offset,mcmrootrank)

            varname='yh'
            description='N/S location of scalar points (model uses Arakawa C grid)'
            units='meters'
            dims(1)=ionumj; count(1)=nj; offset(1)=mygroupoffsetj
            call h5_write_1d_float(mesh_group_id,varname,description,units,yh(1:nj),nj,dims,count,offset,mylygrouprank)

            varname='yf'
            description='N/S location of V points (model uses Arakawa C grid)'
            units='meters'
            dims(1)=ionumj; count(1)=nj; offset(1)=mygroupoffsetj
!Last y point will be missing in 3d file
            call h5_write_1d_float(mesh_group_id,varname,description,units,yf(1:nj),nj,dims,count,offset,mylygrouprank)
!but not in 2d file. We Gather to root and tack on last point
!2D file full mesh

            varname='xh'
            description='E/W location of scalar points (model uses Arakawa C grid)'
            units='meters'
            dims(1)=ionumi; count(1)=ni; offset(1)=mygroupoffseti
            call h5_write_1d_float(mesh_group_id,varname,description,units,xh(1:ni),ni,dims,count,offset,mybxgrouprank)
!2D file full mesh

            varname='xf'
            description='E/W location of U points (model uses Arakawa C grid)'
            units='meters'
            dims(1)=ionumi; count(1)=ni; offset(1)=mygroupoffseti
!Last x point will be missing in 3d file
            call h5_write_1d_float(mesh_group_id,varname,description,units,xf(1:ni),ni,dims,count,offset,mybxgrouprank)
!but not in 2d file. We Gather to root and tack on last point
!2D file full mesh

      ! terrain

           if (output_zs.eq.1.and.terrain_flag) then
             do j=1,nj
             do i=1,ni
               ds2(i,j)=zs(i,j)
             enddo
             enddo

             varname='zs'
             description='terrain height'
             units='meters'
             call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

           endif

           if (output_zh.eq.1.and.terrain_flag) then
             do k=1,nk
             do j=1,nj
             do i=1,ni
               ds(i,j,k)=zh(i,j,k)
             enddo
             enddo
             enddo

             varname='zh'
             description='height (above ground) of scalar grid points'
             units='meters'
             call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)

           endif
                  
      ! ORF 1/11/11 Following netcdf example, allow option for output of 3D
      ! base state. 1D basestate variables in /basestate group are always
      ! written but are not useful for folks running with terrain who require
      ! the 3D basestate.

           if (output_basestate.eq.1) then

                do k=1,nk
                do j=1,nj
                do i=1,ni
                  ds(i,j,k)=pi0(i,j,k)
                enddo
                enddo
                enddo

                varname='pi0'
                description = '3D base state nondimensional pressure (Exner function)'
                units = 'dimensionless'
                call h5_write_3d_float(threed_basestate_group_id,varname,description,units,ds,corexyztype,0,0,0)

                do k=1,nk
                do j=1,nj
                do i=1,ni
                  ds(i,j,k)=th0(i,j,k)
                enddo
                enddo
                enddo

                varname='th0'
                description = '3D base state potential temperature'
                units = 'K'
                call h5_write_3d_float(threed_basestate_group_id,varname,description,units,ds,corexyztype,0,0,0)

                do k=1,nk
                do j=1,nj
                do i=1,ni
                  ds(i,j,k)=prs0(i,j,k)
                enddo
                enddo
                enddo

                varname='p0'
                description = '3D base state pressure'
                units = 'Pa'
                call h5_write_3d_float(threed_basestate_group_id,varname,description,units,ds,corexyztype,0,0,0)

                do k=1,nk
                do j=1,nj
                do i=1,ni
                  ds(i,j,k)=qv0(i,j,k)
                enddo
                enddo
                enddo

                varname='qv0'
                description = '3D base state water vapor mixing ratio'
                units = 'kg/kg'
                call h5_write_3d_float(threed_basestate_group_id,varname,description,units,ds,corexyztype,0,0,0)

                do k=1,nk
                do j=1,nj
                do i=1,ni+1
                  dumu(i,j,k)=u0(i,j,k)
                enddo
                enddo
                enddo

                varname='u0'
                description = '3D base state x component of velocity on native Arakawa C grid'
                units = 'm/s'
                call h5_write_3d_float(threed_basestate_group_id,varname,description,units,dumu(1:ni+1,1:nj,1:nk),corexyztype,1,0,0)

                do k=1,nk
                do j=1,nj+1
                do i=1,ni
                  dumv(i,j,k)=v0(i,j,k)
                enddo
                enddo
                enddo

                varname='v0'
                description = '3D base state y component of velocity on native Arakawa C grid'
                units = 'm/s'
                call h5_write_3d_float(threed_basestate_group_id,varname,description,units,dumv(1:ni,1:nj+1,1:nk),corexyztype,0,1,0)

            endif !output_basestate.eq.1
       endif !if wr3d

       if(output_rain.eq.1) then
           do j=1,nj
           do i=1,ni
             ds2(i,j)=rain(i,j,1)
           enddo
           enddo
           varname='sfcrain'
           description='accumulated surface rainfall'
           units='cm'
           if(wr3d)call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)
           if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)
           if (nrain.eq.2) then
               do j=1,nj
               do i=1,ni
                 ds2(i,j)=rain(i,j,2)
               enddo
               enddo
               varname='sfcrain_trans'
               description='accumulated surface rainfall, translated with moving domain'
               units='cm'
               if(wr3d)call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)
               if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)
           endif
       endif

      if(output_sws.eq.1) then
           do j=1,nj
           do i=1,ni
             ds2(i,j)=sws(i,j,1)
           enddo
           enddo
           varname='maxsws'
           description='maximum windspeed at lowest level'
           units='m/s'
           if(wr3d)call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)
           if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

           do j=1,nj
           do i=1,ni
             ds2(i,j)=svs(i,j,1)
           enddo
           enddo
           varname='maxsvs'
           description='maximum vertical vorticity at lowest level'
           units='s^{-1}'
           if(wr3d)call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)
           if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

           do j=1,nj
           do i=1,ni
             ds2(i,j)=sps(i,j,1)
           enddo
           enddo
           varname='minsps'
           description='minimum pressure at lowest level'
           units='Pa'
           if(wr3d)call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)
           if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

           do j=1,nj
           do i=1,ni
             ds2(i,j)=srs(i,j,1)
           enddo
           enddo
           varname='maxsrs'
           description='maximum surface rainwater mixing ratio'
           units='kg/kg'
           if(wr3d)call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)
           if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

           do j=1,nj
           do i=1,ni
             ds2(i,j)=sgs(i,j,1)
           enddo
           enddo
           varname='maxsgs'
           description='maximum surface graupel/hail mixing ratio'
           units='kg/kg'
           if(wr3d)call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)
           if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

           do j=1,nj
           do i=1,ni
             ds2(i,j)=sus(i,j,1)
           enddo
           enddo
           varname='maxsus'
           description='maximum updraft velocity'
           units='m/s'
           if(wr3d)call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)
           if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

           do j=1,nj
           do i=1,ni
             ds2(i,j)=sus(i,j,1)
           enddo
           enddo
           varname='maxshs'
           description='maximum integrated updraft helicity'
           units='m^2/s^2'
           if(wr3d)call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)
           if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)


!ORF Here is where we write all the orfswaths stuff that I added
#ifdef ORFSWATHS

           if(wr2d) then
                do j=1,nj
                do i=1,ni
                  ds2(i,j)=orfswaths(i,j,1,1)
                enddo
                enddo

                varname='minthpert'
                description='minimum surface potential temperature perturbation'
                units='K'

                call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

                do j=1,nj
                do i=1,ni
                  ds2(i,j)=orfswaths(i,j,1,2)
                enddo
                enddo

                varname='wmax1km'
                description='Maximum updraft at 1 km'
                units='m/s'

                call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

                do j=1,nj
                do i=1,ni
                  ds2(i,j)=orfswaths(i,j,1,3)
                enddo
                enddo

                varname='wmin1km'
                description='Maximum downdraft at 1 km'
                units='m/s'

                call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

                do j=1,nj
                do i=1,ni
                  ds2(i,j)=orfswaths(i,j,1,4)
                enddo
                enddo

                varname='zetamax1km'
                description='Maximum vertical vorticity (zeta) at 1 km'
                units='s^{-1}'

                call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)
            endif !if wr2d
#endif
      endif !output_sws.eq.1


      if (nrain.eq.2) then
         if(output_rain.eq.1) then
              do j=1,nj
              do i=1,ni
                ds2(i,j)=rain(i,j,2)
              enddo
              enddo
              varname='maxrain_trans'
              description='accumulated surface rainfall, translated with moving domain'
              units='cm'
              if(wr3d)call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)
              if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)
          endif

          if(output_sws.eq.1) then
              do j=1,nj
              do i=1,ni
                ds2(i,j)=sws(i,j,2)
              enddo
              enddo
              varname='maxsws_trans'
              description='maximum windspeed at lowest level, translated with moving domain'
              units='m/s'
              if(wr3d)call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)
              if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

              do j=1,nj
              do i=1,ni
                ds2(i,j)=svs(i,j,2)
              enddo
              enddo
              varname='maxsvs_trans'
              description='maximum vertical vorticity at lowest level, translated with moving domain'
              units='s^{-1}'
              if(wr3d)call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)
              if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

              do j=1,nj
              do i=1,ni
                ds2(i,j)=sps(i,j,2)
              enddo
              enddo
              varname='maxsps_trans'
              description='minimum pressure at lowest level, translated with moving domain'
              units='Pa'
              if(wr3d)call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)
              if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

              do j=1,nj
              do i=1,ni
                ds2(i,j)=srs(i,j,2)
              enddo
              enddo
              varname='maxsrs_trans'
              description='maximum surface rainwater mixing ratio, translated with moving domain'
              units='kg/kg'
              if(wr3d)call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)
              if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

              do j=1,nj
              do i=1,ni
                ds2(i,j)=sgs(i,j,2)
              enddo
              enddo
              varname='maxsgs_trans'
              description='maximum surface graupel/hail mixing ratio, translated with moving domain'
              units='kg/kg'
              if(wr3d)call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)
              if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

              do j=1,nj
              do i=1,ni
                ds2(i,j)=sus(i,j,2)
              enddo
              enddo
              varname='maxsus_trans'
              description='maximum updraft velocity, translated with moving domain'
              units='m/s'
              if(wr3d)call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)
              if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

              do j=1,nj
              do i=1,ni
                ds2(i,j)=sus(i,j,2)
              enddo
              enddo
              varname='maxshs_trans'
              description='maximum integrated updraft helicity, translated with moving domain'
              units='m^2/s^2'
              if(wr3d)call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)
              if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

!ORF Here is where we write all the orfswaths stuff that I added
#ifdef ORFSWATHS

              if(wr2d) then
                   do j=1,nj
                   do i=1,ni
                     ds2(i,j)=orfswaths(i,j,2,1)
                   enddo
                   enddo
  
                   varname='minthpert_trans'
                   description='minimum surface potential temperature perturbation (translated)'
                   units='K'
  
                   call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)
  
                   do j=1,nj
                   do i=1,ni
                     ds2(i,j)=orfswaths(i,j,2,2)
                   enddo
                   enddo
  
                   varname='wmax1km_trans'
                   description='Maximum updraft at 1 km (translated)'
                   units='m/s'
  
                   call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)
  
                   do j=1,nj
                   do i=1,ni
                     ds2(i,j)=orfswaths(i,j,2,3)
                   enddo
                   enddo
  
                   varname='wmin1km_trans'
                   description='Maximum downdraft at 1 km (translated)'
                   units='m/s'
  
                   call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)
  
                   do j=1,nj
                   do i=1,ni
                     ds2(i,j)=orfswaths(i,j,2,4)
                   enddo
                   enddo
  
                   varname='zetamax1km_trans'
                   description='Maximum vertical vorticity (zeta) at 1 km (translated)'
                   units='s^{-1}'
  
                   call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)
              endif !if wr2d
#endif
           endif !output_sws.eq.1
      endif !nrain.eq.2

! Composite reflectivity

      if(output_dbz.eq.1.and.imoist.eq.1)then
          call calccref(dum1(ib,jb,1),dbz)
          do j=1,nj
          do i=1,ni
            ds2(i,j)=dum1(i,j,1)
          enddo
          enddo
          varname='cref'
          description='composite reflectivity'
          units='dbZ'
          if(wr3d)call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)
          if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)
          varname="dbzsfc"
          description = 'surface reflectivity'
          units = 'dBZ'
          do j=1,nj
          do i=1,ni
            ds2(i,j)=dbz(i,j,1)
          enddo
          enddo
          if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

          varname="dbz1km"
          description = '1km reflectivity'
          units = 'dBZ'
          do j=1,nj
          do i=1,ni
            ds2(i,j)=dbz(i,j,ki1)
          enddo
          enddo
          if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)
      endif

! Lou wants average buoyuancy below 1 km and temperature and sfc temp.

! Also theta-e at 1 km

!---------------- begin buoyancy calculation code stolen from parcel.F ------------------------!

        if(wr2d) then

              varname='ki1'
              description='1 km index'
              units='index'
              call h5_write_int(twodfull_shear_id,varname,description,units,ki1,.true.)

              b = 0.0

               IF(imoist.eq.1)THEN
           
             do k=1,ki1 
                 do j=1,nj
                 do i=1,ni
                   qt(i,j)=0.0
                 enddo
                 enddo
           
                 do j=1,nj
                 do i=1,ni
                   qt(i,j)=qt(i,j)+qa(i,j,k,nqv)
                 enddo
                 enddo
           
                 do n=nql1,nql2
                   do j=1,nj
                   do i=1,ni
                     qt(i,j)=qt(i,j)+qa(i,j,k,n)
                   enddo
                   enddo
                 enddo
           
                 do n=nqs1,nqs2
                   do j=1,nj
                   do i=1,ni
                     qt(i,j)=qt(i,j)+qa(i,j,k,n)
                   enddo
                   enddo
                 enddo
           
           
                 do j=1,nj
                 do i=1,ni
                   tx=(th0(i,j,k)+tha(i,j,k))*(pi0(i,j,k)+ppi(i,j,k))
                   cpm=cp+cpl*qa(i,j,k,nqv)
                   do n=nql1,nql2
                     cpm=cpm+cpl*qa(i,j,k,n)
                   enddo
                   qvs=rslf( prs(i,j,k) , tx )
                   the(i,j)=tx*((p00*(1.0+qa(i,j,k,nqv)*reps)      &
                                   /prs(i,j,k))**(rd/cpm))       &
                      *((qa(i,j,k,nqv)/qvs)**(-qa(i,j,k,nqv)*rv/cpm))    &
                      *exp((lv1-lv2*tx)*qa(i,j,k,nqv)/(cpm*tx))
                   thv(i,j)=(th0(i,j,k)+tha(i,j,k))*(1.0+reps*qa(i,j,k,nqv))/(1.0+qt(i,j))
                   b(i,j)=b(i,j)+g*( thv(i,j)/thv0(i,j,k) - 1.0 )
                 enddo
                 enddo

                 if (k.eq.1)then
                       varname='thetae_sfc'
                       description='Theta-e at surface'
                       units='K'
                       call h5_write_2d_float(twodfull_id,varname,description,units,the,corexytype,0,0,.true.)
                 endif
               enddo

                 b = b/float(ki1)

                 varname='buoy1km'
                 description='Average buoyancy below 1 km'
                 units='m/s^2'

                 call h5_write_2d_float(twodfull_id,varname,description,units,b,corexytype,0,0,.true.)

                 varname='thetae1km'
                 description='Theta-e at 1 km'
                 units='K'

                 call h5_write_2d_float(twodfull_id,varname,description,units,the,corexytype,0,0,.true.)

           endif !imoist.eq.1


      !----------------   end buoyancy calculation code stolen from parcel.F ------------------------!

      ! Temperature, in deg F:

                 do j=1,nj
                 do i=1,ni
                   ds2(i,j)=(((th0(i,j,1)+tha(i,j,1))*(pi0(i,j,1)+ppi(i,j,1)))-273.15)*(9./5.) + 32.0
                 enddo
                 enddo

                 varname='sfctempf'
                 description='Surface temperature in F'
                 units='degrees F'

                 call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

      ! u, v at sfc, 1km

                 do j=1,nj
                 do i=1,ni
                   ds2(i,j)=0.5*(ua(i,j,1)+ua(i+1,j,1))
                 enddo
                 enddo

                 varname='uinterpsfc'
                 description='surface u wind (interpolated to scalar grid)'
                 units='m/s'
                 call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

                 do j=1,nj
                 do i=1,ni
                   ds2(i,j)=0.5*(va(i,j,1)+va(i,j+1,1))
                 enddo
                 enddo

                 varname='vinterpsfc'
                 description='surface v wind (interpolated to scalar grid)'
                 units='m/s'
                 call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

                 do j=1,nj
                 do i=1,ni
                   ds2(i,j)=0.5*(ua(i,j,ki1)+ua(i+1,j,ki1))
                 enddo
                 enddo

                 varname='uinterp1km'
                 description='1km u wind (interpolated to scalar grid)'
                 units='m/s'
                 call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

                 do j=1,nj
                 do i=1,ni
                   ds2(i,j)=0.5*(va(i,j,ki1)+va(i,j+1,ki1))
                 enddo
                 enddo

                 varname='vinterp1km'
                 description='1km v wind (interpolated to scalar grid)'
                 units='m/s'
                 call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

                 do j=1,nj
                 do i=1,ni
                   ds2(i,j)=tha(i,j,1)
                 enddo
                 enddo
                 varname='thpertsfc'
                 description = 'surface perturbation potential temperature'
                 units = 'K'
                 call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

                 do j=1,nj
                 do i=1,ni
                   ds2(i,j)=wa(i,j,ki1)
                 enddo
                 enddo
                 varname='w1km'
                 description = 'w at 1 km'
                 units = 'm/s'
                 call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

                 do j=1,nj
                 do i=1,ni
                   ds2(i,j)=wa(i,j,ki2)
                 enddo
                 enddo
                 varname='w2km'
                 description = 'w at 2 km'
                 units = 'm/s'
                 call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

                 do j=1,nj
                 do i=1,ni
                   ds2(i,j)=wa(i,j,ki3)
                 enddo
                 enddo
                 varname='w3km'
                 description = 'w at 3 km'
                 units = 'm/s'
                 call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

                 do j=1,nj
                 do i=1,ni
                   ds2(i,j)=wa(i,j,ki4)
                 enddo
                 enddo
                 varname='w4km'
                 description = 'w at 4 km'
                 units = 'm/s'
                 call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

                 do j=1,nj
                 do i=1,ni
                   ds2(i,j)=wa(i,j,ki5)
                 enddo
                 enddo
                 varname='w5km'
                 description = 'w at 5 km'
                 units = 'm/s'
                 call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)
           endif !if wr2d


! Cold pool C and H (RKW)

      if(output_coldpool.eq.1) then
          call calccpch(zf,th0,qv0,dum1(ib,jb,1),dum1(ib,jb,2),tha,qa)
          do j=1,nj
          do i=1,ni
            ds2(i,j)=dum1(i,j,1)
          enddo
          enddo
          varname='cpc'
          description='cold pool intensity C'
          units='m/s'
          if(wr3d)call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)
          if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=dum1(i,j,2)
          enddo
          enddo
          varname='cph'
          description='cold pool depth h'
          units='meters'
          if(wr3d)call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)
          if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds2,corexytype,0,0,.true.)
      endif !output_coldpool.eq.1


! Surface fluxes
! I think only 3d from here on out ORF

      if(wr3d)then
      if(output_sfcflx.eq.1) then
          do j=1,nj
          do i=1,ni
            ds2(i,j)=thflux(i,j)
          enddo
          enddo
          varname='thflux'
          description='surface potential temperature flux'
          units='K m s^{-1}'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)
        
          do j=1,nj
          do i=1,ni
            ds2(i,j)=qvflux(i,j)
          enddo
          enddo

          varname='qvflux'
          description='surface water vapor flux'
          units='kg/kg m s^{-1}'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
            do i=1,ni
             ds2(i,j)=0.25*( (cdu(i,j)+cdu(i+1,j))   &
                           +(cdv(i,j)+cdv(i,j+1)) )
            enddo
          enddo

          varname='cd'
          description='cd' !ORF WTF IS THIS?
          units='nondimensional'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=ce(i,j)
          enddo
          enddo

          varname='ce'
          description='ce' !ORF WTF IS THIS?
          units='nondimensional'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=tsk(i,j)
          enddo
          enddo

          varname='tsk'
          description='soil/ocean temperature'
          units='K'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

      endif !output_sfcflx.eq.1

      if(output_sfcparams.eq.1) then

          do j=1,nj
          do i=1,ni
            ds2(i,j)=xland(i,j)
          enddo
          enddo

          varname='xland'
          description='land/water flag (1=land, 2=water)'
          units='integer flag'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=lu_index(i,j)
          enddo
          enddo

          varname='lu'
          description='land usage index'
          units='integer flag'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=mavail(i,j)
          enddo
          enddo

          varname='mavail'
          description='surface moisture availability'
          units='integer flag'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

      endif !output_sfcparams.eq.1

      if((output_sfcparams.eq.1).and.(sfcmodel.eq.2.or.oceanmodel.eq.2))then

          do j=1,nj
          do i=1,ni
            ds2(i,j)=tmn(i,j)
          enddo
          enddo
          varname='tmn'
          description='deep-layer soil temperature'
          units='K'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=hfx(i,j)
          enddo
          enddo
          varname='hfx'
          description='heat flux at surface'
          units='W/m^2'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=qfx(i,j)
          enddo
          enddo
          varname='qfx'
          description='surface moisture flux'
          units='W/m^2'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=gsw(i,j)
          enddo
          enddo
          varname='gsw'
          description='downward SW flux at surface'
          units='W/m^2'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=glw(i,j)
          enddo
          enddo
          varname='glw'
          description='downward LW flux at surface'
          units='W/m^2'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)
      endif

      if((output_sfcparams.eq.1).and.(sfcmodel.eq.2))then

          do j=1,nj
          do i=1,ni
            ds2(i,j)=tslb(i,j,1)
          enddo
          enddo
          varname='tslb1'
          description='soil temp, layer 1'
          units='K'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=tslb(i,j,2)
          enddo
          enddo
          varname='tslb2'
          description='soil temp, layer 2'
          units='K'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=tslb(i,j,3)
          enddo
          enddo
          varname='tslb3'
          description='soil temp, layer 3'
          units='K'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=tslb(i,j,4)
          enddo
          enddo
          varname='tslb4'
          description='soil temp, layer 4'
          units='K'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=tslb(i,j,5)
          enddo
          enddo
          varname='tslb5'
          description='soil temp, layer 5'
          units='K'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)
      endif

      if(output_sfcparams.eq.1.and.oceanmodel.eq.2)then

          do j=1,nj
          do i=1,ni
            ds2(i,j)=tml(i,j)
          enddo
          enddo
          varname='tml'
          description='ocean mixed layer temperature'
          units='K'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=hml(i,j)
          enddo
          enddo
          varname='hml'
          description='ocean mixed layer depth'
          units='meters'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=huml(i,j)
          enddo
          enddo
          varname='huml'
          description='ocean mixed layer u velocity'
          units='m/s'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=hvml(i,j)
          enddo
          enddo
          varname='hvml'
          description='ocean mixed layer v velocity'
          units='m/s'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)
      endif

      if(output_radten.eq.1)then

          do j=1,nj
          do i=1,ni
            ds2(i,j)=radsw(i,j)
          enddo
          enddo
          varname='radsw'
          description='solar radiation at surface'
          units='W/m^2'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=rnflx(i,j)
          enddo
          enddo
          varname='rnflx'
          description='net radiation absorbed by surface'
          units='W/m^2'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=radswnet(i,j)
          enddo
          enddo
          varname='radswnet'
          description='net solar radiation'
          units='W/m^2'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=radlwin(i,j)
          enddo
          enddo
          varname='radlwin'
          description='incoming longwave radiation'
          units='W/m^2'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)
      endif

      if(output_sfcdiags.eq.1)then

          do j=1,nj
          do i=1,ni
            ds2(i,j)=u10(i,j)
          enddo
          enddo
          varname='ul0'
          description='diagnostic 10m u wind'
          units='m/s'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=v10(i,j)
          enddo
          enddo
          varname='vl0'
          description='diagnostic 10m v wind'
          units='m/s'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=t2(i,j)
          enddo
          enddo
          varname='t2'
          description='diagnostic 2m temperature'
          units='K'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=q2(i,j)
          enddo
          enddo
          varname='q2'
          description='diagnostic 2m water vapor mixing ratio'
          units='kg/kg'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=znt(i,j)
          enddo
          enddo
          varname='znt'
          description='roughness length'
          units='m'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=ust(i,j)
          enddo
          enddo
          varname='ust'
          description='u* in similarity theory'
          units='m/s'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          if(ipbl.eq.1)then
                description='PBL height (from PBL scheme)'
          else
                description='rough estimate of PBL height'
          endif
          do j=1,nj
          do i=1,ni
            ds2(i,j)=hpbl(i,j)
          enddo
          enddo
          varname='hpbl'
          units='meters'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=zol(i,j)
          enddo
          enddo
          varname='zol'
          description='z/L (z over Monin-Obukhov length)'
          units='dimensionless'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=mol(i,j)
          enddo
          enddo
          varname='mol'
          description='T* (similarity theory)'
          units='K'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=br(i,j)
          enddo
          enddo
          varname='br'
          description='bulk Richardson number in surface layer'
          units='dimensionless'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)
      endif

      if(output_th  .eq.1)then
          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=th0(i,j,k)+tha(i,j,k)
          enddo
          enddo
          enddo
          varname='th'
          description = 'Potential temperature'
          units = 'K'
          call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)
      endif
       if(output_u    .eq.1) then
           do k=1,nk
           do j=1,nj
           do i=1,ni+1
             dumu(i,j,k)=ua(i,j,k)
           enddo
           enddo
           enddo
           varname='u'
           description = 'velocity in x direction on native Arakawa C grid'
           units = 'm/s'
! xyztype not xyzu type because we have that overlap and only need the last
! yz plane and we take care of it in h5_write_3d_float with isu
           call h5_write_3d_float(threed_group_id,varname,description,units,dumu(1:ni+1,1:nj,1:nk),corexyztype,1,0,0)
       endif

      if(output_upert.eq.1)then
          do k=1,nk
          do j=1,nj
          do i=1,ni+1
            dumu(i,j,k)=ua(i,j,k)-u0(i,j,k)
          enddo
          enddo
          enddo
          varname='upert'
          description = 'perturbation velocity in x direction on native Arakawa C grid'
          units = 'm/s'
          call h5_write_3d_float(threed_group_id,varname,description,units,dumu(1:ni+1,1:nj,1:nk),corexyztype,1,0,0)
      endif
      if(output_uinterp.eq.1)then
          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=0.5*(ua(i,j,k)+ua(i+1,j,k))
          enddo
          enddo
          enddo
          varname="uinterp"
          description = 'velocity in x-direction, interpolated to scalar points'
          units = 'm/s'
          call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)
      endif

      if(output_v    .eq.1) then
          do k=1,nk
          do j=1,nj+1
          do i=1,ni
            dumv(i,j,k)=va(i,j,k)
          enddo
          enddo
          enddo
          varname='v'
          description = 'velocity in y direction on native Arakawa C grid'
          units = 'm/s'
! xyztype not xyzv type because we have that overlap and only need the last
! xz plane and we take care of it in h5_write_3d_float with isv
          call h5_write_3d_float(threed_group_id,varname,description,units,dumv(1:ni,1:nj+1,1:nk),corexyztype,0,1,0)
      endif

      if(output_vpert.eq.1)then
          do k=1,nk
          do j=1,nj+1
          do i=1,ni
            dumv(i,j,k)=va(i,j,k)-v0(i,j,k)
          enddo
          enddo
          enddo
          varname='vpert'
          description = 'perturbation velocity in y direction on native Arakawa C grid'
          units = 'm/s'
          call h5_write_3d_float(threed_group_id,varname,description,units,dumv(1:ni,1:nj+1,1:nk),corexyztype,0,1,0)
      endif

      if(output_vinterp.eq.1)then
          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=0.5*(va(i,j,k)+va(i,j+1,k))
          enddo
          enddo
          enddo
          varname="vinterp"
          description = 'velocity in y-direction, interpolated to scalar points'
          units = 'm/s'
          call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)
      endif

      if(output_w    .eq.1) then
          do k=1,nk+1
          do j=1,nj
          do i=1,ni
            dumw(i,j,k)=wa(i,j,k)
          enddo
          enddo
          enddo
          varname='w'
          description = 'velocity in z direction on native Arakawa C grid'
          units = 'm/s'

! NOTE! we can get rid of ds and just use George's temp array and index
! it accordingly

! we do use corexyzwtype because the we don't have vertical
! decomposition, just pass a bigger array and be done with it.
! isw is used in h5_write_3d_float
          call h5_write_3d_float(threed_group_id,varname,description,units,dumw(1:ni,1:nj,1:nk+1),corexyzwtype,0,0,1)
      endif

      if(output_winterp.eq.1)then
          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=0.5*(wa(i,j,k)+wa(i,j,k+1))
          enddo
          enddo
          enddo
          varname='winterp'
          description = 'velocity in z direction, interpolated to scalar points'
          units = 'm/s'
          call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)
      endif

      if(output_thpert .eq.1) then
          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=tha(i,j,k)
          enddo
          enddo
          enddo
          varname='thpert'
          description = 'Perturbation potential temperature'
          units = 'K'
          call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)
      endif

      if(output_prs    .eq.1) then
          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=prs(i,j,k)
          enddo
          enddo
          enddo
          varname='p'
          description = 'Pressure'
          units = 'Pa'
          call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)
      endif

      if(output_prspert.eq.1)then
          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=prs(i,j,k)-p00*(pi0(i,j,k)**cpdrd)
          enddo
          enddo
          enddo
          varname="ppert"
          description = 'Perturbation pressure'
          units = 'Pa'
          call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)
      endif
      if(output_pi.eq.1)then  
          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=pi0(i,j,k)+ppi(i,j,k)
          enddo
          enddo
          enddo
          varname = "pi"
          description = 'Nondimensional pressure (Exner function)'
          units = 'dimensionless'
          call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)
      endif
      if(output_pipert .eq.1) then
          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=ppi(i,j,k)
          enddo
          enddo
          enddo
          varname='pipert'
          description = 'Nondimensional pressure (Exner function) perturbation'
          units = 'dimensionless'
          call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)
      endif

      if(output_rho    .eq.1) then
          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=rho(i,j,k)
          enddo
          enddo
          enddo
          varname = "rho"
          description = 'Density'
          units = 'kg/m^3'
          call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)
      endif
      if(output_rhopert.eq.1)then
          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=rho(i,j,k)-rho0(i,j,k)
          enddo
          enddo
          enddo
          varname="rhopert"
          description = 'Density perturbation'
          units = 'kg/m^3'
          call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)
      endif
      if(iturb.eq.1)then
            if(output_tke.eq.1) then
                do k=1,nk
                do j=1,nj
                do i=1,ni
                  ds(i,j,k)=tkea(i,j,k)
                enddo
                enddo
                enddo
                varname="tke"
                description = 'Subgrid turbulence kinetic energy'
                units = 'J/kg'
                call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)
           endif
      endif

      if(iturb.ge.1)then

          if(output_km  .eq.1) then
              do k=1,nk
              do j=1,nj
              do i=1,ni
                ds(i,j,k)=kmh(i,j,k)
              enddo
              enddo
              enddo

              varname='kmh'
              description = 'Eddy mixing coefficient for momentum in the horizontal direction'
              units = 'm^2/s'
              call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)

              do k=1,nk
              do j=1,nj
              do i=1,ni
                ds(i,j,k)=kmv(i,j,k)
              enddo
              enddo
              enddo
              varname='kmv'
              description = 'Eddy mixing coefficient for momentum in the vertical direction'
              units = 'm^2/s'
              call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)
          endif

         if(output_kh  .eq.1) then
           do k=1,nk
           do j=1,nj
           do i=1,ni
             ds(i,j,k)=khh(i,j,k)
           enddo
           enddo
           enddo
           varname='khh'
           description = 'Eddy mixing coefficient for scalars in the horizontal direction'
           units = 'm^2/s'
           call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)

           do k=1,nk
           do j=1,nj
           do i=1,ni
             ds(i,j,k)=khv(i,j,k)
           enddo
           enddo
           enddo
           varname='khv'
           description = 'Eddy mixing coefficient for scalars in the vertical direction'
           units = 'm^2/s'
           call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)
         endif
      endif !iturb .ge. 1

      if(imoist.eq.1)then
          if(output_qv .eq. 1)  then
                do k=1,nk
                do j=1,nj
                do i=1,ni
                  ds(i,j,k)=qa(i,j,k,nqv)
                enddo
                enddo
                enddo
                varname='qv'
                description = 'Water vapor mixing ratio'
                units = 'kg/kg'
                call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)
          endif
          if(output_qvpert.eq.1)then
              do k=1,nk
              do j=1,nj
              do i=1,ni
                ds(i,j,k)=qa(i,j,k,nqv)-qv0(i,j,k)
              enddo
              enddo
              enddo
              varname="qvpert"
              description = 'Perturbation water vapor mixing ratio'
              units = 'kg/kg'
              call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)
          endif

          if(output_q.eq.1)then
              do n=1,numq
                  if(n.ne.nqv)then
                      do k=1,nk
                      do j=1,nj
                      do i=1,ni
                        ds(i,j,k)=qa(i,j,k,n)
                      enddo
                      enddo
                      enddo
                      qvarname='                    '
                      write(qvarname(1:3),156) qname(n)
156                   Format(a3)
                      if(idm.eq.1.and.n.ge.nnc1.and.n.le.nnc2)then
                           description = 'Number concentration'
                           units = 'kg^{-1}'
                      else
                           description = 'Hydrometeor mixing ratio'
                           units = 'kg/kg'
                      endif
                      call h5_write_3d_float(threed_group_id,qvarname,description,units,ds,corexyztype,0,0,0)
                  endif
              enddo
          endif !output_q .eq.1
          if(output_dbz.eq.1) then
              do k=1,nk
              do j=1,nj
              do i=1,ni
                ds(i,j,k)=dbz(i,j,k)
              enddo
              enddo
              enddo
              varname="dbz"
              description = 'Reflectivity'
              units = 'dBZ'
              call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)
          endif

      endif !imoist.eq.1



      if(iptra.eq.1)then
          do n=1,npt
            do k=1,nk
            do j=1,nj
            do i=1,ni
              ds(i,j,k)=pta(i,j,k,n)
            enddo
            enddo
            enddo

            chid = 'pt      '
            write(chid(3:4),111) n
111         format(i2.2)

            varname=chid
            description='mixing ratio of passive tracer'
            units='kg/kg'
            call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)
          enddo
      endif


      if(output_pblten.eq.1)then
          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=thpten(i,j,k)
          enddo
          enddo
          enddo

          varname='thpten'
          description='pbl tendency: theta'
          units='Not sure, email leigh.orf@cmich.edu if you do know'
          call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)

          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=qvpten(i,j,k)
          enddo
          enddo
          enddo

          varname='qvpten'
          description='pbl tendency: qv'
          units='Not sure, email leigh.orf@cmich.edu if you do know'
          call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)

          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=qcpten(i,j,k)
          enddo
          enddo
          enddo

          varname='qcpten'
          description='pbl tendency: qc'
          units='Not sure, email leigh.orf@cmich.edu if you do know'
          call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)

          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=qipten(i,j,k)
          enddo
          enddo
          enddo

          varname='qipten'
          description='pbl tendency: qi'
          units='Not sure, email leigh.orf@cmich.edu if you do know'
          call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)

          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=upten(i,j,k)
          enddo
          enddo
          enddo

          varname='upten'
          description='pbl tendency: u'
          units='Not sure, email leigh.orf@cmich.edu if you do know'
          call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)

          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=vpten(i,j,k)
          enddo
          enddo
          enddo

          varname='vpten'
          description='pbl tendency: v'
          units='Not sure, email leigh.orf@cmich.edu if you do know'
          call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)

      endif !output_pblten.eq.1

      if(output_uh.eq.1) then

        ! get height AGL:
          if( terrain_flag )then
            do k=1,nk+1
            do j=1,nj
            do i=1,ni
              dum3(i,j,k) = zh(i,j,k)-zs(i,j)
              dumw(i,j,k) = zf(i,j,k)-zs(i,j)
            enddo
            enddo
            enddo
          else
            do k=1,nk+1
            do j=1,nj
            do i=1,ni
              dum3(i,j,k) = zh(i,j,k)
              dumw(i,j,k) = zf(i,j,k)
            enddo
            enddo
            enddo
          endif
          call calcuh(uf,vf,dum3,dumw,ua,va,wa,dum1(ib,jb,1),dum2)
          do j=1,nj
          do i=1,ni
            ds2(i,j)=dum1(i,j,1)
          enddo
          enddo

          varname='uh'
          description='integrated updraft helicity'
          units='m^2/s^2'
          call h5_write_2d_float(twod_group_id,varname,description,units,ds2,corexytype,0,0,.false.)

      endif!output_uh.eq.1
endif !wr3d

      if(output_vort.eq.1)then
          call calcvort(xh,xf,uf,vf,zh,mf,zf,ua,va,wa,dum1,dum2,dum3,dum4)

          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=dum1(i,j,k)
          enddo
          enddo
          enddo

          varname='xvort'
          description='horizontal vorticity (x), usually called xi'
          units='s^{-1}'
          if(wr3d)call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)
          varname='xvortsfc'
          description='surface horizontal vorticity (x), usually called xi'
          units='s^{-1}'
          if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds(1:ni,1:nj,1),corexytype,0,0,.true.)
          varname='xvort1km'
          description='1km horizontal vorticity (x), usually called xi'
          units='s^{-1}'
          if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds(1:ni,1:nj,ki1),corexytype,0,0,.true.)

          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=dum2(i,j,k)
          enddo
          enddo
          enddo

          varname='yvort'
          description='horizontal vorticity (y), usually called eta'
          units='s^{-1}'
          if(wr3d)call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)

          varname='yvortsfc'
          description='surface horizontal vorticity (y), usually called eta'
          units='s^{-1}'
          if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds(1:ni,1:nj,1),corexytype,0,0,.true.)
          varname='yvort1km'
          description='1km horizontal vorticity (y), usually called eta'
          units='s^{-1}'
          if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds(1:ni,1:nj,ki1),corexytype,0,0,.true.)

          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=dum3(i,j,k)
          enddo
          enddo
          enddo

          varname='zvort'
          description='vertical vorticity (zeta)'
          units='s^{-1}'
          if(wr3d)call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)

          varname='zvortsfc'
          description='surface vertical vorticity (zeta)'
          units='s^{-1}'
          if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds(1:ni,1:nj,1),corexytype,0,0,.true.)
          varname='zvort1km'
          description='1km vertical vorticity (zeta)'
          units='s^{-1}'
          if(wr2d)call h5_write_2d_float(twodfull_id,varname,description,units,ds(1:ni,1:nj,ki1),corexytype,0,0,.true.)

      endif !output_vort.eq.1

      if(output_dissten.eq.1) then

          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=dissten(i,j,k)
          enddo
          enddo
          enddo

          varname='dissten'
          description='dissipative heating tendency'
          units='K/s'
          if(wr3d)call h5_write_3d_float(threed_group_id,varname,description,units,ds,corexyztype,0,0,0)

       endif


! close everything
            if(wr3d) then
                  call h5gclose_f(grid_group_id,ierror)
                  call h5gclose_f(mesh_group_id,ierror)
                  call h5gclose_f(basestate_group_id,ierror)
                  call h5gclose_f(threed_basestate_group_id,ierror)
                  call h5gclose_f(twod_group_id,ierror)
                  call h5gclose_f(threed_group_id,ierror)
                  call h5fclose_f(file_id,ierror)
            endif

            if(wr2d) then
                  call h5gclose_f(twodfull_id,ierror)
                  call h5gclose_f(twodfull_mesh_id,ierror)
                  call h5gclose_f(twodfull_grid_id,ierror)
                  call h5gclose_f(twodfull_shear_id,ierror)
                  call h5gclose_f(twodfull_base_id,ierror)
                  call h5fclose_f(file_2d_id,ierror)
            endif

            call h5close_f(ierror)

            if(myid.eq.0) then
                  if (wr3d) write(outfile,*) 'Done Writing Data to File: ',trim(filename_2d)
                  if (wr2d) write(outfile,*) 'Done Writing Data to File: ',trim(filename)
                  call FFLUSH(outfile)
            endif

!#ifdef EATPOO
!
!! begin block o' crap
!!     endif
!            call MPI_BARRIER(MPI_COMM_WORLD,ierror)
!            call h5gclose_f(grid_group_id,ierror)
!            call h5gclose_f(mesh_group_id,ierror)
!            call h5gclose_f(basestate_group_id,ierror)
!            call h5gclose_f(threed_basestate_group_id,ierror)
!            call h5gclose_f(twod_group_id,ierror)
!            call h5gclose_f(threed_group_id,ierror)
!            call h5fclose_f(file_id,ierror)
!            call h5close_f(ierror)
!            if (iodebug) print *, 'myid = ', myid, ' Made it past 1d writes and group closings'
!
!#endif
!EATPOO

      return
      end

!--------------------------------------------------------------------------------------------MARK

      subroutine h5_write_2d_float(file_id,varname,description,units,core2d,mcmtype,isu,isv,isslice)
      use HDF5
      use orfiomod
      implicit none
      include 'input.incl'
#ifdef MPI
      include 'mpif.h'
#endif

      character(LEN=20) :: varname
      character*20  units
      character*120  description
      integer(HSIZE_T), DIMENSION(2) :: dims,count
      integer(HSSIZE_T), DIMENSION(2) :: offset
      integer(HSIZE_T), DIMENSION(1) :: strdims
      integer :: arank = 1
      integer :: rank = 2
      integer(HSIZE_T), DIMENSION(1) :: adims = (/1/)
      integer(SIZE_T) :: stringlen
      integer(HID_T) :: file_id,dset_id,filespace_id,memspace_id,chunk_id,datatype_id,desctype_id,unitstype_id,desc_id,units_id,aspace_id,plist_id
      integer :: ierror,isu,isv,isw,mcmtype
      integer :: i,j,iix,iiy,ixcore,iycore,ioffset,info
      real, dimension(ni+isu,nj+isv) :: core2d
      logical :: isslice

      if (iodebug2) then
         call MPI_BARRIER(MPI_COMM_WORLD,ierror)
         print *,myid,' Writing 2D float: ',trim(varname)
         call FFLUSH(6)
         call MPI_BARRIER(MPI_COMM_WORLD,ierror)
      endif

            strdims(1)=1

            call h5screate_simple_f(arank,adims,aspace_id,ierror)
            call h5tcopy_f(H5T_NATIVE_CHARACTER,desctype_id,ierror)
            call h5tcopy_f(H5T_NATIVE_CHARACTER,unitstype_id,ierror)
            stringlen = len_trim(description)
            call h5tset_size_f(desctype_id,stringlen,ierror)
            stringlen = len_trim(units)
            call h5tset_size_f(unitstype_id,stringlen,ierror)

! ORF for now isslice only works when using pHDF5

      !handle filespace first
            if (.not. isslice) then
                  dims(1)  = ni*corex
                  dims(2)  = nj*corey
            else
                  dims(1) = nx
                  dims(2) = ny
            endif

            CALL h5screate_simple_f(rank, dims, filespace_id, ierror)
            call h5dcreate_f(file_id,trim(varname),H5T_NATIVE_REAL,filespace_id,dset_id,ierror)!removed chunk_id
            call h5sclose_f(filespace_id,ierror)

      !now memoryspace
            count(1) = ni
            count(2) = nj

            if (.not. isslice) then
                  offset(1) = mygroupi * ni
                  offset(2) = mygroupj * nj
            else
                  offset(1) = (myi-1) * ni
                  offset(2) = (myj-1) * nj
            endif

            call h5screate_simple_f(rank,count,memspace_id,ierror)
            CALL h5dget_space_f(dset_id, filespace_id, ierror)
            CALL h5sselect_hyperslab_f (filespace_id, H5S_SELECT_SET_F, offset, count, ierror)
            CALL h5pcreate_f(H5P_DATASET_XFER_F, plist_id, ierror)
            CALL h5pset_dxpl_mpio_f(plist_id, MPIO, ierror)
            CALL h5dwrite_f(dset_id, H5T_NATIVE_REAL, core2d, dims, ierror, &
                       file_space_id = filespace_id, mem_space_id = memspace_id, xfer_prp = plist_id)
!           CALL h5dwrite_f(dset_id, H5T_NATIVE_INTEGER, core2d, dims, ierror, &
!                      file_space_id = filespace_id, mem_space_id = memspace_id)
            call h5acreate_f(dset_id,"description",desctype_id,aspace_id,desc_id,ierror,H5P_DEFAULT_F,H5P_DEFAULT_F)
            call h5awrite_f(desc_id,desctype_id,description,strdims,ierror)
            call h5acreate_f(dset_id,"units",unitstype_id,aspace_id,units_id,ierror,H5P_DEFAULT_F,H5P_DEFAULT_F)
            call h5awrite_f(units_id,unitstype_id,units,strdims,ierror)

            call H5aclose_f (desc_id,ierror)
            call H5aclose_f (units_id,ierror)
            call H5sclose_f (aspace_id,ierror)
            call H5tclose_f (desctype_id,ierror)
            call H5tclose_f (unitstype_id,ierror)
            call h5dclose_f (dset_id,ierror)
            call h5pclose_f (plist_id,ierror)
            call h5sclose_f (memspace_id,ierror)

      return
      end

!--------------------------------------------------------------------------------------------MARK

      subroutine h5_write_3d_float(file_id,varname,description,units,core3d,mcmtype,isu,isv,isw)
      use HDF5
      use orfiomod
      implicit none
      include 'input.incl'
#ifdef MPI
      include 'mpif.h'
#endif

      character(LEN=20) :: varname
      character*20  units
      character*120  description
      integer(HSIZE_T), DIMENSION(3) :: dims,chunkdims,count,stride,block
      integer(HSSIZE_T), DIMENSION(3) :: offset
      integer(HSIZE_T), DIMENSION(1) :: strdims
      integer :: arank = 1
      integer(HSIZE_T), DIMENSION(1) :: adims = (/1/)
      integer(SIZE_T) :: stringlen
      integer(HID_T) :: file_id,dset_id,dspace_id,chunk_id,datatype_id,desctype_id,unitstype_id,desc_id,units_id,aspace_id
      integer(HID_T) :: localmax_id,localmin_id,globalmax_id,globalmin_id,maxmintype_id
      integer(HID_T) :: memspace_id,filespace_id,plist_id
      integer ::ierror,isu,isv,isw,mcmtype
      integer :: rank
      integer :: i,j,k,iix,iiy,ixcore,iycore,ioffset
      real, dimension(ni+isu,nj+isv,nk+isw) :: core3d
      real, dimension(ionumi,nk) :: lastxz
      real, dimension(ionumj,nk) :: lastyz
      real, dimension(ionumi*nk) :: vlast_source
      real, dimension(ionumj*nk) :: ulast_source
      real localmax,localmin,globalmax,globalmin
      integer source, dest, tag
      integer, dimension(MPI_STATUS_SIZE) :: mpi_status

! ORF isu, isv, isw are treated as booleans and as an integer to add that last
! point. Isrestart is more appropraitely a boolean only since it's only used for
! making decisions

      rank = 3

      if (iodebug2) call MPI_BARRIER(MPI_COMM_WORLD,ierror)
      if (iodebug2) print *,myid,' Writing 3D float ',trim(varname)
      if (iodebug2) call FFLUSH(6)
      if (iodebug2) call MPI_BARRIER(MPI_COMM_WORLD,ierror)


      dims(1)=ionumi 
      dims(2)=ionumj
      dims(3)=nk+isw
      
      chunkdims(1)=ni
      chunkdims(2)=nj
      chunkdims(3)=nk+isw

! Count is blocks, which are chunks
! Making chunkdims(3) nk because I think it needs to always divide evenly -
! in other words you need your block to line up right

      count(1)=1
      count(2)=1
      count(3)=1

      offset(1) = mygroupi * chunkdims(1)
      offset(2) = mygroupj * chunkdims(2)
      offset(3) = 0

      stride(1) = 1
      stride(2) = 1
      stride(3) = 1

      block(1) = chunkdims(1)
      block(2) = chunkdims(2)
      block(3) = chunkdims(3)

      strdims(1)=1

! ORF 2/21/11
! This is commented out because we are wanting to call this routine for
! only a subset of the full number of ranks in order to reduce hitting
! the filesystem too hard with tens of thousands of cores writing - and hence we
! can't do collective operations with MPI_COMM_WORLD

!     call maxmin_orf(ni,nj,nk,core3d(1:ni,1:nj,1:nk),globalmax,globalmin,MPI_COMM_WORLD)
!     call MPI_BCAST(globalmax,1,MPI_REAL,0,MPI_COMM_WORLD,ierror)
!     call MPI_BCAST(globalmin,1,MPI_REAL,0,MPI_COMM_WORLD,ierror)
!     call MPI_BARRIER(MPI_COMM_WORLD,ierror)
!     if(myid.eq.0)print*, varname," global max/min: ",fmax,fmin
!     call FFLUSH(6)

! ORF 2/21/11
! This however is OK becuase we are only getting the max/min for the
! file (communicator:mysubgroupcomm) in question

      call maxmin_orf(ni,nj,nk,core3d(1:ni,1:nj,1:nk),localmax,localmin,mysubgroupcomm)

!     call MPI_BARRIER(MPI_COMM_WORLD,ierror)
!     if(mysubgrouprank.eq.0)print*, varname, " local max/min: ",fmax,fmin
!     call FFLUSH(6)

      call h5screate_simple_f(arank,adims,aspace_id,ierror)
      call h5tcopy_f(H5T_NATIVE_CHARACTER,desctype_id,ierror)
      call h5tcopy_f(H5T_NATIVE_CHARACTER,unitstype_id,ierror)
      call h5tcopy_f(H5T_NATIVE_REAL,maxmintype_id,ierror)
      stringlen = len_trim(description)
      call h5tset_size_f(desctype_id,stringlen,ierror)
      stringlen = len_trim(units)
      call h5tset_size_f(unitstype_id,stringlen,ierror)
!Create filespace first, attach attributes
!forget about U and V crap for now
!The only consequence of this is the very last (East/North) point is lost for (U/V)
      call h5screate_simple_f(rank,dims,filespace_id,ierror)
      call h5screate_simple_f(rank,chunkdims,memspace_id,ierror)
      call h5pcreate_f(H5P_DATASET_CREATE_F,chunk_id,ierror)
      call h5pset_chunk_f(chunk_id,rank,chunkdims,ierror)
      call h5dcreate_f(file_id,trim(varname),H5T_NATIVE_REAL,filespace_id,dset_id,ierror,chunk_id)
      call h5sclose_f(filespace_id,ierror)
!     print *, 'myid = ',myid,' Working on ',trim(varname)
! description and units attributes
      call h5acreate_f(dset_id,"description",desctype_id,aspace_id,desc_id,ierror,H5P_DEFAULT_F,H5P_DEFAULT_F)
      call h5awrite_f(desc_id,desctype_id,description,strdims,ierror)
      call h5acreate_f(dset_id,"units",unitstype_id,aspace_id,units_id,ierror,H5P_DEFAULT_F,H5P_DEFAULT_F)
      call h5awrite_f(units_id,unitstype_id,units,strdims,ierror)
! max/min attributes
      call h5acreate_f(dset_id,"localmax",maxmintype_id,aspace_id,localmax_id,ierror,H5P_DEFAULT_F,H5P_DEFAULT_F)
      call h5awrite_f(localmax_id,maxmintype_id,localmax,strdims,ierror)
      call h5acreate_f(dset_id,"localmin",maxmintype_id,aspace_id,localmin_id,ierror,H5P_DEFAULT_F,H5P_DEFAULT_F)
      call h5awrite_f(localmin_id,maxmintype_id,localmin,strdims,ierror)
!     call h5acreate_f(dset_id,"globalmax",maxmintype_id,aspace_id,globalmax_id,ierror,H5P_DEFAULT_F,H5P_DEFAULT_F)
!     call h5awrite_f(globalmax_id,maxmintype_id,globalmax,strdims,ierror)
!     call h5acreate_f(dset_id,"globalmin",maxmintype_id,aspace_id,globalmin_id,ierror,H5P_DEFAULT_F,H5P_DEFAULT_F)
!     call h5awrite_f(globalmin_id,maxmintype_id,globalmin,strdims,ierror)
! Write 3D data
      call h5dget_space_f(dset_id, filespace_id, ierror)
      call h5sselect_hyperslab_f (filespace_id, H5S_SELECT_SET_F, offset, count, ierror,stride,block)
      call h5pcreate_f(H5P_DATASET_XFER_F, plist_id, ierror)
      call h5pset_dxpl_mpio_f(plist_id, MPIO, ierror)
      call h5dwrite_f(dset_id, H5T_NATIVE_REAL, core3d(1:ni,1:nj,1:nk+isw), dims, ierror, &
                 file_space_id = filespace_id, mem_space_id = memspace_id, xfer_prp = plist_id)
! Close everything

      call h5aclose_f (desc_id,ierror)
      call h5aclose_f (units_id,ierror)
      call h5sclose_f (aspace_id,ierror)
      call h5tclose_f (desctype_id,ierror)
      call h5tclose_f (unitstype_id,ierror)

      call h5tclose_f (maxmintype_id,ierror)
      call h5aclose_f (localmax_id,ierror)
      call h5aclose_f (localmin_id,ierror)
!     call h5aclose_f (globalmax_id,ierror)
!     call h5aclose_f (globalmin_id,ierror)

      call h5dclose_f (dset_id,ierror)
      call h5sclose_f (memspace_id,ierror)
      call h5sclose_f (filespace_id,ierror)
      call h5pclose_f (chunk_id,ierror)
      call h5pclose_f (plist_id,ierror)


!-------------------------------Write last U, V plane as separate dataset---------------------------------------------
! In order to keep things nice, just write the last yz (for u) or xz (for v)
! plane as a separate (2d) dataset.

!ORF  2/22/11
! Don't bother with this for restart files, just write overlapping data, we will
! never visualize restart files anyway

      if (isu .eq. 1 ) then
            rank=2
            varname=trim(varname)//"_yzlast"
            description='Easternmost yz plane of U to be tacked onto U array'
            units='m/s'
            call h5screate_simple_f(arank,adims,aspace_id,ierror)
            call h5tcopy_f(H5T_NATIVE_CHARACTER,desctype_id,ierror)
            call h5tcopy_f(H5T_NATIVE_CHARACTER,unitstype_id,ierror)
            stringlen = len_trim(description)
            call h5tset_size_f(desctype_id,stringlen,ierror)
            stringlen = len_trim(units)
            call h5tset_size_f(unitstype_id,stringlen,ierror)

      !handle filespace first
            dims(1)  = nj*corey
            dims(2)  = nk

            CALL h5screate_simple_f(rank, dims, filespace_id, ierror)
            call h5dcreate_f(file_id,trim(varname),H5T_NATIVE_REAL,filespace_id,dset_id,ierror)!removed chunk_id
            call h5sclose_f(filespace_id,ierror)

      !now memoryspace
            count(1) = nj
            count(2) = nk

            offset(1) = mygroupj * nj
            offset(2) = 0

            call h5screate_simple_f(rank,count,memspace_id,ierror)
            CALL h5dget_space_f(dset_id, filespace_id, ierror)
!           if (myrygrouprank .ne. MPI_UNDEFINED) then
            if (myi .eq. nodex) then
                  CALL h5sselect_hyperslab_f (filespace_id, H5S_SELECT_SET_F, offset, count, ierror)
            else
                  call h5sselect_none_f(filespace_id,ierror)
                  call h5sselect_none_f(memspace_id,ierror)
            endif

            CALL h5pcreate_f(H5P_DATASET_XFER_F, plist_id, ierror)
            CALL h5pset_dxpl_mpio_f(plist_id, MPIO, ierror)
            CALL h5dwrite_f(dset_id, H5T_NATIVE_REAL, core3d(ni+1,:,:), dims, ierror, &
                       file_space_id = filespace_id, mem_space_id = memspace_id, xfer_prp = plist_id)
            call h5acreate_f(dset_id,"description",desctype_id,aspace_id,desc_id,ierror,H5P_DEFAULT_F,H5P_DEFAULT_F)
            call h5awrite_f(desc_id,desctype_id,description,strdims,ierror)
            call h5acreate_f(dset_id,"units",unitstype_id,aspace_id,units_id,ierror,H5P_DEFAULT_F,H5P_DEFAULT_F)
            call h5awrite_f(units_id,unitstype_id,units,strdims,ierror)
      endif

      if (isv .eq. 1 ) then
            rank=2
            varname=trim(varname)//"_xzlast"
            description='Northernmost xz plane of V to be tacked onto V array'
            units='m/s'
            call h5screate_simple_f(arank,adims,aspace_id,ierror)
            call h5tcopy_f(H5T_NATIVE_CHARACTER,desctype_id,ierror)
            call h5tcopy_f(H5T_NATIVE_CHARACTER,unitstype_id,ierror)
            stringlen = len_trim(description)
            call h5tset_size_f(desctype_id,stringlen,ierror)
            stringlen = len_trim(units)
            call h5tset_size_f(unitstype_id,stringlen,ierror)

      !handle filespace first
            dims(1)  = ni*corex
            dims(2)  = nk

            CALL h5screate_simple_f(rank, dims, filespace_id, ierror)
            call h5dcreate_f(file_id,trim(varname),H5T_NATIVE_REAL,filespace_id,dset_id,ierror)
            call h5sclose_f(filespace_id,ierror)

      !now memoryspace
            count(1) = ni
            count(2) = nk

            offset(1) = mygroupi * ni
            offset(2) = 0

            call h5screate_simple_f(rank,count,memspace_id,ierror)
            CALL h5dget_space_f(dset_id, filespace_id, ierror)
!           if (mytxgrouprank .ne. MPI_UNDEFINED) then
            if (myj .eq. nodey) then
                  CALL h5sselect_hyperslab_f (filespace_id, H5S_SELECT_SET_F, offset, count, ierror)
            else
                  call h5sselect_none_f(filespace_id,ierror)
                  call h5sselect_none_f(memspace_id,ierror)
            endif

            CALL h5pcreate_f(H5P_DATASET_XFER_F, plist_id, ierror)
            CALL h5pset_dxpl_mpio_f(plist_id, MPIO, ierror)
            CALL h5dwrite_f(dset_id, H5T_NATIVE_REAL, core3d(:,nj+1,:), dims, ierror, &
                       file_space_id = filespace_id, mem_space_id = memspace_id, xfer_prp = plist_id)
            call h5acreate_f(dset_id,"description",desctype_id,aspace_id,desc_id,ierror,H5P_DEFAULT_F,H5P_DEFAULT_F)
            call h5awrite_f(desc_id,desctype_id,description,strdims,ierror)
            call h5acreate_f(dset_id,"units",unitstype_id,aspace_id,units_id,ierror,H5P_DEFAULT_F,H5P_DEFAULT_F)
            call h5awrite_f(units_id,unitstype_id,units,strdims,ierror)
      endif

      if ( isu.eq.1.or.isv.eq.1) then
            call H5aclose_f (desc_id,ierror)
            call H5aclose_f (units_id,ierror)
            call H5sclose_f (aspace_id,ierror)
            call H5tclose_f (desctype_id,ierror)
            call H5tclose_f (unitstype_id,ierror)
            call h5dclose_f (dset_id,ierror)
            call h5sclose_f (memspace_id,ierror)
            call h5sclose_f (filespace_id,ierror)
            call H5pclose_f (plist_id,ierror)
      endif

!-------------------------------END Write last U, V plane as separate !dataset---------------------------------------------

#ifdef ORFS_HEAD_ABOUT_TO_EXPLODE
      do i=1,toomanybeers
            call Orf_Drink_Beer(i)
      enddo
#endif
      return
      end
!--------------------------------------------------------------------------------------------MARK
      subroutine h5_write_float (file_id,varname,description,units,value,isslice)
      use HDF5
      use orfiomod
      implicit none
      include 'input.incl'
#ifdef MPI
      include 'mpif.h'
#endif
      character(LEN=20) :: varname
      character*120  description
      character*20  units

      integer(HSIZE_T), DIMENSION(1) :: strdims
      integer :: arank = 1
      integer(HSIZE_T), DIMENSION(1) :: adims = (/1/)
      integer(SIZE_T) :: stringlen

      real value
      integer rank,localiorank,ierror
      integer(HSIZE_T), DIMENSION(1) :: dims,count
      integer(HSSIZE_T), DIMENSION(1) :: offset
      integer(HID_T) :: file_id,dset_id,filespace_id,memspace_id,aspace_id,desc_id,desctype_id,unitstype_id,units_id
      integer(HID_T) :: plist_id
      logical :: isslice

      if (iodebug2) call MPI_BARRIER(MPI_COMM_WORLD,ierror)
      if (iodebug2) print *,myid,' Writing float: ',trim(varname)
      if (iodebug2) call FFLUSH(6)
      if (iodebug2) call MPI_BARRIER(MPI_COMM_WORLD,ierror)

      call h5screate_simple_f(arank,adims,aspace_id,ierror)
      call h5tcopy_f(H5T_NATIVE_CHARACTER,desctype_id,ierror)
      call h5tcopy_f(H5T_NATIVE_CHARACTER,unitstype_id,ierror)
      stringlen = len_trim(description)
      call h5tset_size_f(desctype_id,stringlen,ierror)
      stringlen = len_trim(units)
      call h5tset_size_f(unitstype_id,stringlen,ierror)

      ! filespace first
      rank=1
      strdims(1)=1
      dims(1)=1
      count(1)=1
      offset(1) = 0

      call h5screate_simple_f(rank,dims,filespace_id,ierror)
      call h5dcreate_f(file_id,trim(varname),H5T_NATIVE_REAL,filespace_id,dset_id,ierror)
      call h5sclose_f(filespace_id,ierror)

!           print *, 'Writing ',trim(varname)

      !memoryspace:
      call h5screate_simple_f(rank,count,memspace_id,ierror)
      call h5dget_space_f(dset_id,filespace_id,ierror)

      if(isslice) then
            localiorank = myid
      else
            localiorank = iorank
      endif

      if (localiorank.eq.0) then
            call h5sselect_hyperslab_f(filespace_id,H5S_SELECT_SET_F,offset,count,ierror)
      else
            call h5sselect_none_f(filespace_id,ierror)
            call h5sselect_none_f(memspace_id,ierror)
      endif
      CALL h5pcreate_f(H5P_DATASET_XFER_F, plist_id, ierror)
      CALL h5pset_dxpl_mpio_f(plist_id, MPIO, ierror)
      CALL h5dwrite_f(dset_id, H5T_NATIVE_REAL, value, dims, ierror, &
                 file_space_id = filespace_id, mem_space_id = memspace_id, xfer_prp = plist_id)
      call h5acreate_f(dset_id,"description",desctype_id,aspace_id,desc_id,ierror,H5P_DEFAULT_F,H5P_DEFAULT_F)
      call h5awrite_f(desc_id,desctype_id,trim(description),strdims,ierror)
      call h5acreate_f(dset_id,"units",unitstype_id,aspace_id,units_id,ierror,H5P_DEFAULT_F,H5P_DEFAULT_F)
      call h5awrite_f(units_id,unitstype_id,trim(units),strdims,ierror)


      call H5aclose_f (desc_id,ierror)
      call H5aclose_f (units_id,ierror)
      call H5sclose_f (aspace_id,ierror)
      call H5tclose_f (desctype_id,ierror)
      call H5tclose_f (unitstype_id,ierror)
      call h5dclose_f (dset_id,ierror)
      call h5sclose_f (filespace_id,ierror)
      call h5sclose_f (memspace_id,ierror)
      return
      end
!--------------------------------------------------------------------------------------------MARK
      subroutine h5_write_int (file_id,varname,description,units,value,isslice)
      use HDF5
      use orfiomod
      implicit none
      include 'input.incl'
#ifdef MPI
      include 'mpif.h'
#endif
      character(LEN=20) :: varname
      character*120  description
      character*20  units

      integer(HSIZE_T), DIMENSION(1) :: strdims
      integer :: arank = 1
      integer(HSIZE_T), DIMENSION(1) :: adims = (/1/)
      integer(SIZE_T) :: stringlen

      integer value
      integer rank,ierror,localiorank
      integer(HSIZE_T), DIMENSION(1) :: dims,count
      integer(HSSIZE_T), DIMENSION(1) :: offset
      integer(HID_T) :: file_id,dset_id,filespace_id,memspace_id,aspace_id,desc_id,desctype_id,unitstype_id,units_id
      integer(HID_T) :: plist_id
      logical :: isslice

      if (iodebug2) call MPI_BARRIER(MPI_COMM_WORLD,ierror)
      if (iodebug2) print *,myid,' Writing int: ',trim(varname)
      if (iodebug2) call FFLUSH(6)
      if (iodebug2) call MPI_BARRIER(MPI_COMM_WORLD,ierror)

      call h5screate_simple_f(arank,adims,aspace_id,ierror)
      call h5tcopy_f(H5T_NATIVE_CHARACTER,desctype_id,ierror)
      call h5tcopy_f(H5T_NATIVE_CHARACTER,unitstype_id,ierror)
      stringlen = len_trim(description)
      call h5tset_size_f(desctype_id,stringlen,ierror)
      stringlen = len_trim(units)
      call h5tset_size_f(unitstype_id,stringlen,ierror)

      ! filespace first
      rank=1
      strdims(1)=1
      dims(1)=1
      count(1)=1
      offset(1) = 0

      call h5screate_simple_f(rank,dims,filespace_id,ierror)
      call h5dcreate_f(file_id,trim(varname),H5T_NATIVE_INTEGER,filespace_id,dset_id,ierror)
      call h5sclose_f(filespace_id,ierror)

      !memoryspace:
      call h5screate_simple_f(rank,count,memspace_id,ierror)
      call h5dget_space_f(dset_id,filespace_id,ierror)
      if(isslice) then
            localiorank = myid
      else
            localiorank = iorank
      endif
      if (localiorank.eq.0) then
            call h5sselect_hyperslab_f(filespace_id,H5S_SELECT_SET_F,offset,count,ierror)
      else
            call h5sselect_none_f(filespace_id,ierror)
            call h5sselect_none_f(memspace_id,ierror)
      endif
      CALL h5pcreate_f(H5P_DATASET_XFER_F, plist_id, ierror)
      CALL h5pset_dxpl_mpio_f(plist_id, MPIO, ierror)
      CALL h5dwrite_f(dset_id, H5T_NATIVE_INTEGER, value, dims, ierror, &
                 file_space_id = filespace_id, mem_space_id = memspace_id, xfer_prp = plist_id)
      call h5acreate_f(dset_id,"description",desctype_id,aspace_id,desc_id,ierror,H5P_DEFAULT_F,H5P_DEFAULT_F)
      call h5awrite_f(desc_id,desctype_id,trim(description),strdims,ierror)
      call h5acreate_f(dset_id,"units",unitstype_id,aspace_id,units_id,ierror,H5P_DEFAULT_F,H5P_DEFAULT_F)
      call h5awrite_f(units_id,unitstype_id,trim(units),strdims,ierror)


      call H5aclose_f (desc_id,ierror)
      call H5aclose_f (units_id,ierror)
      call H5sclose_f (aspace_id,ierror)
      call H5tclose_f (desctype_id,ierror)
      call H5tclose_f (unitstype_id,ierror)
      call h5dclose_f (dset_id,ierror)
      call h5sclose_f (filespace_id,ierror)
      call h5sclose_f (memspace_id,ierror)
      return
      end

!--------------------------------------------------------------------------------------------MARK
! This routine is only for writing to the 2D file the FULL xf, yf arrays, each of which has
! that pesky extra point to deal with. 

! Note that we currently do NOT write the last xf,yf values in the 3D cm1hdf5 files

      subroutine h5_write_1d_float_uv (file_id,varname,description,units,local1darray,nlocal,dims,count,offset,onedcomm,onedrank,sourcerank)
      use HDF5
      use orfiomod
      implicit none
      include 'input.incl'
#ifdef MPI
      include 'mpif.h'
#endif
      character(LEN=20) :: varname
      character*120  description
      character*20  units

      integer(HSIZE_T), DIMENSION(1) :: strdims
      integer :: arank = 1
      integer(HSIZE_T), DIMENSION(1) :: adims = (/1/)
      integer(SIZE_T) :: stringlen

      integer :: nlocal
      real, dimension(nlocal) ::  local1darray
      real, dimension(:), allocatable :: vel
      real :: lastval
      integer rank,localrank,ierror,nglobal,tag,sourcerank,destrank,onedcomm,onedrank
      integer(HSIZE_T), DIMENSION(1) :: dims,count
      integer(HSSIZE_T), DIMENSION(1) :: offset
      integer(HID_T) :: file_id,dset_id,filespace_id,memspace_id,aspace_id,desc_id,desctype_id,unitstype_id,units_id
      integer(HID_T) :: plist_id
      integer status(MPI_STATUS_SIZE)

      if (iodebug2) call MPI_BARRIER(MPI_COMM_WORLD,ierror)
      if (iodebug2) print *,myid,' Writing 1d float: ',trim(varname)
      if (iodebug2) call FFLUSH(6)
      if (iodebug2) call MPI_BARRIER(MPI_COMM_WORLD,ierror)

      call h5screate_simple_f(arank,adims,aspace_id,ierror)
      call h5tcopy_f(H5T_NATIVE_CHARACTER,desctype_id,ierror)
      call h5tcopy_f(H5T_NATIVE_CHARACTER,unitstype_id,ierror)
      stringlen = len_trim(description)
      call h5tset_size_f(desctype_id,stringlen,ierror)
      stringlen = len_trim(units)
      call h5tset_size_f(unitstype_id,stringlen,ierror)

      ! filespace first
      rank=1
      strdims(1)=1
      nglobal=dims(1)
      allocate ( vel(nglobal))

      call h5screate_simple_f(rank,dims,filespace_id,ierror)
      call h5dcreate_f(file_id,trim(varname),H5T_NATIVE_REAL,filespace_id,dset_id,ierror)
      call h5sclose_f(filespace_id,ierror)

! Note that rank 0 (root) is a member of onedcomm which is either the southernmost or
! westernmost row/column

      if (onedrank .ne. MPI_UNDEFINED) call MPI_GATHER(local1darray,nlocal,MPI_REAL,vel,nlocal,MPI_REAL,0,onedcomm,ierror)

! sourcerank is either northwest for y or southeast for x (passed to routine)
! Dest rank is root

      destrank=0
      tag=1
      if (myid.eq.destrank) then
            call MPI_RECV(lastval,1,MPI_REAL,sourcerank,tag,MPI_COMM_WORLD,status,ierror)
      else if (myid.eq.sourcerank) then
            call MPI_SEND(local1darray(nlocal+1),1,MPI_REAL,destrank,tag,MPI_COMM_WORLD,ierror)
      endif

!FINALLY TACK THAT FUCKER ON

      if (myid.eq.0) vel(nglobal) = lastval

      !memoryspace:
      call h5screate_simple_f(rank,dims,memspace_id,ierror)
      call h5dget_space_f(dset_id,filespace_id,ierror)

      if (myid .eq. 0) then
            call h5sselect_hyperslab_f(filespace_id,H5S_SELECT_SET_F,offset,count,ierror)
      else
            call h5sselect_none_f(filespace_id,ierror)
            call h5sselect_none_f(memspace_id,ierror)
      endif
      CALL h5pcreate_f(H5P_DATASET_XFER_F, plist_id, ierror)
      CALL h5pset_dxpl_mpio_f(plist_id, MPIO, ierror)
      CALL h5dwrite_f(dset_id, H5T_NATIVE_REAL, vel, dims, ierror, &
                 file_space_id = filespace_id, mem_space_id = memspace_id, xfer_prp = plist_id)
      call h5acreate_f(dset_id,"description",desctype_id,aspace_id,desc_id,ierror,H5P_DEFAULT_F,H5P_DEFAULT_F)
      call h5awrite_f(desc_id,desctype_id,trim(description),strdims,ierror)
      call h5acreate_f(dset_id,"units",unitstype_id,aspace_id,units_id,ierror,H5P_DEFAULT_F,H5P_DEFAULT_F)
      call h5awrite_f(units_id,unitstype_id,trim(units),strdims,ierror)

      call H5aclose_f (desc_id,ierror)
      call H5aclose_f (units_id,ierror)
      call H5sclose_f (aspace_id,ierror)
      call H5tclose_f (desctype_id,ierror)
      call H5tclose_f (unitstype_id,ierror)
      call h5dclose_f (dset_id,ierror)
      call h5sclose_f (filespace_id,ierror)
      call h5sclose_f (memspace_id,ierror)
      return
      end

!--------------------------------------------------------------------------------------------MARK

      subroutine h5_write_1d_float (file_id,varname,description,units,local1darray,nlocal,dims,count,offset,localrank)
      use HDF5
      use orfiomod
      implicit none
      include 'input.incl'
#ifdef MPI
      include 'mpif.h'
#endif
      character(LEN=20) :: varname
      character*120  description
      character*20  units

      integer(HSIZE_T), DIMENSION(1) :: strdims
      integer :: arank = 1
      integer(HSIZE_T), DIMENSION(1) :: adims = (/1/)
      integer(SIZE_T) :: stringlen

      integer :: nlocal
      real, dimension(nlocal) ::  local1darray

      integer rank,localrank,ierror
      integer(HSIZE_T), DIMENSION(1) :: dims,count
      integer(HSSIZE_T), DIMENSION(1) :: offset
      integer(HID_T) :: file_id,dset_id,filespace_id,memspace_id,aspace_id,desc_id,desctype_id,unitstype_id,units_id
      integer(HID_T) :: plist_id

      if (iodebug2) call MPI_BARRIER(MPI_COMM_WORLD,ierror)
      if (iodebug2) print *,myid,' Writing 1d float: ',trim(varname)
      if (iodebug2) call FFLUSH(6)
      if (iodebug2) call MPI_BARRIER(MPI_COMM_WORLD,ierror)

      call h5screate_simple_f(arank,adims,aspace_id,ierror)
      call h5tcopy_f(H5T_NATIVE_CHARACTER,desctype_id,ierror)
      call h5tcopy_f(H5T_NATIVE_CHARACTER,unitstype_id,ierror)
      stringlen = len_trim(description)
      call h5tset_size_f(desctype_id,stringlen,ierror)
      stringlen = len_trim(units)
      call h5tset_size_f(unitstype_id,stringlen,ierror)

      ! filespace first
      rank=1
      strdims(1)=1

      call h5screate_simple_f(rank,dims,filespace_id,ierror)
      call h5dcreate_f(file_id,trim(varname),H5T_NATIVE_REAL,filespace_id,dset_id,ierror)
      call h5sclose_f(filespace_id,ierror)

      !memoryspace:
      call h5screate_simple_f(rank,count,memspace_id,ierror)
      call h5dget_space_f(dset_id,filespace_id,ierror)

! Everything hinges upon whether localrank, which is passed to
! this subroutine, is defined for a the current MPI rank (all
! ranks are calling this routine). I like setting up MPI groups
! and then essentially checking if we are in the group when making
! decisions about writing things - a leftover from when I was doing
! 1D MPI_Gathers, which udoubtedly pHDF5 is doing under the hood when
! set up for collective communication. So long as localrank is for a
! given MPI rank, the array passed to this routine, and its length, are
! written by the current MPI rank; if not, it's masked out as undefined.
! What is also crucial to making this work is the file_id that is
! passed, i.e., if it's the 2D slice file or the 3D MCM file (they are
! defined and initialized at the betinning of this file). This approch
! really cleans up a lot of crap that I had written here and makes this
! routine flexible (don't have to create separate 1d write routines),
! but it does require that you supply dims, count, offset from outside
! when calling this routine, which I have typically not required.
! Otherwise you have to pass booleans and make decisions in here and it
! gets messy. See param.F for all the different groups, ranks, etc. that
! are set up (you only need to do this once).
!
! Somewhat ironically, 2d and 3d writes are easier because they always
! require contribution from all ranks. In that case, I require the
! isslice boolean and can make decisions based upon that. Maybe I should
! change those to match this routine's API, I dunno.

      if (localrank .ne. MPI_UNDEFINED) then
            call h5sselect_hyperslab_f(filespace_id,H5S_SELECT_SET_F,offset,count,ierror)
      else
            call h5sselect_none_f(filespace_id,ierror)
            call h5sselect_none_f(memspace_id,ierror)
      endif
      CALL h5pcreate_f(H5P_DATASET_XFER_F, plist_id, ierror)
      CALL h5pset_dxpl_mpio_f(plist_id, MPIO, ierror)
      CALL h5dwrite_f(dset_id, H5T_NATIVE_REAL, local1darray, dims, ierror, &
                 file_space_id = filespace_id, mem_space_id = memspace_id, xfer_prp = plist_id)
      call h5acreate_f(dset_id,"description",desctype_id,aspace_id,desc_id,ierror,H5P_DEFAULT_F,H5P_DEFAULT_F)
      call h5awrite_f(desc_id,desctype_id,trim(description),strdims,ierror)
      call h5acreate_f(dset_id,"units",unitstype_id,aspace_id,units_id,ierror,H5P_DEFAULT_F,H5P_DEFAULT_F)
      call h5awrite_f(units_id,unitstype_id,trim(units),strdims,ierror)

      call H5aclose_f (desc_id,ierror)
      call H5aclose_f (units_id,ierror)
      call H5sclose_f (aspace_id,ierror)
      call H5tclose_f (desctype_id,ierror)
      call H5tclose_f (unitstype_id,ierror)
      call h5dclose_f (dset_id,ierror)
      call h5sclose_f (filespace_id,ierror)
      call h5sclose_f (memspace_id,ierror)
      return
      end
!--------------------------------------------------------------------------------------------MARK
      subroutine h5_write_1d_double (file_id,varname,description,units,local1darray,nlocal,dims,count,offset,localrank)
      use HDF5
      use orfiomod
      implicit none
      include 'input.incl'
#ifdef MPI
      include 'mpif.h'
#endif
      character(LEN=20) :: varname
      character*120  description
      character*20  units

      integer(HSIZE_T), DIMENSION(1) :: strdims
      integer :: arank = 1
      integer(HSIZE_T), DIMENSION(1) :: adims = (/1/)
      integer(SIZE_T) :: stringlen

      integer :: nlocal
      real*8, dimension(nlocal) ::  local1darray

      integer rank,localrank,ierror
      integer(HSIZE_T), DIMENSION(1) :: dims,count
      integer(HSSIZE_T), DIMENSION(1) :: offset
      integer(HID_T) :: file_id,dset_id,filespace_id,memspace_id,aspace_id,desc_id,desctype_id,unitstype_id,units_id
      integer(HID_T) :: plist_id

      if (iodebug2) call MPI_BARRIER(MPI_COMM_WORLD,ierror)
      if (iodebug2) print *,myid,' Writing 1d double: ',trim(varname)
      if (iodebug2) call FFLUSH(6)
      if (iodebug2) call MPI_BARRIER(MPI_COMM_WORLD,ierror)

      call h5screate_simple_f(arank,adims,aspace_id,ierror)
      call h5tcopy_f(H5T_NATIVE_CHARACTER,desctype_id,ierror)
      call h5tcopy_f(H5T_NATIVE_CHARACTER,unitstype_id,ierror)
      stringlen = len_trim(description)
      call h5tset_size_f(desctype_id,stringlen,ierror)
      stringlen = len_trim(units)
      call h5tset_size_f(unitstype_id,stringlen,ierror)

      ! filespace first
      rank=1
      strdims(1)=1

      call h5screate_simple_f(rank,dims,filespace_id,ierror)
      call h5dcreate_f(file_id,trim(varname),H5T_NATIVE_DOUBLE,filespace_id,dset_id,ierror)
      call h5sclose_f(filespace_id,ierror)

      !memoryspace:
      call h5screate_simple_f(rank,count,memspace_id,ierror)
      call h5dget_space_f(dset_id,filespace_id,ierror)

      if (localrank .ne. MPI_UNDEFINED) then
            call h5sselect_hyperslab_f(filespace_id,H5S_SELECT_SET_F,offset,count,ierror)
      else
            call h5sselect_none_f(filespace_id,ierror)
            call h5sselect_none_f(memspace_id,ierror)
      endif
      CALL h5pcreate_f(H5P_DATASET_XFER_F, plist_id, ierror)
      CALL h5pset_dxpl_mpio_f(plist_id, MPIO, ierror)
      CALL h5dwrite_f(dset_id, H5T_NATIVE_DOUBLE, local1darray, dims, ierror, &
                 file_space_id = filespace_id, mem_space_id = memspace_id, xfer_prp = plist_id)
      call h5acreate_f(dset_id,"description",desctype_id,aspace_id,desc_id,ierror,H5P_DEFAULT_F,H5P_DEFAULT_F)
      call h5awrite_f(desc_id,desctype_id,trim(description),strdims,ierror)
      call h5acreate_f(dset_id,"units",unitstype_id,aspace_id,units_id,ierror,H5P_DEFAULT_F,H5P_DEFAULT_F)
      call h5awrite_f(units_id,unitstype_id,trim(units),strdims,ierror)

      call H5aclose_f (desc_id,ierror)
      call H5aclose_f (units_id,ierror)
      call H5sclose_f (aspace_id,ierror)
      call H5tclose_f (desctype_id,ierror)
      call H5tclose_f (unitstype_id,ierror)
      call h5dclose_f (dset_id,ierror)
      call h5sclose_f (filespace_id,ierror)
      call h5sclose_f (memspace_id,ierror)
      return
      end
!--------------------------------------------------------------------------------------------MARK

! Fields along parcels are hardwired in parcel.F. If you add more fields, change
! here accordingly.

subroutine writepdata_hdf5(nrec,rtime,pdata)

      use HDF5
      implicit none
      include 'input.incl'

      character*160 filename
      integer :: nrec
      real :: rtime
      real, dimension(npvals,nparcels) :: pdata
      integer(SIZE_T) :: stringlen
      integer(HID_T) :: file_id,dset_id,dspace_id,chunk_id,datatype_id
      integer(HID_T) :: desctype_id,unitstype_id,desc_id,units_id,aspace_id
      integer(HID_T) :: parcel_group_id,memoryspace,filespace,time_id
      integer(HSIZE_T), dimension(2) :: maxdims,chunkdims,currentdims,singledims,offset
      integer(HSIZE_T), dimension(1) :: maxtimedims,timechunkdims,timecurrentdims,timesingledims,timeoffset
      integer(HID_T) :: timechunk_id,timememoryspace,timefilespace,timedset_id
      character(len=6), parameter :: parcel_group_name="parcel"
      character(len=120), parameter :: timedesc="model time"
      character(len=20), parameter :: timevar="time",timeunits="s"
      integer :: i,ierror
      integer :: rank,nvals

!     character, parameter :: varname(14) = (/character(len=20) :: &
      character(*), parameter :: varname(14) = (/  &
!     character(len=20), parameter :: varname(14) = (/character :: &
      'x', &
      'y', &
      'z', &
      'qv', &
      'qc', &
      'qr', &
      'nsq', &
      'u', &
      'v', &
      'w', &
      'kh', &
      'the', &
      'b', &
      'dpdz'/)

      character(*), parameter :: vardesc(14) = (/  &
      'x position of parcel', &
      'y position of parcel', &
      'z position of parcel', &
      'water vapor mixing ratio', &
      'cloud water mixing ratio', &
      'rain water mixing ratio', &
      'Brunt Vaisala frequency squared', &
      'u component of wind', &
      'v component of wind', &
      'w component of wind', &
      'turbulence coefficient for scalars', &
      'theta-e', &
      'buoyancy', &
      'dpdz tendency'/)

      character(*), parameter :: varunits(14) = (/  &
      'm','m','m', &
      'kg/kg','kg/kg','kg/kg', &
      'dimensionless', &
      'm/s','m/s','m/s', &
      'm^2/s','K', &
      'm/s^2', &
      'm/s^2'/)

      call h5open_f (ierror)

      maxdims(1) = nparcels
      maxdims(2) = H5S_UNLIMITED_F
      currentdims(1)=nparcels
      currentdims(2)=1
      singledims(1)=nparcels
      singledims(2)=1
      chunkdims(1) =nparcels
      chunkdims(2) = 1
      offset(1) = 0
      offset(2) = 0

      maxtimedims(1) = H5S_UNLIMITED_F
      timechunkdims(1) = 100
      timecurrentdims(1)=1
      timesingledims(1)=1
      timeoffset(1)=0

      timeoffset(1)=nrec-1
      timecurrentdims(1)=nrec
      offset(2) = nrec-1
      currentdims(2) = nrec

      filename = trim(output_basename)//'_pdata.hdf5'
      if(nrec.eq.1) then 
             call h5fcreate_f(trim(filename),H5F_ACC_TRUNC_F,file_id,ierror)
             call h5gcreate_f(file_id,parcel_group_name,parcel_group_id,ierror)      
      else
!            print *,'myid = ',myid,' nrec= ',nrec,' filename = ',trim(filename)
             call h5fopen_f(trim(filename),H5F_ACC_RDWR_F, file_id,ierror)
             call h5gopen_f(file_id,parcel_group_name,parcel_group_id,ierror)
      endif

      rank=1
      nvals=1
      call writefloat_hdf_unlimited(rank,nvals,nrec,parcel_group_id,timevar,timedesc,timeunits,time_id,timefilespace, &
            timememoryspace,timedset_id,timechunk_id,timechunkdims,maxtimedims,timecurrentdims,timesingledims,timeoffset,rtime)

      rank=2
      nvals=nparcels
      do i=1,14
            call writefloat_hdf_unlimited(rank,nvals,nrec,parcel_group_id,varname(i),vardesc(i), &
                  varunits(i),dspace_id,filespace,memoryspace,dset_id,chunk_id, &
                  chunkdims,maxdims,currentdims,singledims,offset,pdata(i,:))
            enddo

      nrec = nrec + 1
      call h5close_f (ierror)

      return
      end 


!--------------------------------------------------------------------------------------------MARK
! ORF 1/11/11
! Following routine works on data with an unlimited dimension,
! i.e., for creating and appending variables in parcel or stats hdf5 files

! Only one hdf file is written, model does a gather to root

subroutine writefloat_hdf_unlimited(rank,nvals,irec,file_id,varname,vardesc,varunits,dspace_id, &
      filespace,memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,floatval)

      use HDF5
      implicit none
      include 'input.incl'

      integer(HID_T) :: file_id,dset_id,dspace_id,chunk_id,datatype_id
      integer(HID_T) :: desctype_id,unitstype_id,desc_id,units_id,aspace_id
      integer(HID_T) :: parcel_group_id,memoryspace,filespace
      integer rank,nvals
      integer(HSIZE_T),dimension(rank) :: chunkdims,maxdims,currentdims,singledims,offset
      integer(HSIZE_T), DIMENSION(1) :: adims = (/1/)
      integer(HSIZE_T), DIMENSION(1) :: strdims = (/1/)
      real, dimension(nvals) :: floatval
      character(len=20) ::  varname
      character(len=120) :: vardesc
      character(len=20) :: varunits
      integer irec,ierror
      integer(SIZE_T) :: stringlen

!     print *, 'myid: ',myid,'irec: ',irec, varname
      if (irec.eq.1) then

            call h5screate_simple_f(rank, singledims, dspace_id, ierror, maxdims)
            call h5screate_simple_f(rank, singledims, memoryspace, ierror, singledims)
            call h5pcreate_f(H5P_DATASET_CREATE_F,chunk_id,ierror)
            call h5pset_chunk_f(chunk_id,rank,chunkdims,ierror)
! This is an example of how you would add gzip compression to unlimited dimension arrays for parcel and stats hdf5 files
!           if (output_format.eq.3.or.output_format.eq.4)call h5pset_deflate_f (chunk_id,1,ierror)

            call h5dcreate_f(file_id,trim(varname),H5T_NATIVE_REAL,dspace_id,dset_id,ierror,chunk_id)
            call h5dset_extent_f(dset_id,currentdims,ierror)
            call h5dget_space_f(dset_id,filespace,ierror)
            call h5sselect_hyperslab_f(filespace,H5S_SELECT_SET_F,offset,singledims,ierror)
            call h5dwrite_f(dset_id,H5T_NATIVE_REAL,floatval,singledims,ierror,memoryspace,filespace,H5P_DEFAULT_F)
            call h5sclose_f(memoryspace,ierror)
            call h5sclose_f(filespace,ierror)
            call h5sclose_f(dspace_id,ierror)
            call H5pclose_f(chunk_id,ierror)

            call h5tcopy_f(H5T_NATIVE_CHARACTER,desctype_id,ierror)
            stringlen = len_trim(vardesc)
            call h5tset_size_f(desctype_id,stringlen,ierror)
            call h5screate_simple_f(1,adims,aspace_id,ierror)
            call h5acreate_f(dset_id,"description",desctype_id,aspace_id,desc_id,ierror,H5P_DEFAULT_F,H5P_DEFAULT_F)
            call h5awrite_f(desc_id,desctype_id,vardesc,strdims,ierror)
            call H5aclose_f (desc_id,ierror)
            call H5sclose_f (aspace_id,ierror)
            call H5tclose_f (desctype_id,ierror)

            call h5tcopy_f(H5T_NATIVE_CHARACTER,desctype_id,ierror)
            stringlen = len_trim(varunits)
            call h5tset_size_f(desctype_id,stringlen,ierror)
            call h5screate_simple_f(1,adims,aspace_id,ierror)
            call h5acreate_f(dset_id,"units",desctype_id,aspace_id,desc_id,ierror,H5P_DEFAULT_F,H5P_DEFAULT_F)
            call h5awrite_f(desc_id,desctype_id,varunits,strdims,ierror)
            call H5aclose_f (desc_id,ierror)
            call H5sclose_f (aspace_id,ierror)
            call H5tclose_f (desctype_id,ierror)

            call h5dclose_f(dset_id,ierror)
      else
!           print *,'myid = ',myid,' irec= ',irec,' varname = ',varname
            call h5dopen_f(file_id,trim(varname),dset_id,ierror)
            call h5dset_extent_f(dset_id,currentdims,ierror)
            call h5dget_space_f(dset_id,filespace,ierror)
            call h5sselect_hyperslab_f(filespace,H5S_SELECT_SET_F,offset,singledims,ierror)
            call h5screate_simple_f(rank, singledims, memoryspace, ierror, singledims)
            call h5dwrite_f(dset_id,H5T_NATIVE_REAL,floatval,singledims,ierror,memoryspace,filespace,H5P_DEFAULT_F)
            call h5sclose_f(memoryspace,ierror)
            call h5sclose_f(filespace,ierror)
            call h5dclose_f(dset_id,ierror)
      endif
      return
end 


!--------------------------------------------------------------------------------------------MARK
! Only one hdf file is written, model does a gather to root, so we are only on
! rank 0 when in this routine
subroutine writestat_hdf5(nrec,rtime,nstat,rstat,qname,budname)

      use HDF5
      implicit none
      include 'input.incl'

      integer, intent(inout) :: nrec
      real,    intent(in)    :: rtime
      integer, intent(in)    :: nstat
      real, dimension(stat_out), intent(in) :: rstat
      character*3, dimension(maxq), intent(in) :: qname
      character*6, dimension(maxq), intent(in) :: budname

      character*160 filename
      integer(SIZE_T) :: stringlen
      integer(HID_T) :: file_id,dset_id,dspace_id,chunk_id,datatype_id
      integer(HID_T) :: desctype_id,unitstype_id,desc_id,units_id,aspace_id
      integer(HID_T) :: stats_group_id,memoryspace,filespace,var_id
      integer(HSIZE_T), dimension(1) :: maxdims,chunkdims,currentdims,singledims,offset
      character(len=6), parameter :: stats_group_name="stats"
      integer :: i,n,ierror
      integer :: rank,nvals
      character(len=20) :: varname
      character(len=20) :: units
      character(len=120)::  description
      character(len=8) :: text1
      character(len=30) :: text2
      integer :: stats_nrec

      call h5open_f (ierror)

      maxdims(1) = H5S_UNLIMITED_F
      chunkdims(1) = 100
      currentdims(1)=1
      singledims(1)=1
      offset(1)=0

      rank=1
      nvals=1

!     print *,'nrec = ',nrec
!     print *,'rtime = ',rtime

      filename = trim(output_basename)//'_stats.hdf5'
      if(nrec.eq.1) then 
             call h5fcreate_f(trim(filename),H5F_ACC_TRUNC_F,file_id,ierror)
             call h5gcreate_f(file_id,stats_group_name,stats_group_id,ierror)      
             call h5screate_simple_f(rank,singledims,filespace,ierror)
             call h5dcreate_f(file_id,'nrec',H5T_NATIVE_INTEGER,filespace,dset_id,ierror)
             stats_nrec = 0
      else
             call h5fopen_f(trim(filename),H5F_ACC_RDWR_F, file_id,ierror)
             call h5gopen_f(file_id,stats_group_name,stats_group_id,ierror)
             call h5dopen_f(file_id,'nrec',dset_id,ierror)
             call h5dread_f(dset_id,H5T_NATIVE_INTEGER,stats_nrec,singledims,ierror)
             call h5dclose_f(dset_id,ierror)
      endif

! ORF restarts need to be handled differently. We need to keep a record of where
! we left off in the stats file and only start writing after we catch up.
! It is highly likely that the stats file is not in sync with the restart file.
! stats_nrec is how many stats have been written to the stats file, is one less
! than regular nrec

      offset(1)=nrec-1
      currentdims(1)=nrec

      if ( (nrec-1) .eq. stats_nrec) then

      varname='time'
      description='model time'
      units='s'
      call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
            memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rtime)
 
      i=0
      if(adapt_dt.eq.1) then
            i=i+1
            varname='dt'
            description='average timestep (this simulation used adaptive timestepping)'
            units='s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
      endif
 
      if(stat_w.eq.1) then
            i=i+1
            varname='wmax'
            description='maximum vertical velocity'
            units='m/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='wmin'
            description='minimum vertical velocity'
            units='m/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
      endif
 
      if(stat_u.eq.1) then
            i=i+1
            varname='umax'
            description='maximum E/W wind speed'
            units='m/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='umin'
            description='minimum E/W wind speed'
            units='m/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='sumax'
            description='maximum surface E/W wind speed'
            units='m/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='sumin'
            description='minimum surface E/W wind speed'
            units='m/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
      endif
 
      if(stat_v.eq.1) then
            i=i+1
            varname='vmax'
            description='maximum N/S wind speed'
            units='m/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='vmin'
            description='minimum N/S wind speed'
            units='m/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='svmax'
            description='maximum surface N/S wind speed'
            units='m/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='svmin'
            description='minimum surface N/S wind speed'
            units='m/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
      endif
 
      if(stat_rmw.eq.1) then
            i=i+1
            varname='rmw'
            description='radius of maximum V'
            units='m'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
      endif
 
      if(stat_pipert.eq.1) then
            i=i+1
            varname='ppimax'
            description='maximum pi perturbation'
            units='dimensionless'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='ppimin'
            description='minimum pi perturbation'
            units='dimensionless'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
      endif

      if(stat_prspert.eq.1) then
            i=i+1
            varname='ppmax'
            description='maximum pressure perturbation'
            units='Pa'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='ppmin'
            description='minimum pressure perturbation'
            units='Pa'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
      endif
 
      if(stat_thpert.eq.1) then
            i=i+1
            varname='thpmax'
            description='maximum potential temperature perturbation'
            units='K'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='thpmin'
            description='minimum potential temperature perturbation'
            units='K'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='sthpmax'
            description='maximum surface potential temperature perturbation'
            units='K'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='sthpmin'
            description='minimum surface potential temperature perturbation'
            units='K'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
      endif
 
      if(stat_q.eq.1) then
            do n=1,numq
                  text1='max     '
                  text2='max                           '
                  write(text1(4:6),156) qname(n)
                  write(text2(5:7),156) qname(n)
156 format(a3)
                  i=i+1
                  varname=text1
                  description=text2
                  units='kg/kg'
                  call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                        memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
 
                  text1='min     '
                  text2='min                           '
                  write(text1(4:6),156) qname(n)
                  write(text2(5:7),156) qname(n)
                  i=i+1
                  varname=text1
                  description=text2
                  units='kg/kg'
                  call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                        memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            enddo
      endif

      if(stat_tke.eq.1) then
            i=i+1
            varname='tkemax'
            description='maximum turbulent kinetic energy'
            units='m^2/s^2'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='tkemin'
            description='minimum turbulent kinetic energy'
            units='m^2/s^2'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
      endif
 
      if(stat_km.eq.1) then
            i=i+1
            varname='kmhmax'
            description='maximum horizontal eddy mixing coefficient for momentum'
            units='m^2/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='kmhmin'
            description='minimum horizontal eddy mixing coefficient for momentum'
            units='m^2/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='kmvmax'
            description='maximum vertical eddy mixing coefficient for momentum'
            units='m^2/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='kmvmin'
            description='minimum vertical eddy mixing coefficient for momentum'
            units='m^2/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
      endif
 
      if(stat_kh.eq.1) then
            i=i+1
            varname='khhmax'
            description='maximum horizontal eddy mixing coefficient for scalars'
            units='m^2/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='khhmin'
            description='minimum horizontal eddy mixing coefficient for scalars'
            units='m^2/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='khvmax'
            description='maximum vertical eddy mixing coefficient for scalars'
            units='m^2/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='khvmin'
            description='minimum vertical eddy mixing coefficient for scalars'
            units='m^2/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
      endif
 
      if(stat_div.eq.1) then
            i=i+1
            varname='divmax'
            description='maximum 3D divergence'
            units='s^{-1}'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='divmin'
            description='minimum 3D divergence'
            units='s^{-1}'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
      endif
 
      if(stat_rh.eq.1) then
            i=i+1
            varname='rhmax'
            description='maximum relative humidity'
            units='dimensionless'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='rhmin'
            description='minimum relative humidity'
            units='dimensionless'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
      endif
 
      if(stat_rhi.eq.1) then
            i=i+1
            varname='rhimax'
            description='maximum relative humidity wrt ice'
            units='dimensionless'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='rhimin'
            description='minimum relative humidity wrt ice'
            units='dimensionless'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
      endif
 
      if (iptra.eq.1) then
            do n=1,npt
                  text1='maxpt   '
                  text2='max pt                        '
                  write(text1(6:6),157) n
                  write(text2(7:7),157) n
157 format(i1)
                  i=i+1
                  varname=text1
                  description=text2
                  units='kg/kg'
                  call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                        memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
 
                  text1='minpt   '
                  text2='min pt                        '
                  write(text1(6:6),157) n
                  write(text2(7:7),157) n
                  i=i+1
                  varname=text1
                  description=text2
                  units='kg/kg'
                  call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                        memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            enddo
      endif
 
      if(stat_the.eq.1) then
            i=i+1
            varname='themax'
            description='maximum theta-e below 10 km'
            units='K'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='themin'
            description='minimum theta-e below 10 km'
            units='K'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='sthemax'
            description='maximum surface theta-e'
            units='K'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='sthemin'
            description='minimum surface theta-e'
            units='K'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
      endif
 
      if(stat_cloud.eq.1) then
            i=i+1
            varname='qctop'
            description='maximum cloud top height'
            units='m'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='qcbot'
            description='minimum cloud base height'
            units='m'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
      endif
 
      if(stat_sfcprs.eq.1) then
            i=i+1
            varname='sprsmax'
            description='maximum surface pressure'
            units='Pa'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='sprsmin'
            description='minumum surface pressure'
            units='Pa'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
      endif
 
      if(stat_wsp.eq.1) then
            i=i+1
            varname='wspmax'
            description='maximum wind speed'
            units='m/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='wspmin'
            description='minumum wind speed'
            units='m/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='swspmax'
            description='maximum surface wind speed'
            units='m/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='swspmin'
            description='minumum surface wind speed'
            units='m/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            if(idrag.eq.1)then
                  i=i+1
                  varname='wsp10max'
                  description='maximum 10 m wind speed'
                  units='m/s'
                  call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                        memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
                  i=i+1
                  varname='wsp10min'
                  description='minimum 10 m wind speed'
                  units='m/s'
                  call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                        memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            endif
      endif
 
      if(stat_cfl.eq.1) then
            if(adapt_dt.eq.1) then
                  i=i+1
                  varname='cflmax'
                  description='maximum Courant number (average - model ran with adaptive time stepping)'
                  units='dimensionless'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            else
                  i=i+1
                  varname='cflmax'
                  description='maximum Courant number'
                  units='dimensionless'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            endif
 
            i=i+1
            varname='kshmax'
            description='maximum horizontal K stability factor'
            units='dimensionless'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
 
            i=i+1
            varname='ksvmax'
            description='maximum vertical K stability factor'
            units='dimensionless'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
      endif
 
      if (stat_vort.eq.1) then
            i=i+1
            varname='vortsfc'
            description='maximum surface vertical vorticity'
            units='s^{-1}'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
 
            i=i+1
            varname='vort1km'
            description='maximum 1 km vertical vorticity'
            units='s^{-1}'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
 
            i=i+1
            varname='vort2km'
            description='maximum 2 km vertical vorticity'
            units='s^{-1}'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
 
            i=i+1
            varname='vort3km'
            description='maximum 3 km vertical vorticity'
            units='s^{-1}'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
 
            i=i+1
            varname='vort4km'
            description='maximum 4 km vertical vorticity'
            units='s^{-1}'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
 
            i=i+1
            varname='vort5km'
            description='maximum 5 km vertical vorticity'
            units='s^{-1}'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
      endif
 
      if(stat_tmass.eq.1) then
            i=i+1
            varname='tmass'
            description='total mass of dry air'
            units='kg'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
      endif
      if(stat_tmois.eq.1) then
            i=i+1
            varname='tmois'
            description='total mass of water vapor in air (doublecheck)'
            units='kg'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
      endif
! Note, tmois seems to be same as massqv 
      if(stat_qmass.eq.1)then
            do n=1,numq
                  if( (n.eq.nqv) .or.                                 &
                      (n.ge.nql1.and.n.le.nql2) .or.                  &
                      (n.ge.nqs1.and.n.le.nqs2.and.iice.eq.1) )then
                            text1='mass    '
                            text2='total mass of                 '
                            write(text1( 5: 7),156) qname(n)
                            write(text2(15:17),156) qname(n)
 
                            i=i+1
                            varname=text1
                            description=text2
                            units='kg'
                            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
                   endif
             enddo
       endif
 
      if(stat_tenerg.eq.1) then
            i=i+1
            varname='ek'
            description='total kinetic energy'
            units='J'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='ei'
            description='total internal energy'
            units='J'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='ep'
            description='total potential energy'
            units='J'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='le'
            description='total latent energy'
            units='J'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='et'
            description='total energy'
            units='J'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
      endif
 
      if(stat_mo.eq.1) then
            i=i+1
            varname='tmu'
            description='total E-W momentum '
            units='kg m/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='tmv'
            description='total N-S momentum '
            units='kg m/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='tmw'
            description='total vertical momentum '
            units='kg m/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
      endif
 
      if(stat_tmf.eq.1) then
            i=i+1
            varname='tmfu'
            description='total upward mass flux'
            units='kg/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            i=i+1
            varname='tmfd'
            description='total downward mass flux'
            units='kg/s'
            call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                  memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
      endif
      if(stat_pcn.eq.1)then
            do n=1,nbudget
                  text1='        '
                  text2='                              '
                  write(text1(1:6),158) budname(n)
                  write(text2(1:6),158) budname(n)
158 format(a6)
                  i=i+1
                  varname=text1
                  description=trim(text2)//' (microphysics water budget in qbudget array - try `grep qbudget solve.F`)'
                  units='kg'
                  call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                        memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            enddo
      endif
 
      IF(stat_qsrc.eq.1)THEN
            do n=1,numq
                  text1='as      '
                  text2='artificial source of          '
                  write(text1( 3: 5),156) qname(n)
                  write(text2(22:24),156) qname(n)
                  i=i+1
                  varname=text1
                  description=text2
                  units='kg'
                  call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                        memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            enddo
            do n=1,numq
                  text1='bs      '
                  text2='bndry source/sink of          '
                  write(text1( 3: 5),156) qname(n)
                  write(text2(22:24),156) qname(n)
                  i=i+1
                  varname=text1
                  description=text2
                  units='kg'
                  call writefloat_hdf_unlimited(rank,nvals,nrec,stats_group_id,varname,description,units,var_id,filespace, &
                        memoryspace,dset_id,chunk_id,chunkdims,maxdims,currentdims,singledims,offset,rstat(i))
            enddo
      endif

! Note: i should now equal stat_out (see procXXXXX.out file) or something is wrong
      if (i .ne. stat_out) then
            print *, 'Number of stats written in writestat_hdf5: ',i 
            print *, 'Number of stats caculated in param: ',stat_out
            print *, 'These two must be equal, something is wrong, likely in writestat_hdf5'
            call h5close_f (ierror)
            call stopcm1
      endif

!Now write nrec, only if we've gotten in here

      stats_nrec = nrec
      call h5dopen_f(file_id,'nrec',dset_id,ierror)
      call h5dwrite_f(dset_id,H5T_NATIVE_INTEGER,stats_nrec,singledims,ierror)
      call h5dclose_f(dset_id,ierror)
      endif ! stats_nrec .eq. nrec

! However we need  to increment nrec whether we are caught up or not otherwise
! we'll never catch up!

      nrec = nrec + 1

      call h5close_f (ierror)
#endif
! above endif matches #ifdef HDFOUT
      return
      end 

#ifdef HDF5RESTART
!--------------------------------------------------------------------------------------------MARK
! NOTE! This has not been fully tested yet, use at your own risk.
      subroutine write_restart_hdf5(nrec,prec,nwrite,nrst,         &
                               qbudget,asq,bsq,                          &
                               rain,sws,radbcw,radbce,radbcs,radbcn,     &
                               ua,va,wa,ppi,tha,qa,tkea,pta,pdata,rtime)
      use HDF5
      implicit none

      include 'input.incl'
      include 'constants.incl'

      real :: rtime
      integer nstep,nrec,prec,nwrite,nrst,itime
      real*8, dimension(nbudget) :: qbudget
      real*8, dimension(numq) :: asq,bsq
      real, dimension(ib:ie,jb:je,nrain) :: rain,sws
      real, dimension(jb:je,kb:ke) :: radbcw,radbce
      real, dimension(ib:ie,kb:ke) :: radbcs,radbcn
      real, dimension(ib:ie+1,jb:je,kb:ke) :: ua
      real, dimension(ib:ie,jb:je+1,kb:ke) :: va
      real, dimension(ib:ie,jb:je,kb:ke+1) :: wa
      real, dimension(ib:ie,jb:je,kb:ke) :: ppi,tha
      real, dimension(ibm:iem,jbm:jem,kbm:kem,numq) :: qa
      real, dimension(ibt:iet,jbt:jet,kbt:ket) :: tkea
      real, dimension(ibp:iep,jbp:jep,kbp:kep,npt) :: pta
      real, dimension(npvals,nparcels) :: pdata

      character(len=160) filename
      character(len=120)  description
      character(len=20)   units
      character(len=20)   varname
      integer lt,flag,n
      character*5 ctime,crank
      character*4 cid
      integer ierror
      integer(HID_T) file_id,dspace_id,dset_id
      integer(HSIZE_T), dimension(4) :: dims,chunkdims
      character(LEN=7), parameter :: restart_group_name = 'restart'


            write(ctime,1102)int(rtime)
            write(cid,1103)ioid
      1102   format(i5.5)
      1103   format(i4.4)
            filedir = trim(output_path)//'/'//trim(output_basename)//'.'//ctime//'.rdir/'
            dircmd = 'mkdir -p '// trim(filedir)
            filename = trim(filedir)//trim(output_basename)//'.'//ctime//'_'//cid//'.restart_cm1hdf5'
            if (myid.eq.0) call system(dircmd)

            call h5pcreate_f(H5P_FILE_ACCESS_F, plist_id, ierror)
            info = MPI_INFO_NULL
            call h5pset_fapl_mpio_f(plist_id, mysubgroupcomm, info, ierror)
            call h5fcreate_f(trim(filename),H5F_ACC_TRUNC_F,file_id,ierror,access_prp=plist_id)
            call h5pclose_f(plist_id, ierror)

            if(myid.eq.0)then
                  write(outfile,*)
                  write(outfile,*) '  Writing to restart file:'
                  write(outfile,*) '  filename=',filename
                  write(outfile,*)
            endif

            varname='nstep'
            description='nstep'
            units='index'
            call h5_write_int  (file_id,varname,description,units,nx,.false.)

            varname='nrec'
            description='nrec'
            units='index'
            call h5_write_int  (file_id,varname,description,units,nx,.false.)

            varname='prec'
            description='prec'
            units='index'
            call h5_write_int  (file_id,varname,description,units,nx,.false.)

            varname='nwrite'
            description='nwrite'
            units='index'
            call h5_write_int  (file_id,varname,description,units,nx,.false.)

            varname='nrst'
            description='nrst'
            units='index'
            call h5_write_int  (file_id,varname,description,units,nx,.false.)

            varname='rtime'
            description='model time'
            units='s'
            call h5_write_float  (file_id,varname,description,units,nx,.false.)

            write(crank,1102)mysubgrouprank

            varname=trim('qbudget'//'_'//crank)
            description='qbudget'
            units='derp'
            dims(1)=nbudget;count(1)=nbudget;offset(1)=0
            call h5_write_1d_double(file_id,varname,description,units,qbudget,nbudget,dims,count,offset,mysubgrouprank)

            varname=trim('asq'//'_'//crank)
            description='asq'
            units='derp'
            dims(1)=numq;count(1)=numq;offset(1)=0
            call h5_write_1d_double(file_id,varname,description,units,qbudget,numq,dims,count,offset,mysubgrouprank)

            varname=trim('bsq'//'_'//crank)
            description='bsq'
            units='derp'
            dims(1)=numq;count(1)=numq;offset(1)=0
            call h5_write_1d_double(file_id,varname,description,units,qbudget,numq,dims,count,offset,mysubgrouprank)

            varname='rain'
            description='rain'
            units='derp'
! I am abandoning this code. It is too much of a hassle to cram restart
! files into parallel hdf5 files without creating new writetout routines
! because of all the different dimensions to deal with. I could go back
! to my old way (one file per mpi rank) but really what's the point,
! George already has his code working and restart files are only used
! for restarting, not analysis.

! Only caveat would be for very large simulations, only write 3000
! files or so at a time, and/or perhaps split them up into separate
! directories.

            call h5_write_3d_float(file_id,varname,description,units,rain,corexyztype,0,0,0)
      call h5_write_3d_float(group_id,  'rain                ',0,rain,ie-ib+1,je-jb+1,nrain,.false.)
      call h5_write_3d_float(group_id,  'sws                 ',0,sws,ie-ib+1,je-jb+1,nrain,.false.)
      call h5_write_3d_float(group_id,  'ua                  ',0,ua,ie+1-ib+1,je-jb+1,ke-kb+1,.false.)
      call h5_write_3d_float(group_id,  'va                  ',0,va,ie-ib+1,je+1-jb+1,ke-kb+1,.false.)
      call h5_write_3d_float(group_id,  'wa                  ',0,wa,ie-ib+1,je-jb+1,ke+1-kb+1,.false.)
      call h5_write_3d_float(group_id,  'ppi                 ',0,ppi,ie-ib+1,je-jb+1,ke-kb+1,.false.)
      call h5_write_3d_float(group_id,  'tha                 ',0,tha,ie-ib+1,je-jb+1,ke-kb+1,.false.)
      if(imoist.eq.1) call h5_write_4d_float(group_id,'qa                  ',0,qa,iem-ibm+1,jem-jbm+1,kem-kbm+1,numq,.false.)
      if(iturb.eq.1)  call h5_write_3d_float(group_id,'tkea                ',0,tkea,iet-ibt+1,jet-jbt+1,ket-kbt+1,.false.)
      if(iptra.eq.1)  call h5_write_4d_float(group_id,'pta                 ',0,pta,iep-ibp+1,jep-jbp+1,kep-kbp+1,npt,.false.)
      if(iprcl.eq.1)  call h5_write_2d_float(group_id,'pdata               ',pdata,npvals,nparcels)
      if(irbc.eq.4.and.ibw.eq.1) call h5_write_2d_float(group_id,'radbcw              ',radbcw,je-jb+1,ke-kb+1)
      if(irbc.eq.4.and.ibe.eq.1) call h5_write_2d_float(group_id,'radbce              ',radbce,je-jb+1,ke-kb+1)
      if(irbc.eq.4.and.ibs.eq.1) call h5_write_2d_float(group_id,'radbcs              ',radbcs,ie-ib+1,ke-kb+1)
      if(irbc.eq.4.and.ibn.eq.1) call h5_write_2d_float(group_id,'radbcn              ',radbcn,ie-ib+1,ke-kb+1)

      call h5close_f(ierror)

      return
      end

!--------------------------------------------------------------------------------------------MARK
! NOTE! This has not been fully tested yet, use at your own risk.

      subroutine read_restart_hdf5(nloop1,nrec,prec,nwrite,nrst,taptim,rsttim,   &
                              qbudget,asq,bsq,                              &
                              rain,sws,radbcw,radbce,radbcs,radbcn,         &
                              ua,va,wa,ppi,tha,qa,tkea,pta,pdata,rtime)
      use HDF5
      implicit none

      include 'input.incl'
      include 'constants.incl'

      integer nloop1,nrec,prec,nwrite,nrst,itime
      real*8 taptim,rsttim
      real*8, dimension(nbudget) :: qbudget
      real*8, dimension(numq) :: asq,bsq
      real, dimension(ib:ie,jb:je,nrain) :: rain,sws
      real, dimension(jb:je,kb:ke) :: radbcw,radbce
      real, dimension(ib:ie,kb:ke) :: radbcs,radbcn
      real, dimension(ib:ie+1,jb:je,kb:ke) :: ua
      real, dimension(ib:ie,jb:je+1,kb:ke) :: va
      real, dimension(ib:ie,jb:je,kb:ke+1) :: wa
      real, dimension(ib:ie,jb:je,kb:ke) :: ppi,tha
      real, dimension(ibm:iem,jbm:jem,kbm:kem,numq) :: qa
      real, dimension(ibt:iet,jbt:jet,kbt:ket) :: tkea
      real, dimension(ibp:iep,jbp:jep,kbp:kep,npt) :: pta
      real, dimension(npvals,nparcels) :: pdata
      real rtime

      character*160 filename
      integer lt,flag,n
      character*5 ctime
      character*4 cid
      integer ierror
      integer(HID_T) file_id,group_id,dspace_id,dset_id
      integer(HSIZE_T), dimension(4) :: dims
      character(LEN=7), parameter :: restart_group_name = 'restart'

      call create_output_filename(mymcmid,filename,rstnum,'.cm1hdf5.restart     ')

      write(outfile,*)
      write(outfile,*) '  Reading from restart file!'
      write(outfile,*) '  filename=',filename
      write(outfile,*)

      call h5open_f(ierror)
      call h5fopen_f(filename,H5F_ACC_RDONLY_F,file_id,ierror)

      dims(1)=1

      call h5dopen_f(file_id,'/restart/nrec',dset_id,ierror)
      call h5dread_f(dset_id,H5T_NATIVE_INTEGER,nrec,dims,ierror)
      call h5dclose_f(dset_id,ierror)

      call h5dopen_f(file_id,'/restart/prec',dset_id,ierror)
      call h5dread_f(dset_id,H5T_NATIVE_INTEGER,prec,dims,ierror)
      call h5dclose_f(dset_id,ierror)

      call h5dopen_f(file_id,'/restart/nwrite',dset_id,ierror)
      call h5dread_f(dset_id,H5T_NATIVE_INTEGER,nwrite,dims,ierror)
      call h5dclose_f(dset_id,ierror)

      call h5dopen_f(file_id,'/restart/nrst',dset_id,ierror)
      call h5dread_f(dset_id,H5T_NATIVE_INTEGER,nrst,dims,ierror)
      call h5dclose_f(dset_id,ierror)

      call h5dopen_f(file_id,'/restart/rtime',dset_id,ierror)
      call h5dread_f(dset_id,H5T_NATIVE_REAL,rtime,dims,ierror)
      call h5dclose_f(dset_id,ierror)

      dims(1)=nbudget
      call h5dopen_f(file_id,'/restart/qbudget',dset_id,ierror)
      call h5dread_f(dset_id,H5T_NATIVE_DOUBLE,qbudget,dims,ierror)
      call h5dclose_f(dset_id,ierror)

      dims(1)=numq
      call h5dopen_f(file_id,'/restart/asq',dset_id,ierror)
      call h5dread_f(dset_id,H5T_NATIVE_DOUBLE,asq,dims,ierror)
      call h5dclose_f(dset_id,ierror)

      dims(1)=numq
      call h5dopen_f(file_id,'/restart/bsq',dset_id,ierror)
      call h5dread_f(dset_id,H5T_NATIVE_DOUBLE,bsq,dims,ierror)
      call h5dclose_f(dset_id,ierror)

      dims(1)=ie-ib+1
      dims(2)=je-jb+1
      dims(3)=nrain
      call h5dopen_f(file_id,'/restart/rain',dset_id,ierror)
      call h5dread_f(dset_id,H5T_NATIVE_REAL,rain,dims,ierror)
      call h5dclose_f(dset_id,ierror)

      dims(1)=ie-ib+1
      dims(2)=je-jb+1
      dims(3)=nrain
      call h5dopen_f(file_id,'/restart/sws',dset_id,ierror)
      call h5dread_f(dset_id,H5T_NATIVE_REAL,sws,dims,ierror)
      call h5dclose_f(dset_id,ierror)

      dims(1)=ie+1-ib+1
      dims(2)=je-jb+1
      dims(3)=ke-kb+1
      call h5dopen_f(file_id,'/restart/ua',dset_id,ierror)
      call h5dread_f(dset_id,H5T_NATIVE_REAL,ua,dims,ierror)
      call h5dclose_f(dset_id,ierror)

      dims(1)=ie-ib+1
      dims(2)=je+1-jb+1
      dims(3)=ke-kb+1
      call h5dopen_f(file_id,'/restart/va',dset_id,ierror)
      call h5dread_f(dset_id,H5T_NATIVE_REAL,va,dims,ierror)
      call h5dclose_f(dset_id,ierror)

      dims(1)=ie-ib+1
      dims(2)=je-jb+1
      dims(3)=ke+1-kb+1
      call h5dopen_f(file_id,'/restart/wa',dset_id,ierror)
      call h5dread_f(dset_id,H5T_NATIVE_REAL,wa,dims,ierror)
      call h5dclose_f(dset_id,ierror)

      dims(1)=ie-ib+1
      dims(2)=je-jb+1
      dims(3)=ke-kb+1
      call h5dopen_f(file_id,'/restart/ppi',dset_id,ierror)
      call h5dread_f(dset_id,H5T_NATIVE_REAL,ppi,dims,ierror)
      call h5dclose_f(dset_id,ierror)

      dims(1)=ie-ib+1
      dims(2)=je-jb+1
      dims(3)=ke-kb+1
      call h5dopen_f(file_id,'/restart/tha',dset_id,ierror)
      call h5dread_f(dset_id,H5T_NATIVE_REAL,tha,dims,ierror)
      call h5dclose_f(dset_id,ierror)

      if (imoist.eq.1) then
            dims(1)=iem-ibm+1
            dims(2)=jem-jbm+1
            dims(3)=kem-kbm+1
            dims(4)=numq
            call h5dopen_f(file_id,'/restart/qa',dset_id,ierror)
            call h5dread_f(dset_id,H5T_NATIVE_REAL,qa,dims,ierror)
            call h5dclose_f(dset_id,ierror)
      endif

      if (iturb.eq.1) then
            dims(1)=iet-ibt+1
            dims(2)=jet-jbt+1
            dims(3)=ket-kbt+1
            call h5dopen_f(file_id,'/restart/tkea',dset_id,ierror)
            call h5dread_f(dset_id,H5T_NATIVE_REAL,tkea,dims,ierror)
            call h5dclose_f(dset_id,ierror)
      endif

      if (iptra.eq.1) then
            dims(1)=iep-ibp+1
            dims(2)=jep-jbp+1
            dims(3)=kep-kbp+1
            dims(4)=npt
            call h5dopen_f(file_id,'/restart/pta',dset_id,ierror)
            call h5dread_f(dset_id,H5T_NATIVE_REAL,pta,dims,ierror)
            call h5dclose_f(dset_id,ierror)
      endif

      if (iprcl.eq.1) then
            dims(1)=npvals
            dims(2)=nparcels
            call h5dopen_f(file_id,'/restart/pdata',dset_id,ierror)
            call h5dread_f(dset_id,H5T_NATIVE_REAL,pdata,dims,ierror)
            call h5dclose_f(dset_id,ierror)
      endif

      if (irbc.eq.4.and.ibw.eq.1) then
            dims(1)=je-jb+1
            dims(2)=ke-kb+1
            call h5dopen_f(file_id,'/restart/radbcw',dset_id,ierror)
            call h5dread_f(dset_id,H5T_NATIVE_REAL,radbcw,dims,ierror)
            call h5dclose_f(dset_id,ierror)
      endif

      if (irbc.eq.4.and.ibe.eq.1) then
            dims(1)=je-jb+1
            dims(2)=ke-kb+1
            call h5dopen_f(file_id,'/restart/radbce',dset_id,ierror)
            call h5dread_f(dset_id,H5T_NATIVE_REAL,radbce,dims,ierror)
            call h5dclose_f(dset_id,ierror)
      endif

      if (irbc.eq.4.and.ibs.eq.1) then
            dims(1)=ie-ib+1
            dims(2)=ke-kb+1
            call h5dopen_f(file_id,'/restart/radbcs',dset_id,ierror)
            call h5dread_f(dset_id,H5T_NATIVE_REAL,radbcs,dims,ierror)
            call h5dclose_f(dset_id,ierror)
      endif

      if (irbc.eq.4.and.ibn.eq.1) then
            dims(1)=ie-ib+1
            dims(2)=ke-kb+1
            call h5dopen_f(file_id,'/restart/radbcn',dset_id,ierror)
            call h5dread_f(dset_id,H5T_NATIVE_REAL,radbcn,dims,ierror)
            call h5dclose_f(dset_id,ierror)
      endif

      call h5close_f (ierror)

      nloop1=nint(rtime/dtl)+1
      taptim=rtime+tapfrq
      rsttim=rtime+rstfrq
      prcltim=rtime+prclfrq
      nrec=nrec-stat_out

      write(outfile,*) nloop1,nrec,taptim,rsttim
      write(outfile,*)

      return
      end

#endif

!--------------------------------------------------------------------------------------------MARK

