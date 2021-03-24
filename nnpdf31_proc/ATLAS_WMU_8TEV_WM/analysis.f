cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call set_error_estimation(1)
      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'etal',5,0.00d0,1.05d0)
      call HwU_book(2,'etal',1,1.05d0,1.37d0)
      call HwU_book(3,'etal',1,1.37d0,1.52d0)
      call HwU_book(4,'etal',1,1.52d0,1.74d0)
      call HwU_book(5,'etal',1,1.74d0,1.95d0)
      call HwU_book(6,'etal',1,1.95d0,2.18d0)
      call HwU_book(7,'etal',1,2.18d0,2.40d0)
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
      double precision ppl(0:3), pplb(0:3), ppv(0:3), xmll, getinvm
      double precision xeta
      double precision eta_04
      external eta_04

      double precision p_reco(0:4,nexternal)
      integer iPDG_reco(nexternal)



      call recombine_momenta(rphreco, etaphreco, lepphreco, quarkphreco,
     $                       p, iPDG, p_reco, iPDG_reco)

      xeta = 0d0
      do i = nincoming+1, nexternal
        if (iPDG_reco(i).eq.13) then
          xeta = abs(eta_04(p_reco(0,i)))
          exit
        endif
      enddo

      if (xeta.lt.0d0.or.xeta.gt.2.5d0) then
        write (*,*) "error: event outside bins", xeta
        stop 1
      else
        call HwU_fill(1,xeta,wgts)
        call HwU_fill(2,xeta,wgts)
        call HwU_fill(3,xeta,wgts)
        call HwU_fill(4,xeta,wgts)
        call HwU_fill(5,xeta,wgts)
        call HwU_fill(6,xeta,wgts)
        call HwU_fill(7,xeta,wgts)
      endif

 999  return
      end
