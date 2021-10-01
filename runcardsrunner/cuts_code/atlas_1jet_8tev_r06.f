c
      if ({}) then
        xjet=.false.

        do i=1,njet
          xyj = dabs(atanh(pjet(3,i)/pjet(0,i)))
          xptj = pt_04(pjet(0,i))

          if (xyj.lt.0.5d0) then
            if (xptj.le.2500d0) then
              xjet=.true.
c             exit
            endif
          else if (xyj.lt.1.0d0) then
            if (xptj.le.2500d0) then
              xjet=.true.
              exit
            endif
          else if (xyj.lt.1.5d0) then
            if (xptj.le.1992d0) then
              xjet=.true.
              exit
            endif
          else if (xyj.lt.2.0d0) then
            if (xptj.le.1310d0) then
              xjet=.true.
              exit
            endif
          else if (xyj.lt.2.5d0) then
            if (xptj.le.838d0) then
              xjet=.true.
              exit
            endif
          else if (xyj.lt.3.0d0) then
            if (xptj.le.556d0) then
              xjet=.true.
              exit
            endif
          endif
        enddo

        if (.not. xjet) then
          passcuts_user=.false.
          return
        endif
      endif
