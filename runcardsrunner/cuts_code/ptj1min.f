c     cut for ptl1min (leading jet)
      if (pt(pjet(0,1)) .lt. {}) then
        passcuts_user=.false.
        return
      endif
