cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'ptz', 5,  0d0, 150d0)
      call HwU_book(2,'ptz', 1,150d0, 220d0)
      call HwU_book(3,'ptz', 1,220d0,1000d0)
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
      integer i, j
      double precision ppl(0:3), pplb(0:3), ppv(0:3), ptll

      double precision p_reco(0:4,nexternal)
      logical is_nextph_iso(nexternal),is_nextph_iso_reco(nexternal)
      integer iPDG_reco(nexternal)



      is_nextph_iso(:) = .false.
      call recombine_momenta(rphreco, etaphreco, lepphreco, quarkphreco,
     $                       p, iPDG, is_nextph_iso, p_reco, iPDG_reco,
     $                       is_nextph_iso_reco)

      do j = nincoming+1, nexternal
        if (iPDG_reco(j).eq.11) ppl(0:3)=p_reco(0:3,j)
        if (iPDG_reco(j).eq.-11) pplb(0:3)=p_reco(0:3,j)
      enddo

      do i=0,3
        ppv(i)=ppl(i)+pplb(i)
      enddo

      ptll = dsqrt(ppv(1)**2+ppv(2)**2)

      call HwU_fill(1,ptll,wgts)
      call HwU_fill(2,ptll,wgts)
      call HwU_fill(3,ptll,wgts)

 999  return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
