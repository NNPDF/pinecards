c     cut for ptl1min (leading lepton)
      j = 0
      do i=1,nexternal
        if (is_a_lm(i) .or. is_a_lp(i)) then
          if (j.eq.0 .or. pt_04(p_reco(0,i)).ge.pt_04(p_reco(0,j))) then
            j = i
          endif
        endif
      enddo
      if (pt_04(p_reco(0,j)) .lt. {}) then
        passcuts_user=.false.
        return
      endif
