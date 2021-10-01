c     cut on the pt of SFOS lepton pairs
      do i=1,nexternal-1
        if (is_a_lm(i) .or. is_a_lp(i)) then
          do j=i+1,nexternal
            if (ipdg_reco(i) .eq. -ipdg_reco(j)) then
              if (((p_reco(1,i)+p_reco(1,j))**2+
     &            (p_reco(2,i)+p_reco(2,j))**2) .lt. {}**2) then
                passcuts_user=.false.
                return
              endif
            endif
          enddo
        endif
      enddo
