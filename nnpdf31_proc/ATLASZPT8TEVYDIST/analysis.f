cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call set_error_estimation(1)
      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'Z pT', 60, 0d0, 60d0)

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
      integer i,ptbin
      integer j
      double precision p(0:4,nexternal)
      double precision wgts(*)
      double precision ppl(0:3),pplb(0:3),ppv(0:3),ptv,yll
      double precision getabsy
      external getabsy

      double precision p_reco(0:4,nexternal)
      integer iPDG_reco(nexternal)

      call recombine_momenta(rphreco, etaphreco, lepphreco, quarkphreco,
     $                       p, iPDG, p_reco, iPDG_reco)

      do j = nincoming+1, nexternal
        if (iPDG_reco(j).eq.13) ppl(0:3)=p_reco(0:3,j)
        if (iPDG_reco(j).eq.-13) pplb(0:3)=p_reco(0:3,j)
      enddo
      do i=0,3
        ppv(i)=ppl(i)+pplb(i)
      enddo

      ptv=sqrt(ppv(1)**2+ppv(2)**2)

      if (ptv.ge.30d0.and.ptv.lt.37d0) then
        ptbin = 0
      elseif (ptv.ge.37d0.and.ptv.lt.45d0) then
        ptbin = 1
      elseif (ptv.ge.45d0.and.ptv.lt.55d0) then
        ptbin = 2
      elseif (ptv.ge.55d0.and.ptv.lt.65d0) then
        ptbin = 3
      elseif (ptv.ge.65d0.and.ptv.lt.75d0) then
        ptbin = 4
      elseif (ptv.ge.75d0.and.ptv.lt.85d0) then
        ptbin = 5
      elseif (ptv.ge.85d0.and.ptv.lt.105d0) then
        ptbin = 6
      elseif (ptv.ge.105d0.and.ptv.lt.150d0) then
        ptbin = 7
      elseif (ptv.ge.150d0.and.ptv.lt.200d0) then
        ptbin = 8
      elseif (ptv.ge.200d0.and.ptv.lt.900d0) then
        ptbin = 9
      endif

      yll=getabsy(ppv(0),ppv(3))

      call HwU_fill(1,dble(10*idint(yll/0.4d0)+ptbin),wgts)

 999  return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      function getabsy(en,pl)
      implicit none
      real*8 getabsy,en,pl,tmp
c
      tmp=pl/en
      if(abs(tmp).lt.1d0)then
        tmp=abs(atanh(tmp))
      else
        write(*,*)'Attempt to compute atanh(x) with x > 1'
        stop
      endif
      getabsy=tmp
      return
      end
