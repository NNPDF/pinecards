c     cut on the sum of all missing transverse momentum
      xptmiss=0d0

      do i=3,nexternal
        if (abs(ipdg_reco(i)).eq.12 .or.
     &      abs(ipdg_reco(i)).eq.14 .or.
     &      abs(ipdg_reco(i)).eq.16) then
          xptmiss(1)=xptmiss(1)+p_reco(1,i)
          xptmiss(2)=xptmiss(2)+p_reco(2,i)
        endif
      enddo

      if (xptmiss(1)**2+xptmiss(2)**2.lt.(({})**2)) then
        passcuts_user=.false.
        return
      endif
