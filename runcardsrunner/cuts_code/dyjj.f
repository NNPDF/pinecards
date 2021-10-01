c     cut on the rapidity separation of the leading jets
      tmpvar=atanh(pjet(3,1)/pjet(0,1))
     &      -atanh(pjet(3,2)/pjet(0,2))
      if (abs(tmpvar) .lt. {}) then
        passcuts_user=.false.
        return
      endif
