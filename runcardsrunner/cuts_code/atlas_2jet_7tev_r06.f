c
      if ({}) then
        if (njet.lt.2) then
          passcuts_user=.false.
          return
        endif

        xystar = 0.5d0 * dabs(atanh(pjet(3,1)/pjet(0,1))-
     $                        atanh(pjet(3,2)/pjet(0,2)))
        xmjj = sqrt(invm2_04(pjet(0,1),pjet(0,2),1d0))

        if (xystar.lt.0.5d0) then
          if (xmjj.lt.260d0 .or. xmjj.gt.4270d0) then
            passcuts_user=.false.
            return
          endif
        else if (xystar.lt.1.0d0) then
          if (xmjj.lt.310d0 .or. xmjj.gt.4270d0) then
            passcuts_user=.false.
            return
          endif
        else if (xystar.lt.1.5d0) then
          if (xmjj.lt.510d0 .or. xmjj.gt.4640d0) then
            passcuts_user=.false.
            return
          endif
        else if (xystar.lt.2.0d0) then
          if (xmjj.lt.760d0 .or. xmjj.gt.4640d0) then
            passcuts_user=.false.
            return
          endif
        else if (xystar.lt.2.5d0) then
          if (xmjj.lt.1310d0 .or. xmjj.gt.5040d0) then
            passcuts_user=.false.
            return
          endif
        else if (xystar.lt.3.0d0) then
          if (xmjj.lt.2120d0 .or. xmjj.gt.5040d0) then
            passcuts_user=.false.
            return
          endif
        else
          passcuts_user=.false.
          return
        endif
      endif
