cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'mll', 43, 0d0, 43d0)
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
      double precision p(0:4,nexternal)
      double precision wgts(*)
      double precision ppl(0:3), pplb(0:3), ppv(0:3), xmll
      logical is_nextph_iso(nexternal),is_nextph_iso_reco(nexternal)
      double precision obs
      integer i

      double precision p_reco(0:4,nexternal)
      integer iPDG_reco(nexternal),grid

      is_nextph_iso(:) = .false.
      call recombine_momenta(rphreco, etaphreco, lepphreco, quarkphreco,
     $                       p, iPDG, is_nextph_iso, p_reco, iPDG_reco,
     $                       is_nextph_iso_reco)

      do i = nincoming+1, nexternal
        if (iPDG_reco(i).eq.13) ppl(0:3)=p_reco(0:3,i)
        if (iPDG_reco(i).eq.-13) pplb(0:3)=p_reco(0:3,i)
      enddo
      do i=0,3
        ppv(i)=ppl(i)+pplb(i)
      enddo

      xmll=sqrt(ppv(0)**2-ppv(1)**2-ppv(2)**2-ppv(3)**2)
      obs=-1d0

      if (xmll.lt.15d0) then
        obs = -2
      else if (xmll.lt.20d0) then
        obs=0.5d0
      else if (xmll.lt.25d0) then
        obs=1.5d0
      else if (xmll.lt.30d0) then
        obs=2.5d0
      else if (xmll.lt.35d0) then
        obs=3.5d0
      else if (xmll.lt.40d0) then
        obs=4.5d0
      else if (xmll.lt.45d0) then
        obs=5.5d0
      else if (xmll.lt.50d0) then
        obs=6.5d0
      else if (xmll.lt.55d0) then
        obs=7.5d0
      else if (xmll.lt.60d0) then
        obs=8.5d0
      else if (xmll.lt.64d0) then
        obs=9.5d0
      else if (xmll.lt.68d0) then
        obs=10.5d0
      else if (xmll.lt.72d0) then
        obs=11.5d0
      else if (xmll.lt.76d0) then
        obs=12.5d0
      else if (xmll.lt.81d0) then
        obs=13.5d0
      else if (xmll.lt.86d0) then
        obs=14.5d0
      else if (xmll.lt.91d0) then
        obs=15.5d0
      else if (xmll.lt.96d0) then
        obs=16.5d0
      else if (xmll.lt.101d0) then
        obs=17.5d0
      else if (xmll.lt.106d0) then
        obs=18.5d0
      else if (xmll.lt.110d0) then
        obs=19.5d0
      else if (xmll.lt.115d0) then
        obs=20.5d0
      else if (xmll.lt.120d0) then
        obs=21.5d0
      else if (xmll.lt.126d0) then
        obs=22.5d0
      else if (xmll.lt.133d0) then
        obs=23.5d0
      else if (xmll.lt.141d0) then
        obs=24.5d0
      else if (xmll.lt.150d0) then
        obs=25.5d0
      else if (xmll.lt.160d0) then
        obs=26.5d0
      else if (xmll.lt.171d0) then
        obs=27.5d0
      else if (xmll.lt.185d0) then
        obs=28.5d0
      else if (xmll.lt.200d0) then
        obs=29.5d0
      else if (xmll.lt.220d0) then
        obs=30.5d0
      else if (xmll.lt.243d0) then
        obs=31.5d0
      else if (xmll.lt.273d0) then
        obs=32.5d0
      else if (xmll.lt.320d0) then
        obs=33.5d0
      else if (xmll.lt.380d0) then
        obs=34.5d0
      else if (xmll.lt.440d0) then
        obs=35.5d0
      else if (xmll.lt.510d0) then
        obs=36.5d0
      else if (xmll.lt.600d0) then
        obs=37.5d0
      else if (xmll.lt.700d0) then
        obs=38.5d0
      else if (xmll.lt.830d0) then
        obs=39.5d0
      else if (xmll.lt.1000d0) then
        obs=40.5d0
      else if (xmll.lt.1500d0) then
        obs=41.5d0
      else if (xmll.lt.3000d0) then
        obs=42.5d0
      else
        obs=-3
      endif

      if (obs.gt.0d0) then
        call HwU_fill(1,obs,wgts)
      else
        write (*,*) "error: event outside bins", obs
        stop 1
      endif

 999  return
      end
