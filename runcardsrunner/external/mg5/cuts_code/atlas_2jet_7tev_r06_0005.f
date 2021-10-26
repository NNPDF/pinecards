c
      if ({}) then
        if (njet < 2) then
          passcuts_user=.false.
          return
        endif

        block
          real*8 xystar, xmjj

          xystar = 0.5d0 * abs(atanh(pjet(3,1)/pjet(0,1))-
     $                         atanh(pjet(3,2)/pjet(0,2)))
          xmjj = sqrt(invm2_04(pjet(0,1),pjet(0,2),1d0))

          if (xystar > 0.5d0 .or. xmjj < 260d0 .or. xmjj > 4270d0) then
            passcuts_user=.false.
            return
          endif
        end block
      endif
