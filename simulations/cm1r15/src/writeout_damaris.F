

subroutine writeout_mult_damaris(model_time,qname,sigma,sigmaf,xh,xf,uf,yh,yf,vf,mh,zh,mf,zf,    &
                pi0,prs0,rho0,th0,qv0,u0,v0,                        &
                zs,rain,sws,svs,sps,srs,sgs,sus,shs,thflux,qvflux,cdu,cdv,ce,dum1,dum2,dum3,dum4, &
                rho,prs,dbz,ua,dumu,va,dumv,wa,dumw,ppi,tha,        &
                qa,kmh,kmv,khh,khv,tkea,pta,num_soil_layers,   &
                lu_index,xland,mavail,tsk,tmn,tml,hml,huml,hvml,hfx,qfx,gsw,glw,tslb,   &
                radsw,rnflx,radswnet,radlwin,u10,v10,t2,q2,znt,ust,hpbl,zol,mol,br,  &
                dissten,thpten,qvpten,qcpten,qipten,upten,vpten,swten,lwten)
      implicit none

      include 'input.incl'
      include 'constants.incl'
#ifdef MPI
      include 'mpif.h'
#endif

      real :: model_time
      integer :: int_time
      character*3, dimension(maxq) :: qname
      real, dimension(kb:ke) :: sigma
      real, dimension(kb:ke+1) :: sigmaf
      real, dimension(ib:ie)   :: xh
      real, dimension(ib:ie+1) :: xf,uf
      real, dimension(jb:je)   :: yh
      real, dimension(jb:je+1) :: yf,vf
      real, dimension(ib:ie,jb:je,kb:ke) :: mh,zh,pi0,prs0,rho0,th0,qv0
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


#ifdef DAMARIS
      real, dimension(ni,nj,nk) :: ds
      real, dimension(ni,nj) :: ds2
      real, dimension(1:nk) :: zhcol,zfcol,u0col,v0col,th0col,pi0col,rho0col,pres0col,qv0col
      real, dimension(1:ni) :: xfcol,xhcol
      real, dimension(1:nj) :: yfcol,yhcol

      integer i,j,k,n
      character(len=20)   varname
      character(len=160)  filename
      character(LEN=20) :: qvarname
      character(len=8) chid

      integer ierror
      integer sd_id, sds_id, status, dim_id, data_type
      integer ix,iy
      integer rank
      integer itime,lt,flag
      integer x0,x1,y0,y1; ! bounding box for each MPI rank within total domain

#ifdef MPI
      call MPI_BARRIER (MPI_COMM_WORLD,ierr)
#endif
      int_time = int(model_time)

      x0 = (myi-1)*ni;
      x1 = myi*ni-1;
      y0 = (myj-1)*nj;
      y1 = myj*nj-1;

      varname='time'
      call damaris_write_float(varname,int_time,model_time)

      varname='dx'
      call damaris_write_float(varname,int_time,dx)

      varname='dy'
      call damaris_write_float(varname,int_time,dy)

      varname='x0'
      call damaris_write_int  (varname,int_time,x0)

      varname='x1'
      call damaris_write_int  (varname,int_time,x1)

      varname='y0'
      call damaris_write_int  (varname,int_time,y0)

      varname='y1'
      call damaris_write_int  (varname,int_time,y1)

      varname='myi'
      call damaris_write_int  (varname,int_time,myi)

      varname='myj'
      call damaris_write_int  (varname,int_time,myj)

      varname='ni'
      call damaris_write_int  (varname,int_time,ni)

      varname='nj'
      call damaris_write_int  (varname,int_time,nj)

      varname='nodex'
      call damaris_write_int  (varname,int_time,nodex)

      varname='nodey'
      call damaris_write_int  (varname,int_time,nodey)

      varname='nx'
      call damaris_write_int  (varname,int_time,nx)

      varname='ny'
      call damaris_write_int  (varname,int_time,ny)

      varname='nz'
      call damaris_write_int  (varname,int_time,nz)

! Always write out 1D vertical base state column varables. Full 3D
! base state variables are written below if output_basestate.eq.1 for
! you folks who run with terrain. If you have terrain then ignore the
! following 1D basestate variables (i.e., x0col(k) )

! In other words, if you run with terrain and want base state
! variables, set output_basesteate.eq.1 and use vars in group
! /3d_basestate. If you do not run with terrain (flat bottom), you can
! set output_basestate.eq.0 and use vars in /basestate since all base
! state variables will be horizontlaly homogeneous. 1D vars save space
! over 3D vars. Of course nothing is stopping you from writing full 3D
! basestate even if you are not running with terrain!

      do k=1,nk
          zhcol(k)=zh(1,1,k)
          zfcol(k)=zf(1,1,k)
          u0col(k) = u0(1,1,k)
          v0col(k) = v0(1,1,k)
          th0col(k) = th0(1,1,k)
          pi0col(k) = pi0(1,1,k)
          rho0col(k) = rho0(1,1,k)
          pres0col(k) = p00*(pi0(1,1,k))**cpdrd
          qv0col(k) = qv0(1,1,k)
      enddo

      do j = 1,nj
          yfcol(j) = yf(j)
          yhcol(j) = yh(j)
      enddo

      do i = 1,ni
          xfcol(i) = xf(i)
          xhcol(i) = xh(i)
      enddo

      varname='zh'
      call damaris_write_1d_float(varname,int_time,zhcol,0,nk-1)

      varname='zf'
      call damaris_write_1d_float(varname,int_time,zfcol,0,nk-1)

      varname='yh'
      call damaris_write_1d_float(varname,int_time,yhcol,y0,y1)

      varname='yf'
      call damaris_write_1d_float(varname,int_time,yfcol,y0,y1)

      varname='xh'
      call damaris_write_1d_float(varname,int_time,xhcol,x0,x1)

      varname='xf'
      call damaris_write_1d_float(varname,int_time,xfcol,x0,x1)

      varname='u0'
      call damaris_write_1d_float(varname,int_time,u0col,0,nk-1)

      varname='v0'
      call damaris_write_1d_float(varname,int_time,v0col,0,nk-1)

      varname='th0'
      call damaris_write_1d_float(varname,int_time,th0col,0,nk-1)

      varname='pi0'
      call damaris_write_1d_float(varname,int_time,pi0col,0,nk-1)

      varname='pres0'
      call damaris_write_1d_float(varname,int_time,pres0col,0,nk-1)

      varname='rh0'
      call damaris_write_1d_float(varname,int_time,rho0col,0,nk-1)

! terrain

     if (output_zs.eq.1.and.terrain_flag) then
       do j=1,nj
       do i=1,ni
         ds2(i,j)=zs(i,j)
       enddo
       enddo

       varname='zs'
       call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

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
       call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)

     endif
            

     if (output_basestate.eq.1) then

          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=pi0(i,j,k)
          enddo
          enddo
          enddo

          varname='pi0'
          call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)

          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=th0(i,j,k)
          enddo
          enddo
          enddo

          varname='th0'
          call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)

          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=prs0(i,j,k)
          enddo
          enddo
          enddo

          varname='p0'
          call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)

          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=qv0(i,j,k)
          enddo
          enddo
          enddo

          varname='qv0'
          call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)

          do k=1,nk
          do j=1,nj
          do i=1,ni+1
            dumu(i,j,k)=u0(i,j,k)
          enddo
          enddo
          enddo

          varname='u0'
          call damaris_write_3d_float(varname,int_time,dumu,(myi-1)*(ni+1),myi*(ni+1)-1,y0,y1,0,nk-1)

          do k=1,nk
          do j=1,nj+1
          do i=1,ni
            dumv(i,j,k)=v0(i,j,k)
          enddo
          enddo
          enddo

          varname='v0'
          call damaris_write_3d_float(varname,int_time,dumv,x0,x1,(myj-1)*(nj+1),myj*(nj+1)-1,0,nk-1)

      endif !output_basestate.eq.1

      if(output_rain.eq.1) then
          do j=1,nj
          do i=1,ni
            ds2(i,j)=rain(i,j,1)
          enddo
          enddo
          varname='sfcrain'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)
          if (nrain.eq.2) then
              do j=1,nj
              do i=1,ni
                ds2(i,j)=rain(i,j,2)
              enddo
              enddo
              varname='sfcrain_trans'
              call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)
          endif
      endif

      if(output_sws.eq.1) then
           do j=1,nj
           do i=1,ni
             ds2(i,j)=sws(i,j,1)
           enddo
           enddo
           varname='maxsws'
           call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

           do j=1,nj
           do i=1,ni
             ds2(i,j)=svs(i,j,1)
           enddo
           enddo
           varname='maxsvs'
           call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

           do j=1,nj
           do i=1,ni
             ds2(i,j)=sps(i,j,1)
           enddo
           enddo
           varname='maxsps'
           call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

           do j=1,nj
           do i=1,ni
             ds2(i,j)=srs(i,j,1)
           enddo
           enddo
           varname='maxsrs'
           call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

           do j=1,nj
           do i=1,ni
             ds2(i,j)=sgs(i,j,1)
           enddo
           enddo
           varname='maxsgs'
           call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

           do j=1,nj
           do i=1,ni
             ds2(i,j)=sus(i,j,1)
           enddo
           enddo
           varname='maxsus'
           call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

           do j=1,nj
           do i=1,ni
             ds2(i,j)=sus(i,j,1)
           enddo
           enddo
           varname='maxshs'
           call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

      endif !output_sws.eq.1

      if (nrain.eq.2) then
         if(output_rain.eq.1) then
            do j=1,nj
            do i=1,ni
              ds2(i,j)=rain(i,j,2)
            enddo
            enddo
            varname='maxrain_trans'
            call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)
          endif

          if(output_sws.eq.1) then
              do j=1,nj
              do i=1,ni
                ds2(i,j)=sws(i,j,2)
              enddo
              enddo
              varname='maxsws_trans'
              call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

              do j=1,nj
              do i=1,ni
                ds2(i,j)=svs(i,j,2)
              enddo
              enddo
              varname='maxsvs_trans'
              call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

              do j=1,nj
              do i=1,ni
                ds2(i,j)=sps(i,j,2)
              enddo
              enddo
              varname='maxsps_trans'
              call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

              do j=1,nj
              do i=1,ni
                ds2(i,j)=srs(i,j,2)
              enddo
              enddo
              varname='maxsrs_trans'
              call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

              do j=1,nj
              do i=1,ni
                ds2(i,j)=sgs(i,j,2)
              enddo
              enddo
              varname='maxsgs_trans'
              call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

              do j=1,nj
              do i=1,ni
                ds2(i,j)=sus(i,j,2)
              enddo
              enddo
              varname='maxsus_trans'
              call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

              do j=1,nj
              do i=1,ni
                ds2(i,j)=sus(i,j,2)
              enddo
              enddo
              varname='maxshs_trans'
              call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)
           endif !output_sws.eq.1
      endif !nrain.eq.2

! Composite reflectivity

      if(output_dbz.eq.1)then
          call calccref(dum1(ib,jb,1),dbz)
          do j=1,nj
          do i=1,ni
            ds2(i,j)=dum1(i,j,1)
          enddo
          enddo
          varname='cref'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)
      endif

! Cold pool C and H (RKW)

      if(output_coldpool.eq.1) then
          call calccpch(zf,th0,qv0,dum1(ib,jb,1),dum1(ib,jb,2),tha,qa)
          do j=1,nj
          do i=1,ni
            ds2(i,j)=dum1(i,j,1)
          enddo
          enddo
          varname='cpc'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=dum1(i,j,2)
          enddo
          enddo
          varname='cph'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)
      endif !output_coldpool.eq.1

! Surface fluxes

      if(output_sfcflx.eq.1) then
          do j=1,nj
          do i=1,ni
            ds2(i,j)=thflux(i,j)
          enddo
          enddo
          varname='thflux'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)
        
          do j=1,nj
          do i=1,ni
            ds2(i,j)=qvflux(i,j)
          enddo
          enddo

          varname='qvflux'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
            do i=1,ni
             ds2(i,j)=0.25*( (cdu(i,j)+cdu(i+1,j))   &
                           +(cdv(i,j)+cdv(i,j+1)) )
            enddo
          enddo

          varname='cd'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=ce(i,j)
          enddo
          enddo

          varname='ce'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=tsk(i,j)
          enddo
          enddo

          varname='tsk'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

      endif !output_sfcflx.eq.1

      if(output_sfcparams.eq.1) then

          do j=1,nj
          do i=1,ni
            ds2(i,j)=xland(i,j)
          enddo
          enddo

          varname='xland'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=lu_index(i,j)
          enddo
          enddo

          varname='lu'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=mavail(i,j)
          enddo
          enddo

          varname='mavail'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

      endif !output_sfcparams.eq.1

      if((output_sfcparams.eq.1).and.(sfcmodel.eq.2.or.oceanmodel.eq.2))then

          do j=1,nj
          do i=1,ni
            ds2(i,j)=tmn(i,j)
          enddo
          enddo
          varname='tmn'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=hfx(i,j)
          enddo
          enddo
          varname='hfx'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=qfx(i,j)
          enddo
          enddo
          varname='qfx'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=gsw(i,j)
          enddo
          enddo
          varname='gsw'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=glw(i,j)
          enddo
          enddo
          varname='glw'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)
      endif

      if((output_sfcparams.eq.1).and.(sfcmodel.eq.2))then

          do j=1,nj
          do i=1,ni
            ds2(i,j)=tslb(i,j,1)
          enddo
          enddo
          varname='tslb1'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=tslb(i,j,2)
          enddo
          enddo
          varname='tslb2'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=tslb(i,j,3)
          enddo
          enddo
          varname='tslb3'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=tslb(i,j,4)
          enddo
          enddo
          varname='tslb4'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=tslb(i,j,5)
          enddo
          enddo
          varname='tslb5'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)
      endif

      if(output_sfcparams.eq.1.and.oceanmodel.eq.2)then

          do j=1,nj
          do i=1,ni
            ds2(i,j)=tml(i,j)
          enddo
          enddo
          varname='tml'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=hml(i,j)
          enddo
          enddo
          varname='hml'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=huml(i,j)
          enddo
          enddo
          varname='huml'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=hvml(i,j)
          enddo
          enddo
          varname='hvml'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)
      endif

      if(output_radten.eq.1)then

          do j=1,nj
          do i=1,ni
            ds2(i,j)=radsw(i,j)
          enddo
          enddo
          varname='radsw'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=rnflx(i,j)
          enddo
          enddo
          varname='rnflx'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=radswnet(i,j)
          enddo
          enddo
          varname='radswnet'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=radlwin(i,j)
          enddo
          enddo
          varname='radlwin'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)
      endif

      if(output_sfcdiags.eq.1)then

          do j=1,nj
          do i=1,ni
            ds2(i,j)=u10(i,j)
          enddo
          enddo
          varname='ul0'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=v10(i,j)
          enddo
          enddo
          varname='vl0'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=t2(i,j)
          enddo
          enddo
          varname='t2'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=q2(i,j)
          enddo
          enddo
          varname='q2'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=znt(i,j)
          enddo
          enddo
          varname='znt'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=ust(i,j)
          enddo
          enddo
          varname='ust'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=hpbl(i,j)
          enddo
          enddo
          varname='hpbl'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=zol(i,j)
          enddo
          enddo
          varname='zol'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=mol(i,j)
          enddo
          enddo
          varname='mol'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

          do j=1,nj
          do i=1,ni
            ds2(i,j)=br(i,j)
          enddo
          enddo
          varname='br'
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)
      endif

! begin 3d group

      if(output_th  .eq.1)then
          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=th0(i,j,k)+tha(i,j,k)
          enddo
          enddo
          enddo
          varname='th'
          call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)
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
          call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)
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
          call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)
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
          call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)
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
          call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)
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
          call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)
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
          call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)
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
          call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)
      endif
      if(iturb.eq.1)then
          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=tkea(i,j,k)
          enddo
          enddo
          enddo
          varname="tke"
          call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)
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
              call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)        

              do k=1,nk
              do j=1,nj
              do i=1,ni
                ds(i,j,k)=kmv(i,j,k)
              enddo
              enddo
              enddo
              varname='kmv'
              call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)
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
           call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)

           do k=1,nk
           do j=1,nj
           do i=1,ni
             ds(i,j,k)=khv(i,j,k)
           enddo
           enddo
           enddo
           varname='khv'
           call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)
         endif
      endif !iturb .ge. 1

      if(imoist.eq.1)then
          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=qa(i,j,k,nqv)
          enddo
          enddo
          enddo
          varname='qv'
          if(output_qv .eq. 1)   call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)
          if(output_qvpert.eq.1)then
              do k=1,nk
              do j=1,nj
              do i=1,ni
                ds(i,j,k)=qa(i,j,k,nqv)-qv0(i,j,k)
              enddo
              enddo
              enddo
              varname="qvpert"
              call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)
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
                      call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)
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
              call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)
          endif

      endif !imoist.eq.1

      if(output_uinterp.eq.1)then
          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=0.5*(ua(i,j,k)+ua(i+1,j,k))
          enddo
          enddo
          enddo
          varname="uinterp"
          call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)
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
          call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)
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
          call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)
      endif


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
            call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)
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
          call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)

          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=qvpten(i,j,k)
          enddo
          enddo
          enddo

          varname='qvpten'
          call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)

          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=qcpten(i,j,k)
          enddo
          enddo
          enddo

          varname='qcpten'
          call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)

          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=qipten(i,j,k)
          enddo
          enddo
          enddo

          varname='qipten'
          call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)

          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=upten(i,j,k)
          enddo
          enddo
          enddo

          varname='upten'
          call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)

          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=vpten(i,j,k)
          enddo
          enddo
          enddo

          varname='vpten'
          call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)

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
          call damaris_write_2d_float(varname,int_time,ds2,x0,x1,y0,y1)

      endif!output_uh.eq.1

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
          call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)

          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=dum2(i,j,k)
          enddo
          enddo
          enddo

          varname='yvort'
          call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)

          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=dum3(i,j,k)
          enddo
          enddo
          enddo

          varname='zvort'
          call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)

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
          call damaris_write_3d_float(varname,int_time,ds,x0,x1,y0,y1,0,nk-1)

       endif


!--------------------------------------------------------------
!  writeout data on u-points

! Note velocity data is staggered and requires 1 extra point

       if(output_u    .eq.1) then
           do k=1,nk
           do j=1,nj
           do i=1,ni+1
             dumu(i,j,k)=ua(i,j,k)
           enddo
           enddo
           enddo
           varname='u'
           call damaris_write_3d_float(varname,int_time,dumu(1:ni+1,1:nj,1:nk),(myi-1)*(ni+1),myi*(ni+1)-1,y0,y1,0,nk-1)
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
          call damaris_write_3d_float(varname,int_time,dumu(1:ni+1,1:nj,1:nk),(myi-1)*(ni+1),myi*(ni+1)-1,y0,y1,0,nk-1)
      endif

!--------------------------------------------------------------
!  writeout data on v-points

      if(output_v    .eq.1) then
          do k=1,nk
          do j=1,nj+1
          do i=1,ni
            dumv(i,j,k)=va(i,j,k)
          enddo
          enddo
          enddo
          varname='v'
          call damaris_write_3d_float(varname,int_time,dumv(1:ni,1:nj+1,1:nk),x0,x1,(nj+1)*(myj-1),myj*(nj+1)-1,0,nk-1)
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
          call damaris_write_3d_float(varname,int_time,dumv(1:ni,1:nj+1,1:nk),x0,x1,(myj-1)*(nj+1),myj*(nj+1)-1,0,nk-1)
      endif

!--------------------------------------------------------------
!  writeout data on w-points

      if(output_w    .eq.1) then
          do k=1,nk+1
          do j=1,nj
          do i=1,ni
            dumw(i,j,k)=wa(i,j,k)
          enddo
          enddo
          enddo
          varname='w'
          call damaris_write_3d_float(varname,int_time,dumw(1:ni,1:nj,1:nk+1),x0,x1,y0,y1,0,nk)

      endif

!--------------------------------------------------------------

      write(outfile,*) 'Done Writing Data to File: ',trim(filename)
! NOTE if compiler says undefined symbol here, try flush_ (IBM)
!     call flush(outfile)

      call dc_poke("hdf5", int_time, ierror)
! close everything

#ifdef MPI
!     call MPI_BARRIER (MPI_COMM_WORLD,ierr)
#endif

      return
      end

!--------------------------------------------------------------------------------------------MARK

      subroutine damaris_write_3d_float(varname,step,fl_array_3d,numis,numie,numjs,numje,numks,numke)

      implicit none
      integer :: numis,numie,numjs,numje,numks,numke,ierr
      integer :: step
      character(LEN=20) :: varname
      real, dimension(numis:numie,numjs:numje,numks:numke) :: fl_array_3d
      integer*8 :: layout

      call dc_define_a3d_layout(layout,4,numis,numie,numjs,numje,numks,numke)
      call dc_write(varname,step,fl_array_3d,layout,ierr)      
      call dc_free_layout(layout)
      return
      end

!--------------------------------------------------------------------------------------------MARK
      subroutine damaris_write_float(varname,step,value)
      
      implicit none
      character(LEN=20) :: varname
      real value
      integer step
      integer ierr
      integer*8 :: layout
      
      call dc_define_scalar_layout(layout,4)
      call dc_write(varname,step,value,layout,ierr)
      call dc_free_layout(layout)

      return
      end
!--------------------------------------------------------------------------------------------MARK
      subroutine damaris_write_int(varname,step,value)
      
      implicit none
      character(LEN=20) :: varname
      integer value
      integer step
      integer ierr
      integer*8 :: layout

      call dc_define_scalar_layout(layout,2)
      call dc_write(varname,step,value,layout,ierr)
      call dc_free_layout(layout)
      return
      end
!--------------------------------------------------------------------------------------------MARK
      subroutine damaris_write_1d_float(varname,step,fl_array_1d,numks,numke)
      
      implicit none
      character(LEN=20) :: varname

      real value
      integer step
      integer numks,numke,ierr
      real, dimension(numks:numke) :: fl_array_1d
      integer*8 :: layout
      call dc_define_a1d_layout(layout,4,numks,numke)
      call dc_write(varname,step,fl_array_1d,layout,ierr)
      call dc_free_layout(layout)
      return
      end
!--------------------------------------------------------------------------------------------MARK

      subroutine damaris_write_2d_float(varname,step,fl_array_2d,numis,numie,numjs,numje)

      implicit none
      character(LEN=20) :: varname
      integer :: numis,numie,numjs,numje,ierr
      real, dimension(numis:numie,numjs:numje) :: fl_array_2d
      integer step
      integer*8 :: layout

      call dc_define_a2d_layout(layout,4,numis,numie,numjs,numje)
      call dc_write(varname,step,fl_array_2d,layout,ierr)
      call dc_free_layout(layout)
      return
      end
#endif
