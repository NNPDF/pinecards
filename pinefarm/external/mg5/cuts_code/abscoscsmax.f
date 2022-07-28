c     cut on the maximum of the absolute value of the cosine of the Collins-Soper angle of SFOS pairs
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
              zcoscs=(zp1p*zp2m-zp1m*zp2p)/
     &                    sqrt(zmll*zmll+zpt2ll)/zmll*sign(1d0,zpzll)

              if (abs(zcoscs) .gt. {}) then
                passcuts_user=.false.
                return
              endif
            endif
          enddo
        endif
      enddo
