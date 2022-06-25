c     maximum transverse momentum for all charged leptons
      do i=3,nexternal
        if (abs(ipdg_reco(i)) == 12 .or.
     &      abs(ipdg_reco(i)) == 14 .or.
     &      abs(ipdg_reco(i)) == 16) then
          if (p_reco(1,i)**2+p_reco(2,i)**2 > (({})**2)) then
            passcuts_user=.false.
            return
          endif
        endif
      enddo
