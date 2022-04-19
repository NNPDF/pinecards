c     cut on the invariant mass of the leading jets
      if (invm2_04(pjet(0,1),pjet(0,2),1d0) .lt. ({})**2) then
        passcuts_user=.false.
        return
      endif
