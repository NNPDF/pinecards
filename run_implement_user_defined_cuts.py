#!/usr/bin/env python3

import os.path
import sys

cuts_variables = {
    'abscoscs': '''c     variables for abscoscs cuts
      real*8 zcoscs,zenl,zptxl,zptyl,zpzl,zenal,zptxal,zptyal,zpzal
      real*8 zpzll,zmll,zp1p,zp1m,zp2p,zp2m,zpt2ll
      integer zlep,zalep
''',
    'atlas_zpt_8tev_mdist': '''c     variables for atlas_zpt_8tev_mdist cut
      double precision ppl(0:3),pplb(0:3),ppv(0:3),ptv,xmll
''',
    'atlas_dy3d_8tev': '''c variables for atlas_dy3d_8tev cut
      real*8 ppl(0:3),pplb(0:3)
      real*8 p1p,p1m,p2p,p2m,pzll,pt2ll
      real*8 xmll,xyll,xcos,xlimit
''',
    'atlas_wzrap11_cf': '''c variables for atlas_wzrap11_cf cut
      real*8 ppl(0:4),pplb(0:4)
      real*8 xyll,xeta1,xeta2
''',
    'atlas_2jet_7tev_r06': '''c
      real*8 xystar,xmjj
''',
    'cms_2jets_7tev': '''c     variables for cms_2jets_7tev cuts
      real*8 xymax,xmjj
''',
    'atlas_1jet_8tev_r06': '''c     variables for atlas_1jet_8tev_r06 cuts
      real*8 xptj,xyj
      logical xjet
''',
    'cms_2jet_3d_8tev': '''c
      real*8 xystar,xyboost,xptavg
''',
    'ptmiss': '''c variables for ptmiss cut
      real*8 xptmiss(1:2)
''',
    'mtw': '''c variables for ptmiss cut
      real*8 xmtw
''',
}

cuts_code = {
    'mjj': '''c     cut on the invariant mass of the leading jets
      if (invm2_04(pjet(0,1),pjet(0,2),1d0) .lt. ({})**2) then
        passcuts_user=.false.
        return
      endif

''',
    'dyjj': '''c     cut on the rapidity separation of the leading jets
      tmpvar=atanh(pjet(3,1)/pjet(0,1))
     &      -atanh(pjet(3,2)/pjet(0,2))
      if (abs(tmpvar) .lt. {}) then
        passcuts_user=.false.
        return
      endif

''',
    'mmllmax': '''c     cut for mmllmax (SFOS lepton pairs)
      do i=1,nexternal-1
        if (is_a_lm(i) .or. is_a_lp(i)) then
          do j=i+1,nexternal
            if (ipdg_reco(i) .eq. -ipdg_reco(j)) then
              if (invm2_04(p_reco(0,i),p_reco(0,j),1d0) .gt. {}**2) then
                passcuts_user=.false.
                return
              endif
            endif
          enddo
        endif
      enddo

''',
    'ptl1min': '''c     cut for ptl1min (leading lepton)
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

''',
    'ptj1min': '''c     cut for ptl1min (leading jet)
      if (pt(pjet(0,1)) .lt. {}) then
        passcuts_user=.false.
        return
      endif

''',
    'yll': '''c     cut on the rapidity of the two leading leptons
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
     &    /(p_reco(0,j)+p_reco(0,mm)))) .gt. {}) then
        passcuts_user=.false.
        return
      endif

''',
    'ptzmin': '''c     cut on the pt of SFOS lepton pairs
      do i=1,nexternal-1
        if (is_a_lm(i) .or. is_a_lp(i)) then
          do j=i+1,nexternal
            if (ipdg_reco(i) .eq. -ipdg_reco(j)) then
              if (((p_reco(1,i)+p_reco(1,j))**2+
     &            (p_reco(2,i)+p_reco(2,j))**2) .lt. {}**2) then
                passcuts_user=.false.
                return
              endif
            endif
          enddo
        endif
      enddo

''',
    'ptzmax': '''c     cut on the pt of SFOS lepton pairs
      do i=1,nexternal-1
        if (is_a_lm(i) .or. is_a_lp(i)) then
          do j=i+1,nexternal
            if (ipdg_reco(i) .eq. -ipdg_reco(j)) then
              if (((p_reco(1,i)+p_reco(1,j))**2+
     &            (p_reco(2,i)+p_reco(2,j))**2) .gt. {}**2) then
                passcuts_user=.false.
                return
              endif
            endif
          enddo
        endif
      enddo

''',
    'yz': '''c     cut on the rapidity of SFOS lepton pairs
      do i=1,nexternal-1
        if (is_a_lm(i) .or. is_a_lp(i)) then
          do j=i+1,nexternal
            if (ipdg_reco(i) .eq. -ipdg_reco(j)) then
              if (abs(atanh((p_reco(3,i)+p_reco(3,j))
     &            /(p_reco(0,i)+p_reco(0,j)))) .gt. {}) then
                passcuts_user=.false.
                return
              endif
            endif
          enddo
        endif
      enddo

''',
    'yh': '''c     cut on Higgs particles
      do i=1,nexternal
        if (ipdg_reco(i) .eq. 25) then
          if (abs(atanh(p_reco(3,i)/p_reco(0,i)))
     &        .gt. {}) then
            passcuts_user=.false.
            return
          endif
        endif
      enddo

''',
    'yt': '''c     cut on top particles
      do i=1,nexternal
        if (ipdg_reco(i).eq.6 .or. ipdg_reco(i).eq.-6) then
          if (abs(atanh(p_reco(3,i)/p_reco(0,i)))
     &        .gt. {}) then
            passcuts_user=.false.
            return
          endif
        endif
      enddo

''',
    'abscoscsmin': '''c     cut on the minimum of the absolute value of the cosine of the Collins-Soper angle of SFOS pairs
      do i=1,nexternal-1
        if (is_a_lm(i) .or. is_a_lp(i)) then
          do j=i+1,nexternal
            if (ipdg_reco(i) .eq. -ipdg_reco(j)) then
              if (is_a_lm(i)) then
                zlep=i
                zalep=j
              else
                zlep=j
                zalep=i
              endif
              zenl=p_reco(0,zlep)
              zptxl=p_reco(1,zlep)
              zptyl=p_reco(2,zlep)
              zpzl=p_reco(3,zlep)
              zenal=p_reco(0,zalep)
              zptxal=p_reco(1,zalep)
              zptyal=p_reco(2,zalep)
              zpzal=p_reco(3,zalep)
c             implementation of first formula on page 6 of https://arxiv.org/abs/1710.05167
              zp1p=zenl+zpzl
              zp1m=zenl-zpzl
              zp2p=zenal+zpzal
              zp2m=zenal-zpzal
              zpzll=zpzl+zpzal
              zpt2ll=(zptxl+zptxal)*(zptxl+zptxal)+
     &               (zptyl+zptyal)*(zptyl+zptyal)
              zmll=sqrt((zenl+zenal)*(zenl+zenal)-(zpt2ll+zpzll*zpzll))
              zcoscs=sign((zp1p*zp2m-zp1m*zp2p)/
     &                    sqrt(zmll*zmll+zpt2ll)/zmll,zpzll)

              if (abs(zcoscs) .lt. {}) then
                passcuts_user=.false.
                return
              endif
            endif
          enddo
        endif
      enddo

''',
    'minetal': '''c     cut on the minimum pseudorapidity of leptons
      do i=1,nexternal-1
        if (is_a_lm(i) .or. is_a_lp(i)) then
          if (abs(atanh(p_reco(3,i)/sqrt(p_reco(1,i)**2+p_reco(2,i)**2+
     &                  p_reco(3,i)**2))) .lt. {}) then
            passcuts_user=.false.
            return
          endif
        endif
      enddo

''',
    'abscoscsmax': '''c     cut on the maximum of the absolute value of the cosine of the Collins-Soper angle of SFOS pairs
      do i=1,nexternal-1
        if (is_a_lm(i) .or. is_a_lp(i)) then
          do j=i+1,nexternal
            if (ipdg_reco(i) .eq. -ipdg_reco(j)) then
              if (is_a_lm(i)) then
                zlep=i
                zalep=j
              else
                zlep=j
                zalep=i
              endif
              zenl=p_reco(0,zlep)
              zptxl=p_reco(1,zlep)
              zptyl=p_reco(2,zlep)
              zpzl=p_reco(3,zlep)
              zenal=p_reco(0,zalep)
              zptxal=p_reco(1,zalep)
              zptyal=p_reco(2,zalep)
              zpzal=p_reco(3,zalep)
c             implementation of first formula on page 6 of https://arxiv.org/abs/1710.05167
              zp1p=zenl+zpzl
              zp1m=zenl-zpzl
              zp2p=zenal+zpzal
              zp2m=zenal-zpzal
              zpzll=zpzl+zpzal
              zpt2ll=(zptxl+zptxal)*(zptxl+zptxal)+
     &               (zptyl+zptyal)*(zptyl+zptyal)
              zmll=sqrt((zenl+zenal)*(zenl+zenal)-(zpt2ll+zpzll*zpzll))
              zcoscs=sign((zp1p*zp2m-zp1m*zp2p)/
     &                    sqrt(zmll*zmll+zpt2ll)/zmll,zpzll)

              if (abs(zcoscs) .gt. {}) then
                passcuts_user=.false.
                return
              endif
            endif
          enddo
        endif
      enddo

''',
    'atlas_zpt_8tev_mdist': '''c
      if ({}) then
        do i = nincoming+1, nexternal
          if (iPDG_reco(i).eq.13) ppl(0:3)=p_reco(0:3,i)
          if (iPDG_reco(i).eq.-13) pplb(0:3)=p_reco(0:3,i)
        enddo
        do i=0,3
          ppv(i)=ppl(i)+pplb(i)
        enddo

        xmll=sqrt(ppv(0)**2-ppv(1)**2-ppv(2)**2-ppv(3)**2)
        ptv=sqrt(ppv(1)**2+ppv(2)**2)

        if (ptv.gt.900d0) then
          passcuts_user=.false.
          return
        endif

        if (xmll.lt.12d0) then
          passcuts_user=.false.
          return
        elseif (xmll.lt.20d0) then
          if (ptv.lt.45d0) then
            passcuts_user=.false.
            return
          endif
        elseif (xmll.lt.30d0) then
          if (ptv.lt.45d0) then
            passcuts_user=.false.
            return
          endif
        elseif (xmll.lt.46d0) then
          if (ptv.lt.45d0) then
            passcuts_user=.false.
            return
          endif
        elseif (xmll.lt.66d0) then
          if (ptv.lt.30d0) then
            passcuts_user=.false.
            return
          endif
        elseif (xmll.lt.116d0) then
          if (ptv.lt.27.5d0) then
            passcuts_user=.false.
            return
          endif
        elseif (xmll.lt.150d0) then
          if (ptv.lt.30d0) then
            passcuts_user=.false.
            return
          endif
        else
          passcuts_user=.false.
          return
        endif
      endif

''',
    'yzmin': '''c     cut on the rapidity of SFOS lepton pairs
      do i=1,nexternal-1
        if (is_a_lm(i) .or. is_a_lp(i)) then
          do j=i+1,nexternal
            if (ipdg_reco(i) .eq. -ipdg_reco(j)) then
              if (abs(atanh((p_reco(3,i)+p_reco(3,j))
     &            /(p_reco(0,i)+p_reco(0,j)))) .lt. {}) then
                passcuts_user=.false.
                return
              endif
            endif
          enddo
        endif
      enddo

''',
    'atlas_dy3d_8tev': '''c
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
        xcos = sign((p1p*p2m-p1m*p2p)/sqrt(xmll*xmll+pt2ll)/xmll,pzll)

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
''',
    'atlas_wzrap11_cf': '''c
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
''',
    'atlas_2jet_7tev_r06': '''c
      if ({}) then
        if (njet.lt.2) then
          passcuts_user=.false.
          return
        endif

        xystar = 0.5d0 * dabs(atanh(pjet(3,1)/pjet(0,1))-
     $                        atanh(pjet(3,2)/pjet(0,2)))
        xmjj = sqrt(invm2_04(pjet(0,1),pjet(0,2),1d0))

        if (xystar.lt.0.5d0) then
          if (xmjj.lt.260d0 .or. xmjj.gt.4270d0) then
            passcuts_user=.false.
            return
          endif
        else if (xystar.lt.1.0d0) then
          if (xmjj.lt.310d0 .or. xmjj.gt.4270d0) then
            passcuts_user=.false.
            return
          endif
        else if (xystar.lt.1.5d0) then
          if (xmjj.lt.510d0 .or. xmjj.gt.4640d0) then
            passcuts_user=.false.
            return
          endif
        else if (xystar.lt.2.0d0) then
          if (xmjj.lt.760d0 .or. xmjj.gt.4640d0) then
            passcuts_user=.false.
            return
          endif
        else if (xystar.lt.2.5d0) then
          if (xmjj.lt.1310d0 .or. xmjj.gt.5040d0) then
            passcuts_user=.false.
            return
          endif
        else if (xystar.lt.3.0d0) then
          if (xmjj.lt.2120d0 .or. xmjj.gt.5040d0) then
            passcuts_user=.false.
            return
          endif
        else
          passcuts_user=.false.
          return
        endif
      endif

''',
    'atlas_1jet_8tev_r06': '''c
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

''',
    'cms_2jets_7tev': '''c
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

''',
    'cms_2jet_3d_8tev': '''c
      if ({}) then
        if (njet.lt.2) then
          passcuts_user=.false.
          return
        endif

        xystar = 0.5d0 * dabs(atanh(pjet(3,1)/pjet(0,1))-
     $                        atanh(pjet(3,2)/pjet(0,2)))
        xyboost = 0.5d0 * dabs(atanh(pjet(3,1)/pjet(0,1))+
     $                        atanh(pjet(3,2)/pjet(0,2)))
        xptavg = 0.5d0 * (pt_04(pjet(0,1)) + pt_04(pjet(0,2)))

        if (xptavg.lt.133d0) then
          passcuts_user=.false.
          return
        endif

        if (xyboost.lt.1d0) then
          if (xystar.lt.1d0) then
            if (xptavg.gt.1784d0) then
              passcuts_user=.false.
              return
            endif
          elseif (xystar.lt.2d0) then
            if (xptavg.gt.1248d0) then
              passcuts_user=.false.
              return
            endif
          elseif (xystar.lt.3d0) then
            if (xptavg.gt.548d0) then
              passcuts_user=.false.
              return
            endif
          else
            passcuts_user=.false.
            return
          endif
        else if (xyboost.lt.2d0) then
          if (xystar.lt.1d0) then
            if (xptavg.gt.1032d0) then
              passcuts_user=.false.
              return
            endif
          elseif (xystar.lt.2d0) then
            if (xptavg.gt.686d0) then
              passcuts_user=.false.
              return
            endif
          else
            passcuts_user=.false.
            return
          endif
        else if (xyboost.lt.3d0) then
          if (xystar.lt.1d0) then
            if (xptavg.gt.430d0) then
              passcuts_user=.false.
              return
            endif
          else
            passcuts_user=.false.
            return
          endif
        else
          passcuts_user=.false.
          return
        endif
      endif

''',
    'ptmiss': '''c     cut on the sum of all missing transverse momentum
      xptmiss=0d0

      do i=3,nexternal
        if (abs(ipdg_reco(i)).eq.12 .or.
     &      abs(ipdg_reco(i)).eq.14 .or.
     &      abs(ipdg_reco(i)).eq.16) then
          xptmiss(1)=xptmiss(1)+p_reco(1,i)
          xptmiss(2)=xptmiss(2)+p_reco(2,i)
        endif
      enddo

      if (xptmiss(1)**2+xptmiss(2)**2.lt.(({})**2)) then
        passcuts_user=.false.
        return
      endif

''',
    'mtw': '''c     cut on the transverse mass of W bosons
      do i=3,nexternal
        do j=i+1,nexternal
          if (is_a_lm(i) .or. is_a_lp(i) .or.
     &        is_a_lm(j) .or. is_a_lp(j)) then
            if (abs(ipdg_reco(i)+ipdg_reco(j)).eq.1) then
              xmtw=2d0*sqrt((p_reco(1,i)**2+p_reco(2,i)**2)*
     &                      (p_reco(1,j)**2+p_reco(2,j)**2))-
     &             2d0*(p_reco(1,i)*p_reco(1,j)+p_reco(2,i)*p_reco(2,j))
              if (xmtw.lt.(({})**2)) then
                passcuts_user=.false.
                return
              endif
            endif
          endif
        enddo
      enddo

''',
}

if __name__ == '__main__':
    if len(sys.argv) < 5:
        print('Error: wrong number of arguments: {}'.format(sys.argv))
        exit(1)

    # the name of the file we want to patch
    filename = sys.argv[1]

    if not os.path.exists(filename):
        print('Error: cut file `{}` does not exist'.format(filename))
        exit(2)

    for i in zip(sys.argv[2::3], sys.argv[3::3], sys.argv[4::3]):
        name = i[0]

        # check if the cut is recognised
        if not name in cuts_code:
            print('Error: unrecognised cut: {}'.format(name))
            exit(3)

        if i[1] != '=':
            print('Error: wrong argument format')
            exit(4)

    with open(filename, 'r') as fd:
        contents = fd.readlines()

    insertion_marker = 'logical function passcuts_user'
    marker_pos = -1

    for lineno, value in enumerate(contents):
        if insertion_marker in value:
            marker_pos = lineno
            break

    if marker_pos == -1:
        print('Error: could not find insertion marker `{}` in cut file `{}`'
            .format(insertion_marker, filename))
        exit(5)

    marker_pos = marker_pos + 8

    for name in cuts_variables:
        if any(i[0].startswith(name) for i in zip(sys.argv[2::3])):
            contents.insert(marker_pos, cuts_variables[name])

    insertion_marker = 'USER-DEFINED CUTS'
    marker_pos = -1

    for lineno, value in enumerate(contents):
        if insertion_marker in value:
            marker_pos = lineno
            break

    if marker_pos == -1:
        print('Error: could not find insertion marker `{}` in cut file `{}`'
            .format(insertion_marker, filename))
        exit(5)

    # skip some lines with comments
    marker_pos = marker_pos + 4
    # insert and empty line
    contents.insert(marker_pos - 1, '\n')

    for i in zip(reversed(sys.argv[2::3]), reversed(sys.argv[4::3])):
        name = i[0]
        value = i[1]

        try:
            value = float(value)
        except ValueError:
            if value == "True":
                value = ".true."
            elif value == "False":
                value = ".false."
            else:
                print('Error: format of value `{}` not understood'.format(value))
                exit(6)

        code = cuts_code[name].format(value)
        contents.insert(marker_pos, code)

    with open(filename, 'w') as fd:
        fd.writelines(contents)
