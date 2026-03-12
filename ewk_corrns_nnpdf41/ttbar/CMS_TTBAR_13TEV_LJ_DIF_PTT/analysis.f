cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call set_error_estimation(1)
      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'pTt ', 5,0d0,200d0)
      call HwU_book(2,'pTt ', 6,200d0,500d0)
      call HwU_book(3,'pTt ', 1,500d0,600d0)
      call HwU_book(4,'pTt ', 1,600d0,700d0)
      call HwU_book(5,'pTt ', 1,700d0,850d0)
      call HwU_book(6,'pTt ', 1,850d0,1000d0)
      call HwU_book(7,'pTt ', 1,1000d0,1500d0)

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
      
      ptt   = dsqrt(p(1,3)**2 + p(2,3)**2)
      
      call HwU_fill(1,ptt,wgts)
      call HwU_fill(2,ptt,wgts)
      call HwU_fill(3,ptt,wgts)
      call HwU_fill(4,ptt,wgts)
      call HwU_fill(5,ptt,wgts)
      call HwU_fill(6,ptt,wgts)
      call HwU_fill(7,ptt,wgts)

 999  return      
      end

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      
