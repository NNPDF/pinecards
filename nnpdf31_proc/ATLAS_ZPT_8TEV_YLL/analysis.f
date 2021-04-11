cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call set_error_estimation(1)
      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'Z pT', 80, 0d0, 80d0)
      call HwU_book(2,'Z pT', 40,80d0,120d0)

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

      if (xyll.lt.2.4d0) then
        xbin = 20d0 * real(int(6d0 * xyll/2.4d0))
      else
        write (*,*) "error: event outside bins", ptv, xyll
        stop 1
      endif

      if (ptv.lt.2d0) then
        xbin = xbin + 0.5d0
      elseif (ptv.lt.4d0) then
        xbin = xbin + 1.5d0
      elseif (ptv.lt.6d0) then
        xbin = xbin + 2.5d0
      elseif (ptv.lt.8d0) then
        xbin = xbin + 3.5d0
      elseif (ptv.lt.10d0) then
        xbin = xbin + 4.5d0
      elseif (ptv.lt.13d0) then
        xbin = xbin + 5.5d0
      elseif (ptv.lt.16d0) then
        xbin = xbin + 6.5d0
      elseif (ptv.lt.20d0) then
        xbin = xbin + 7.5d0
      elseif (ptv.lt.25d0) then
        xbin = xbin + 8.5d0
      elseif (ptv.lt.30d0) then
        xbin = xbin + 9.5d0
      elseif (ptv.lt.37d0) then
        xbin = xbin + 10.5d0
      elseif (ptv.lt.45d0) then
        xbin = xbin + 11.5d0
      elseif (ptv.lt.55d0) then
        xbin = xbin + 12.5d0
      elseif (ptv.lt.65d0) then
        xbin = xbin + 13.5d0
      elseif (ptv.lt.75d0) then
        xbin = xbin + 14.5d0
      elseif (ptv.lt.85d0) then
        xbin = xbin + 15.5d0
      elseif (ptv.lt.105d0) then
        xbin = xbin + 16.5d0
      elseif (ptv.lt.150d0) then
        xbin = xbin + 17.5d0
      elseif (ptv.lt.200d0) then
        xbin = xbin + 18.5d0
      elseif (ptv.lt.900d0) then
        xbin = xbin + 19.5d0
      else
        write (*,*) "error: event outside bins", ptv, xyll
        stop 1
      endif

      if (xbin.lt.80d0) then
        call HwU_fill(1,xbin,wgts)
      else
        call HwU_fill(2,xbin,wgts)
      endif

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
