c     cut on Higgs particles
      do i=1,nexternal
        if (ipdg_reco(i) .eq. 25) then
          if (abs(atanh(p_reco(3,i)/p_reco(0,i)))
     &        .gt. {}) then
            passcuts_user=.false.
            return
          endif
        endif
      enddo
