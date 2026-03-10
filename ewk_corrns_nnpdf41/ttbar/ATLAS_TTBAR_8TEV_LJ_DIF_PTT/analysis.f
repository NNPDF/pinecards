cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call set_error_estimation(1)
      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'pTt ', 1,0d0,60d0)
      call HwU_book(2,'pTt ', 1,60d0,100d0)
      call HwU_book(3,'pTt ', 1,100d0,150d0)
      call HwU_book(4,'pTt ', 1,150d0,200d0)
      call HwU_book(5,'pTt ', 1,200d0,260d0)
      call HwU_book(6,'pTt ', 1,260d0,320d0)
      call HwU_book(7,'pTt ', 1,320d0,400d0)
      call HwU_book(8,'pTt ', 1,400d0,500d0)
      
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
      double precision ptt
      
      ptt = dsqrt(p(1,3)**2 + p(2,3)**2)
      
      call HwU_fill(1,ptt,wgts)
      call HwU_fill(2,ptt,wgts)
      call HwU_fill(3,ptt,wgts)
      call HwU_fill(4,ptt,wgts)
      call HwU_fill(5,ptt,wgts)
      call HwU_fill(6,ptt,wgts)
      call HwU_fill(7,ptt,wgts)
      call HwU_fill(8,ptt,wgts)

 999  return      
      end

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      
