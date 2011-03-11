      ELSEIF(iwnd.eq.7)THEN

        ! user inputs:

!       shear_layer_01 = 0.010      ! shear (s^{-1}) in layer 1
!       shear_layer_02 = 0.004      ! shear (s^{-1}) in layer 2

!       depth_layer_01 = 1000.0     ! depth of layer 1 (m)
!       depth_layer_02 = 7000.0     ! depth of layer 2 (m)

!       angle =  0.0                ! angle of lower layer (degrees)
                                    !  -45 is SW-to-NE orientation
                                    !    0 is  N-to-S  orientation
                                    !  +45 is SE-to-NW orientation
!ORF
        shear_layer_01 = var1      ! shear (s^{-1}) in layer 1
        shear_layer_02 = var2      ! shear (s^{-1}) in layer 2

        depth_layer_01 = var3     ! depth of layer 1 (m)
        depth_layer_02 = var4     ! depth of layer 2 (m)

        angle =  var5                ! angle of lower layer (degrees)
                                    !  -45 is SW-to-NE orientation
                                    !    0 is  N-to-S  orientation
                                    !  +45 is SE-to-NW orientation

!---------

        ! no need to change anything below here:

        angle = angle*pi/180.0

        du = shear_layer_01 * depth_layer_01 * sin(angle)
        dv = shear_layer_01 * depth_layer_01 * cos(angle)

        do k=1,nk
        do j=1,nj
        do i=1,ni+1
          zu=0.5*(zh(i-1,j,k)+zh(i,j,k))
          if( zu.le.depth_layer_01 )then
!!!            u0(i,j,k) = 0.0
            u0(i,j,k) = du - (shear_layer_01 * zu)*sin(angle)
          elseif( (zu-depth_layer_01).le.depth_layer_02 )then
            u0(i,j,k) = 0.0 + shear_layer_02 * ( zu - depth_layer_01 )
          else
            u0(i,j,k) = 0.0 + shear_layer_02 * depth_layer_02
          endif
        enddo
        enddo
        enddo

        do k=1,nk
        do j=1,nj+1
        do i=1,ni
          zv=0.5*(zh(i,j-1,k)+zh(i,j,k))
          if( zv.le.depth_layer_01 )then
!!!            v0(i,j,k) = 0.0 + shear_layer_01 * zv
            v0(i,j,k) = (shear_layer_01 * zv)*cos(angle)    &
                      + ( shear_layer_01 * depth_layer_01 - dv )
          else
            v0(i,j,k) = 0.0 + shear_layer_01 * depth_layer_01
          endif
        enddo
        enddo
        enddo

!-----------------------------------------------------------------------

      ENDIF    ! endif for iwnd options

    ENDIF   ! endif for isnd=7 check
