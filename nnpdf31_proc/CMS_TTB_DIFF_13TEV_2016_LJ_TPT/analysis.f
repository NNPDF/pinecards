cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call set_error_estimation(1)
      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'t pT ',7,  0.0d0,280.0d0)
      call HwU_book(2,'t pT ',3,280.0d0,430.0d0)
      call HwU_book(3,'t pT ',1,430.0d0,500.0d0)
      call HwU_book(4,'t pT ',1,500.0d0,800.0d0)

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
      integer istatus(nexternal)
      integer iPDG(nexternal)
      integer ibody
      integer i
      double precision p(0:4,nexternal)
      double precision wgts(*)
      double precision pt_t
 
      pt_t = dsqrt(p(1,3)**2 + p(2,3)**2)

      call HwU_fill(1,pt_t,wgts)
      call HwU_fill(2,pt_t,wgts)
      call HwU_fill(3,pt_t,wgts)
      call HwU_fill(4,pt_t,wgts)

 999  return
      end

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
