cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'cos theta*',50, 0d0,  2d0)
      call HwU_book(2,'cos theta*',50, 2d0,  4d0)
      call HwU_book(3,'cos theta*',50, 4d0,  6d0)
      call HwU_book(4,'cos theta*',50, 6d0,  8d0)
      call HwU_book(5,'cos theta*',50, 8d0, 10d0)
      call HwU_book(6,'cos theta*',50,10d0, 12d0)
      call HwU_book(7,'cos theta*',50,12d0, 14d0)

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_end(dummy)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      double precision dummy
      call HwU_write_file
      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_fill(p,istatus,ipdg,wgts,ibody)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      include 'nexternal.inc'
      include 'cuts.inc'
      integer istatus(nexternal)
      integer iPDG(nexternal)
      integer ibody
      integer i
      integer j
      double precision p(0:4,nexternal)
      double precision wgts(*)
      double precision ppl(0:3), pplb(0:3), ppv(0:3)
      logical is_nextph_iso(nexternal),is_nextph_iso_reco(nexternal)
      double precision xmll, xcos, getcostheta
      external getcostheta

      double precision p_reco(0:4,nexternal)
      integer iPDG_reco(nexternal)



      is_nextph_iso(:) = .false.
      call recombine_momenta(rphreco, etaphreco, lepphreco, quarkphreco,
     $                       p, iPDG, is_nextph_iso, p_reco, iPDG_reco,
     $                       is_nextph_iso_reco)

      do j = nincoming+1, nexternal
        if (iPDG_reco(j).eq.13) ppl(0:3)=p_reco(0:3,j)
        if (iPDG_reco(j).eq.-13) pplb(0:3)=p_reco(0:3,j)
      enddo
      do i=0,3
        ppv(i)=ppl(i)+pplb(i)
      enddo

      xmll=sqrt(ppv(0)**2-ppv(1)**2-ppv(2)**2-ppv(3)**2)
      xcos=getcostheta(ppl(0),ppl(1),ppl(2),ppl(3),pplb(0),pplb(1),
     &                 pplb(2),pplb(3))

      if (xmll < 200d0) then
        print *, 'xmll too low', xmll
        stop
      else if (xmll < 500d0) then
        call HwU_fill(1,xcos + 1d0,wgts)
      else if (xmll < 1000d0) then
        call HwU_fill(2,xcos + 3d0,wgts)
      else if (xmll < 3000d0) then
        call HwU_fill(3,xcos + 5d0,wgts)
      else if (xmll < 5000d0) then
        call HwU_fill(4,xcos + 7d0,wgts)
      else if (xmll < 8000d0) then
        call HwU_fill(5,xcos + 9d0,wgts)
      else if (xmll < 10000d0) then
        call HwU_fill(6,xcos + 11d0,wgts)
      else
        call HwU_fill(7,xcos + 13d0,wgts)
      endif
      end

      function getcostheta(enl,ptxl,ptyl,pzl,enal,ptxal,ptyal,pzal)
      implicit none
      real*8 getcostheta,enl,ptxl,ptyl,pzl,enal,ptxal,ptyal,pzal
      real*8 pzll,mll,p1p,p1m,p2p,p2m,pt2ll
c     implementation of first formula on page 6 of https://arxiv.org/abs/1710.05167
      p1p = enl + pzl
      p1m = enl - pzl
      p2p = enal + pzal
      p2m = enal - pzal
      pzll = pzl + pzal
      pt2ll = (ptxl + ptxal) * (ptxl + ptxal) +
     &        (ptyl + ptyal) * (ptyl + ptyal)
      mll = sqrt((enl + enal) * (enl + enal) - (pt2ll + pzll * pzll))
      getcostheta = (p1p*p2m-p1m*p2p)/sqrt(mll*mll+pt2ll)/mll*
     &              sign(1d0,pzll)

      if (abs(getcostheta) > 1d0) then
        print *, 'Calculating cos with cos > 1'
        stop
      endif
      end
