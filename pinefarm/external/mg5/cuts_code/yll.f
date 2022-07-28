c     cut on the rapidity of the two leading leptons
      block
        integer :: mm, tmpvar

        j = 0 ! leading lepton index
        mm = 0 ! subleading lepton index
        do i=1,nexternal
          if (is_a_lm(i) .or. is_a_lp(i)) then
            tmpvar = pt_04(p_reco(0,i))
            if (j.eq.0) then
              j = i
            else if (mm.eq.0) then
              if (tmpvar.ge.pt_04(p_reco(0,j))) then
                mm = j
                j = i
              else if (tmpvar.ge.pt_04(p_reco(0,mm))) then
                mm = i
              endif
            else if (tmpvar.ge.pt_04(p_reco(0,j))) then
              mm = j
              j = i
            else if (tmpvar.ge.pt_04(p_reco(0,mm))) then
              mm = i
            endif
          endif
        enddo
        if (abs(atanh((p_reco(3,j)+p_reco(3,mm))
     &      /(p_reco(0,j)+p_reco(0,mm)))) .gt. {}) then
          passcuts_user=.false.
          return
        endif
      end block
