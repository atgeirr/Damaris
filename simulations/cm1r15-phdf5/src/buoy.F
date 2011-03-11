    IF(imoist.eq.1)THEN

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
      do k=0,nk+1
      do j=0,nj+1
      do i=0,ni+1
        qt(i,j,k)=0.0
      enddo
      enddo
      enddo

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
      do k=0,nk+1
      do j=0,nj+1
      do i=0,ni+1
        qt(i,j,k)=qt(i,j,k)+qa(i,j,k,nqv)
      enddo
      enddo
      enddo

      do n=nql1,nql2
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=0,nk+1
        do j=0,nj+1
        do i=0,ni+1
          qt(i,j,k)=qt(i,j,k)+qa(i,j,k,n)
        enddo
        enddo
        enddo
      enddo

      do n=nqs1,nqs2
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=0,nk+1
        do j=0,nj+1
        do i=0,ni+1
          qt(i,j,k)=qt(i,j,k)+qa(i,j,k,n)
        enddo
        enddo
        enddo
      enddo

!$omp parallel do default(shared)  &
!$omp private(i,j,k,n,tx,cpm,qvs)
      do k=0,nk+1
      do j=0,nj+1
      do i=0,ni+1
        tx=(th0(i,j,k)+tha(i,j,k))*(pi0(i,j,k)+ppi(i,j,k))
        cpm=cp+cpl*qa(i,j,k,nqv)
        do n=nql1,nql2
          cpm=cpm+cpl*qa(i,j,k,n)
        enddo
        qvs=rslf( prs(i,j,k) , tx )
        the(i,j,k)=tx*((p00*(1.0+qa(i,j,k,nqv)*reps)      &
                        /prs(i,j,k))**(rd/cpm))       &
           *((qa(i,j,k,nqv)/qvs)**(-qa(i,j,k,nqv)*rv/cpm))    &
           *exp((lv1-lv2*tx)*qa(i,j,k,nqv)/(cpm*tx))
        thv(i,j,k)=(th0(i,j,k)+tha(i,j,k))*(1.0+reps*qa(i,j,k,nqv))/(1.0+qt(i,j,k))
        b(i,j,k)=g*( thv(i,j,k)/thv0(i,j,k) - 1.0 )
      enddo
      enddo
      enddo
