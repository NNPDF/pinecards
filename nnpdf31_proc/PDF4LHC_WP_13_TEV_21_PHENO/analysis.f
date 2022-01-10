cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'mll', 6, 0d0, 6d0)
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
      double precision obs
      integer i

      double precision p_reco(0:4,nexternal)
      integer iPDG_reco(nexternal),grid

      call recombine_momenta(rphreco, etaphreco, lepphreco, quarkphreco,
     $                       p, iPDG, p_reco, iPDG_reco)

      do i = nincoming+1, nexternal
        if (iPDG_reco(i).eq.14) ppl(0:3)=p_reco(0:3,i)
        if (iPDG_reco(i).eq.-13) pplb(0:3)=p_reco(0:3,i)
      enddo
      do i=0,3
        ppv(i)=ppl(i)+pplb(i)
      enddo

      xmll=sqrt(ppv(0)**2-ppv(1)**2-ppv(2)**2-ppv(3)**2)
      obs=-1d0

      if (xmll.lt.1000d0) then
        obs = -2d0
      else if (xmll.lt.1500d0) then
        obs=0.5d0
      else if (xmll.lt.2000d0) then
        obs=1.5d0
      else if (xmll.lt.3000d0) then
        obs=2.5d0
      else if (xmll.lt.4000d0) then
        obs=3.5d0
      else if (xmll.lt.5000d0) then
        obs=4.5d0
      else if (xmll.lt.7000d0) then
        obs=5.5d0
      else
        obs=-3d0
      endif

      if (obs.gt.0d0) then
        call HwU_fill(1,obs,wgts)
      else
        write (*,*) "error: event outside bins", obs
        stop 1
      endif

 999  return
      end
