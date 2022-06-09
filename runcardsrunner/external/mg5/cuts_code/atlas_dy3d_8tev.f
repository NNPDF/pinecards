c
      if ({}) then
        do j = nincoming+1, nexternal
          if (iPDG_reco(j).eq.13) ppl(0:3)=p_reco(0:3,j)
          if (iPDG_reco(j).eq.-13) pplb(0:3)=p_reco(0:3,j)
        enddo

        p1p = ppl(0) + ppl(3)
        p1m = ppl(0) - ppl(3)
        p2p = pplb(0) + pplb(3)
        p2m = pplb(0) - pplb(3)
        pzll = ppl(3) + pplb(3)
        pt2ll = (ppl(1) + pplb(1)) * (ppl(1) + pplb(1)) +
     &          (ppl(2) + pplb(2)) * (ppl(2) + pplb(2))

        xmll=sqrt((ppl(0)+pplb(0))**2-(ppl(1)+pplb(1))**2-
     &            (ppl(2)+pplb(2))**2-(ppl(3)+pplb(3))**2)
        xyll=abs(atanh((ppl(3)+pplb(3))/(ppl(0)+pplb(0))))
        xcos = (p1p*p2m-p1m*p2p)/sqrt(xmll*xmll+pt2ll)/xmll*
     &         sign(1d0,pzll)

        if (xmll.lt.46d0) then
          passcuts_user=.false.
          return
        elseif (xmll.lt.66d0) then
          if (xcos.lt.-0.7d0) then
            xlimit=2.0d0
          elseif (xcos.lt.-0.4d0) then
            xlimit=2.2d0
          elseif (xcos.lt.0.0d0) then
            xlimit=2.4d0
          elseif (xcos.lt.0.4d0) then
            xlimit=2.4d0
          elseif (xcos.lt.0.7d0) then
            xlimit=2.2d0
          else
            xlimit=2.0d0
          endif
        elseif (xmll.lt.80d0) then
          if (xcos.lt.-0.7d0) then
            xlimit=2.0d0
          elseif (xcos.lt.-0.4d0) then
            xlimit=2.2d0
          elseif (xcos.lt.0.0d0) then
            xlimit=2.4d0
          elseif (xcos.lt.0.4d0) then
            xlimit=2.4d0
          elseif (xcos.lt.0.7d0) then
            xlimit=2.2d0
          else
            xlimit=2.0d0
          endif
        elseif (xmll.lt.91d0) then
          if (xcos.lt.-0.7d0) then
            xlimit=2.2d0
          elseif (xcos.lt.-0.4d0) then
            xlimit=2.2d0
          elseif (xcos.lt.0.0d0) then
            xlimit=2.4d0
          elseif (xcos.lt.0.4d0) then
            xlimit=2.4d0
          elseif (xcos.lt.0.7d0) then
            xlimit=2.2d0
          else
            xlimit=2.2d0
          endif
        elseif (xmll.lt.102d0) then
          if (xcos.lt.-0.7d0) then
            xlimit=2.2d0
          elseif (xcos.lt.-0.4d0) then
            xlimit=2.2d0
          elseif (xcos.lt.0.0d0) then
            xlimit=2.4d0
          elseif (xcos.lt.0.4d0) then
            xlimit=2.4d0
          elseif (xcos.lt.0.7d0) then
            xlimit=2.2d0
          else
            xlimit=2.2d0
          endif
        elseif (xmll.lt.116d0) then
          if (xcos.lt.-0.7d0) then
            xlimit=2.0d0
          elseif (xcos.lt.-0.4d0) then
            xlimit=2.2d0
          elseif (xcos.lt.0.0d0) then
            xlimit=2.4d0
          elseif (xcos.lt.0.4d0) then
            xlimit=2.4d0
          elseif (xcos.lt.0.7d0) then
            xlimit=2.2d0
          else
            xlimit=2.0d0
          endif
        elseif (xmll.lt.150d0) then
          if (xcos.lt.-0.7d0) then
            xlimit=1.8d0
          elseif (xcos.lt.-0.4d0) then
            xlimit=2.2d0
          elseif (xcos.lt.0.0d0) then
            xlimit=2.4d0
          elseif (xcos.lt.0.4d0) then
            xlimit=2.4d0
          elseif (xcos.lt.0.7d0) then
            xlimit=2.2d0
          else
            xlimit=1.8d0
          endif
        elseif (xmll.lt.200d0) then
          if (xcos.lt.-0.7d0) then
            xlimit=1.6d0
          elseif (xcos.lt.-0.4d0) then
            xlimit=2.0d0
          elseif (xcos.lt.0.0d0) then
            xlimit=2.4d0
          elseif (xcos.lt.0.4d0) then
            xlimit=2.4d0
          elseif (xcos.lt.0.7d0) then
            xlimit=2.0d0
          else
            xlimit=1.6d0
          endif
        else
          passcuts_user=.false.
          return
        endif

        if (xyll.gt.xlimit) then
          passcuts_user=.false.
          return
        endif
      endif
