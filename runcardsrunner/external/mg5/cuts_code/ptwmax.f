c     maximum transverse mass cut of W bosons
      do i=3,nexternal
        do j=i+1,nexternal
          if (abs(ipdg_reco(i)+ipdg_reco(j)).eq.1) then
            if (((p_reco(1,i)+p_reco(1,j))**2+
     &          (p_reco(2,i)+p_reco(2,j))**2) .gt. {}**2) then
                passcuts_user=.false.
                return
            endif
          endif
        enddo
      enddo
