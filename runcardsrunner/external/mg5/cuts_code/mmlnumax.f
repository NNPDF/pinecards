c     cut for mmlnumax (SFOS lepton pairs)
      do i=1,nexternal-1
        if (is_a_lm(i) .or. is_a_lp(i)) then
          do j=1,nexternal
          if (abs(ipdg_reco(i) + ipdg_reco(j)) .eq. 1) then
              if (invm2_04(p_reco(0,i),p_reco(0,j),1d0) .gt. {}**2) then
                passcuts_user=.false.
                return
              endif
            endif
          enddo
        endif
      enddo
