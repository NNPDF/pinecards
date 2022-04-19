c     cut on the rapidity of SFOS lepton pairs
      do i=1,nexternal-1
        if (is_a_lm(i) .or. is_a_lp(i)) then
          do j=i+1,nexternal
            if (ipdg_reco(i) .eq. -ipdg_reco(j)) then
              if (abs(atanh((p_reco(3,i)+p_reco(3,j))
     &            /(p_reco(0,i)+p_reco(0,j)))) .lt. {}) then
                passcuts_user=.false.
                return
              endif
            endif
          enddo
        endif
      enddo
