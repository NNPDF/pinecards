c
      if ({}) then
        if (njet.lt.2) then
          passcuts_user=.false.
          return
        endif

        xystar = 0.5d0 * dabs(atanh(pjet(3,1)/pjet(0,1))-
     $                        atanh(pjet(3,2)/pjet(0,2)))
        xyboost = 0.5d0 * dabs(atanh(pjet(3,1)/pjet(0,1))+
     $                        atanh(pjet(3,2)/pjet(0,2)))
        xptavg = 0.5d0 * (pt_04(pjet(0,1)) + pt_04(pjet(0,2)))

        if (xptavg.lt.133d0) then
          passcuts_user=.false.
          return
        endif

        if (xyboost.lt.1d0) then
          if (xystar.lt.1d0) then
            if (xptavg.gt.1784d0) then
              passcuts_user=.false.
              return
            endif
          elseif (xystar.lt.2d0) then
            if (xptavg.gt.1248d0) then
              passcuts_user=.false.
              return
            endif
          elseif (xystar.lt.3d0) then
            if (xptavg.gt.548d0) then
              passcuts_user=.false.
              return
            endif
          else
            passcuts_user=.false.
            return
          endif
        else if (xyboost.lt.2d0) then
          if (xystar.lt.1d0) then
            if (xptavg.gt.1032d0) then
              passcuts_user=.false.
              return
            endif
          elseif (xystar.lt.2d0) then
            if (xptavg.gt.686d0) then
              passcuts_user=.false.
              return
            endif
          else
            passcuts_user=.false.
            return
          endif
        else if (xyboost.lt.3d0) then
          if (xystar.lt.1d0) then
            if (xptavg.gt.430d0) then
              passcuts_user=.false.
              return
            endif
          else
            passcuts_user=.false.
            return
          endif
        else
          passcuts_user=.false.
          return
        endif
      endif
