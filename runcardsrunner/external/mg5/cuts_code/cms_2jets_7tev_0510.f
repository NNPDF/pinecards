c
      if ({}) then
        block
          real*8 xymax, xmjj

          if (njet < 2) then
            passcuts_user = .false.
            return
          end if

          xymax = max(abs(atanh(pjet(3,1)/pjet(0,1))),
     $                abs(atanh(pjet(3,2)/pjet(0,2))))
          xmjj = sqrt(invm2_04(pjet(0,1),pjet(0,2),1d0))

          if (xymax < 0.5d0 .or. xymax > 1.0d0 .or. xmjj < 270d0 .or. xmjj > 4010d0) then
            passcuts_user=.false.
            return
          end if
        end block
      end if
