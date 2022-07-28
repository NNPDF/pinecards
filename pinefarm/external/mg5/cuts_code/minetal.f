c     cut on the minimum pseudorapidity of leptons
      do i=1,nexternal-1
        if (is_a_lm(i) .or. is_a_lp(i)) then
          if (abs(atanh(p_reco(3,i)/sqrt(p_reco(1,i)**2+p_reco(2,i)**2+
     &                  p_reco(3,i)**2))) .lt. {}) then
            passcuts_user=.false.
            return
          endif
        endif
      enddo
