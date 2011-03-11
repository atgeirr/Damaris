    subroutine revthe(ttype,the,p,qt,t,qv)
    implicit none
    include 'constants.incl'
    integer ttype
    real the,p,qt,rh,t,qv

    ! Assumes air is saturated
    ! Input:   the,p,qt  (t is first guess value upon input)
    ! Output:  t,qv

    integer n
    real tlast,tinc,pd,cpm,lhv,thx,diff
    real rslf

    n=1
    tlast=t
    tinc=0.0
250 continue
      t=tlast+tinc
      qv=rslf(p,t)
    IF(ttype.eq.1)THEN
      pd=p/(1.0+reps*qv)
      cpm=cp+cpl*qt
      lhv=lv1-lv2*t
      thx=t*((p00/pd)**(rd/cpm))*exp(lhv*qv/(cpm*t))
    ELSEIF(ttype.eq.2)THEN
      thx=t*( (p00/p)**(0.2854*(1.0-0.28*qv)) )   &
           *exp( ((3376.0/t)-2.54)*qv*(1.0+0.81*qv) )
    ENDIF

      diff=the-thx
      if(n.ge.40) print *,n,p,the,thx
      if(abs(diff).gt.0.0001 .and. n.lt.50)then
        n=n+1
        tinc=0.30*diff
        tlast=t
        if(abs(tinc).ge.0.0001) go to 250
      elseif(n.ge.50)then
        print *,'n exceeded 50!'
        print *,'n=',n
        stop 1222
      endif

    return
    end subroutine revthe

!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

    real function getthp(p,t,qv)
    implicit none
    include 'constants.incl'
    real p,t,qv

    real :: tlcl

    tlcl=55.0+(2840./(3.5*alog(t)-    &
             alog(0.01*p*qv/(eps+qv))-4.805))
    getthp=t*( (p00/p)**(0.2854*(1.0-0.28*qv)) )   &
             *exp( ((3376.0/tlcl)-2.54)*qv*(1.0+0.81*qv) )

    return
    end function getthp

!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

    subroutine revthe2(the,p,rh,t,qv)
    implicit none
    include 'constants.incl'
    integer ttype
    real the,p,qt,rh,t,qv

    ! Input:   the,p,rh
    ! Output:  t,qv

    integer n
    real tlast,tinc,pd,cpm,lhv,thx,diff,tlcl
    real rslf

    n=1
    tlast=t
    tinc=0.0
250 continue
      t=tlast+tinc
      qv=rh*rslf(p,t)
      tlcl=55.0+(2840./(3.5*alog(t)-    &
               alog(0.01*p*qv/(eps+qv))-4.805))
      thx=t*( (p00/p)**(0.2854*(1.0-0.28*qv)) )   &
           *exp( ((3376.0/tlcl)-2.54)*qv*(1.0+0.81*qv) )

      diff=the-thx
!!!      if(n.ge.40) print *,n,p,the,thx
      if(abs(diff).gt.0.0001 .and. n.lt.50)then
        n=n+1
        tinc=0.30*diff
        tlast=t
        if(abs(tinc).ge.0.0001) go to 250
      elseif(n.ge.50)then
        print *,'n exceeded 50!'
        print *,'n=',n
        stop 1222
      endif

    return
    end subroutine revthe2

