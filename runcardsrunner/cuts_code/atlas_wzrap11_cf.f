c
      if ({}) then
        do i = nincoming+1, nexternal
          if (iPDG_reco(i).eq.13) then
            ppl(0:4)=p_reco(0:4,i)
            xeta1=abs(eta_04(p_reco(0,i)))
          elseif (iPDG_reco(i).eq.-13) then
            pplb(0:4)=p_reco(0:4,i)
            xeta2=abs(eta_04(p_reco(0,i)))
          endif
        enddo

        xyll=abs(atanh((ppl(3)+pplb(3))/(ppl(0)+pplb(0))))

        if (xyll.lt.1.2d0 .or. xyll.gt.3.6d0) then
          passcuts_user=.false.
          return
        endif

        if (xeta1.lt.2.5d0) then
          if (xeta2.lt.2.5d0 .or. xeta2.gt.4.9d0) then
            passcuts_user=.false.
            return
          endif
        elseif (xeta2.lt.2.5d0) then
          if (xeta1.lt.2.5d0 .or. xeta1.gt.4.9d0) then
            passcuts_user=.false.
            return
          endif
        else
          passcuts_user=.false.
          return
        endif
      endif
