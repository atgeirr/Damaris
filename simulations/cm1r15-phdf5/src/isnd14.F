     ELSEIF(isnd.eq.14)THEN

        p_sfc  = 96000.0     ! surface pressure (Pa)
        th_sfc = 303.00      ! surface potential temperature (K)
        zmix   =  1000.0     ! depth (m) of nearly mixed layer
        ztrop  = 11000.0     ! height (m) above mixed layer of tropopause
        zmin   = 3000.0      ! height (m) above mixed layer where theta-e is
                             ! a minimum
        dtheta = -25.0       ! change in theta-e (K) from zmix to zmin
        rhexp  =  0.75       ! exponent in specification of RH profile

        rh_sfc = 0.70        ! rh at sfc
        rh_pbl = 0.90        ! rh at top of pbl

        ! assuming no terrain (for now)
        i=1
        j=1

        write(outfile,*)
        do k=1,nk
          if(zh(i,j,k).lt.zmix)then
            thep(k) = 0.0
          elseif(zh(i,j,k).lt.(zmix+zmin))then
            thep(k) = dtheta*sin( 0.5*pi*(zh(i,j,k)-zmix)/(zmin) )
          elseif(zh(i,j,k).lt.(zmix+ztrop))then
            thep(k) = dtheta*sin( 0.5*pi*(zh(i,j,k)-((ztrop+zmix)-2.0*(ztrop-zmin)))/(ztrop-zmin) )
          else
            thep(k) = 0.0
          endif
          write(outfile,*) '  zh,thep = ',k,zh(i,j,k),thep(k)
        enddo
        write(outfile,*)

        pi_sfc = (p_sfc*rp00)**rovcp
        t_sfc = th_sfc * pi_sfc
        qv_sfc = rh_sfc*rslf(p_sfc,t_sfc)
        thv_sfc = th_sfc*(1.0+reps*qv_sfc)/(1.0+qv_sfc)
        the_sfc = getthp(p_sfc,th_sfc*pi_sfc,qv_sfc)

        write(outfile,*)

        do k=1,nk
          the0(k)=the_sfc+thep(k)
          th0(i,j,k)=the0(k)
          ! reasonable first-guess values:
          t0(i,j,k)=200.0
          qv0(i,j,k)=0.001
          write(outfile,*) '  zh,the0 = ',k,zh(i,j,k),the0(k)
        enddo

        thv_trop = thv_sfc
        t_trop = thv_trop
        qv_trop = 0.0

        ! RH profile is set here:
        write(outfile,*)
        do k=1,nk
          if(zh(i,j,k).le.zmix)then
            rh0(i,j,k)=rh_sfc-(rh_sfc-rh_pbl)*zh(i,j,k)/zmix
          elseif(zh(i,j,k).le.zmix+ztrop)then
            rh0(i,j,k)=rh_pbl-(rh_pbl-0.20)*(( (zh(i,j,k)-zmix)/ztrop)**rhexp)
          else
            rh0(i,j,k)=0.20
          endif
          write(outfile,*) '  zh,rh0 = ',zh(i,j,k),rh0(i,j,k)
        enddo

        write(outfile,*)
        do n=1,20
          do k=1,nk
            if(zh(i,j,k).lt.zmix)then
!!!              thv0(i,j,k)=thv_sfc*exp(nm*zh(i,j,k)/g)
              thv0(i,j,k)=th0(i,j,k)*(1.0+reps*qv0(i,j,k))/(1.0+qv0(i,j,k))
            elseif(zh(i,j,k).lt.ztrop+zmix)then
              thv0(i,j,k)=th0(i,j,k)*(1.0+reps*qv0(i,j,k))/(1.0+qv0(i,j,k))
            else
              ! this sets stability in stratosphere:
              thv0(i,j,k)=thv_trop*exp(4.0e-4*(zh(i,j,k)-(ztrop+zmix))/g)
            endif
          enddo
          pi0(i,j,1)=pi_sfc-g*zh(i,j,1)/(cp*0.5*(thv_sfc+thv0(i,j,1)))
          do k=2,nk
            pi0(i,j,k)=pi0(i,j,k-1)-g*(zh(i,j,k)-zh(i,j,k-1))/(cp*0.5*(thv0(i,j,k-1)+thv0(i,j,k)))
          enddo
          do k=1,nk
            prs0(i,j,k)=p00*(pi0(i,j,k)**(cp/rd))
          enddo
          do k=1,nk
            if(zh(i,j,k).lt.zmix)then
!              th0(i,j,k)=thv0(i,j,k)*(1.0+qv0(i,j,k))/(1.0+reps*qv0(i,j,k))
 !             t0(i,j,k)=th0(i,j,k)*pi0(i,j,k)
 !             qv0(i,j,k)=qv_sfc+(qv_mix-qv_sfc)*zh(i,j,k)/zmix
 !             ! use the following line for constant rh in PBL:
 !             qv0(i,j,k)=0.80*rslf(prs0(i,j,k),t0(i,j,k))
 !             the0(k) = getthp(prs0(i,j,k),t0(i,j,k),qv0(i,j,k))
              the0(k) = the_sfc
              call revthe2(the0(k),prs0(i,j,k),rh0(i,j,k),t0(i,j,k),qv0(i,j,k))
              th0(i,j,k)=t0(i,j,k)/pi0(i,j,k)
            elseif(zh(i,j,k).lt.ztrop+zmix)then
              call revthe2(the0(k),prs0(i,j,k),rh0(i,j,k),t0(i,j,k),qv0(i,j,k))
              th0(i,j,k)=t0(i,j,k)/pi0(i,j,k)
              if(zh(i,j,k+1).gt.ztrop+zmix)then
                pi_trop=pi0(i,j,k)-g*(ztrop-zh(i,j,k))/(cp*0.5*(thv0(i,j,k)+thv_trop))
                p_trop=p00*(pi_trop**(cp/rd))
                call revthe2(the_sfc,p_trop,0.25,t_trop,qv_trop)
                th_trop=t_trop/pi_trop
                thv_trop=th_trop*(1.0+reps*qv_trop)/(1.0+qv_trop)
              endif
            else
              th0(i,j,k)=thv0(i,j,k)*(1.0+qv0(i,j,k))/(1.0+reps*qv0(i,j,k))
              t0(i,j,k)=th0(i,j,k)*pi0(i,j,k)
              qv0(i,j,k)=rh0(i,j,k)*rslf(prs0(i,j,k),t0(i,j,k))
              the0(k) = getthp(prs0(i,j,k),t0(i,j,k),qv0(i,j,k))
            endif
          enddo 
          write(outfile,*) n,prs0(i,j,1),thv_trop
        enddo
        write(outfile,*)

        do k=1,nk
          rh0(i,j,k)=qv0(i,j,k)/(rslf(prs0(i,j,k),th0(i,j,k)*pi0(i,j,k)))
        enddo

        ! assuming no terrain (for now):
        do k=1,nk
        do j=jb,je
        do i=ib,ie
           pi0(i,j,k) =  pi0(1,1,k)
          prs0(i,j,k) = prs0(1,1,k)
           th0(i,j,k) =  th0(1,1,k)
           qv0(i,j,k) =  qv0(1,1,k)
           rh0(i,j,k) =  rh0(1,1,k)
        enddo
        enddo
        enddo

!------------------------------------------------------------------
