c     cut on the transverse mass of W bosons
      do i=3,nexternal
        do j=i+1,nexternal
          if (is_a_lm(i) .or. is_a_lp(i) .or.
     &        is_a_lm(j) .or. is_a_lp(j)) then
            if (abs(ipdg_reco(i)+ipdg_reco(j)).eq.1) then
              xmtw=2d0*sqrt((p_reco(1,i)**2+p_reco(2,i)**2)*
     &                      (p_reco(1,j)**2+p_reco(2,j)**2))-
     &             2d0*(p_reco(1,i)*p_reco(1,j)+p_reco(2,i)*p_reco(2,j))
              if (xmtw.lt.(({})**2)) then
                passcuts_user=.false.
                return
              endif
            endif
          endif
        enddo
      enddo
