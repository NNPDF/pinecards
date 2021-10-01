c
      if ({}) then
        if (njet.lt.2) then
          passcuts_user=.false.
          return
        endif

        xymax = max(dabs(atanh(pjet(3,1)/pjet(0,1))),
     $              dabs(atanh(pjet(3,2)/pjet(0,2))))
        xmjj = sqrt(invm2_04(pjet(0,1),pjet(0,2),1d0))

        if (xymax.lt.0.5d0) then
          if (xmjj.lt.197d0 .or. xmjj.gt.4010d0) then
            passcuts_user=.false.
            return
          endif
        else if (xymax.lt.1.0d0) then
          if (xmjj.lt.270d0 .or. xmjj.gt.4010d0) then
            passcuts_user=.false.
            return
          endif
        else if (xymax.lt.1.5d0) then
          if (xmjj.lt.419d0 .or. xmjj.gt.4509d0) then
            passcuts_user=.false.
            return
          endif
        else if (xymax.lt.2.0d0) then
          if (xmjj.lt.565d0 .or. xmjj.gt.5058d0) then
            passcuts_user=.false.
            return
          endif
        else if (xymax.lt.2.5d0) then
          if (xmjj.lt.1000d0 .or. xmjj.gt.5058d0) then
            passcuts_user=.false.
            return
          endif
        else
          passcuts_user=.false.
          return
        endif
      endif
