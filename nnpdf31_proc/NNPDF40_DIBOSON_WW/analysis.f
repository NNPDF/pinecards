cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call set_error_estimation(1)
      call HwU_inithist(nwgt,weights_info)
      call HwU_book( 1,'memu',1,  55d0,  75d0)
      call HwU_book( 2,'memu',2,  75d0,  95d0)
      call HwU_book( 3,'memu',3,  95d0, 140d0)
      call HwU_book( 4,'memu',1, 140d0, 160d0)
      call HwU_book( 5,'memu',1, 160d0, 185d0)
      call HwU_book( 6,'memu',1, 185d0, 220d0)
      call HwU_book( 7,'memu',1, 220d0, 280d0)
      call HwU_book( 8,'memu',1, 280d0, 380d0)
      call HwU_book( 9,'memu',1, 380d0, 600d0)
      call HwU_book(10,'memu',1, 600d0,1500d0)
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
      integer iPDG_reco(nexternal)



      call recombine_momenta(rphreco, etaphreco, lepphreco, quarkphreco,
     $                       p, iPDG, p_reco, iPDG_reco)

      do j = nincoming+1, nexternal
        if (iPDG_reco(j).eq.11) ppl(0:3)=p_reco(0:3,j)
        if (iPDG_reco(j).eq.-13) pplb(0:3)=p_reco(0:3,j)
      enddo

      do i=0,3
        ppv(i)=ppl(i)+pplb(i)
      enddo

      ptll = dsqrt(ppv(0)**2-ppv(1)**2-ppv(2)**2-ppv(3)**2)

      call HwU_fill( 1,ptll,wgts)
      call HwU_fill( 2,ptll,wgts)
      call HwU_fill( 3,ptll,wgts)
      call HwU_fill( 4,ptll,wgts)
      call HwU_fill( 5,ptll,wgts)
      call HwU_fill( 6,ptll,wgts)
      call HwU_fill( 7,ptll,wgts)
      call HwU_fill( 8,ptll,wgts)
      call HwU_fill( 9,ptll,wgts)
      call HwU_fill(10,ptll,wgts)

 999  return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
