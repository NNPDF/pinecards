cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call set_error_estimation(1)
      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'Z pT', 50, 0d0, 50d0)

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
      double precision p(0:4,nexternal)
      double precision wgts(*)
      double precision ppl(0:3),pplb(0:3),ppv(0:3),ptv,xyll,xbin
      double precision getabsy
      external getabsy

      double precision p_reco(0:4,nexternal)
      integer iPDG_reco(nexternal)

      call recombine_momenta(rphreco, etaphreco, lepphreco, quarkphreco,
     $                       p, iPDG, p_reco, iPDG_reco)

      do i = nincoming+1, nexternal
        if (iPDG_reco(i).eq.13) ppl(0:3)=p_reco(0:3,i)
        if (iPDG_reco(i).eq.-13) pplb(0:3)=p_reco(0:3,i)
      enddo
      do i=0,3
        ppv(i)=ppl(i)+pplb(i)
      enddo

      xyll=getabsy(ppv(0),ppv(3))
      ptv=sqrt(ppv(1)**2+ppv(2)**2)

      if (xyll.lt.2d0) then
        xbin = 10d0 * real(int(5d0 * xyll/2d0))
      else
        write (*,*) "error: event outside bins", ptv, xyll
        stop 1
      endif

      if (ptv.lt.20d0) then
        xbin = xbin + 0.5d0
      elseif (ptv.lt.40d0) then
        xbin = xbin + 1.5d0
      elseif (ptv.lt.60d0) then
        xbin = xbin + 2.5d0
      elseif (ptv.lt.80d0) then
        xbin = xbin + 3.5d0
      elseif (ptv.lt.100d0) then
        xbin = xbin + 4.5d0
      elseif (ptv.lt.120d0) then
        xbin = xbin + 5.5d0
      elseif (ptv.lt.140d0) then
        xbin = xbin + 6.5d0
      elseif (ptv.lt.170d0) then
        xbin = xbin + 7.5d0
      elseif (ptv.lt.200d0) then
        xbin = xbin + 8.5d0
      elseif (ptv.lt.1000d0) then
        xbin = xbin + 9.5d0
      else
        write (*,*) "error: event outside bins", ptv, xyll
        stop 1
      endif

      call HwU_fill(1,xbin,wgts)

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
