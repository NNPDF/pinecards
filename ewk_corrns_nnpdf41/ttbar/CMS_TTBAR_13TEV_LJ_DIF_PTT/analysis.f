cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call set_error_estimation(1)
      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'pTt ', 1,0d0,40d0)
      call HwU_book(2,'pTt ', 1,40d0,80d0)
      call HwU_book(3,'pTt ', 1,80d0,120d0)
      call HwU_book(4,'pTt ', 1,120d0,160d0)
      call HwU_book(5,'pTt ', 1,160d0,200d0)
      call HwU_book(6,'pTt ', 1,200d0,250d0)
      call HwU_book(7,'pTt ', 1,250d0,300d0)
      call HwU_book(8,'pTt ', 1,300d0,350d0)
      call HwU_book(9,'pTt ', 1,350d0,400d0)
      call HwU_book(10,'pTt ', 1,400d0,450d0)
      call HwU_book(11,'pTt ', 1,450d0,500d0)
      call HwU_book(12,'pTt ', 1,500d0,600d0)
      call HwU_book(13,'pTt ', 1,600d0,700d0)
      call HwU_book(14,'pTt ', 1,700d0,850d0)
      call HwU_book(15,'pTt ', 1,850d0,1000d0)
      call HwU_book(16,'pTt ', 1,1000d0,1500d0)

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
      call HwU_fill(8,ptt,wgts)
      call HwU_fill(9,ptt,wgts)
      call HwU_fill(10,ptt,wgts)
      call HwU_fill(11,ptt,wgts)
      call HwU_fill(12,ptt,wgts)
      call HwU_fill(13,ptt,wgts)
      call HwU_fill(14,ptt,wgts)
      call HwU_fill(15,ptt,wgts)
      call HwU_fill(16,ptt,wgts)

 999  return      
      end

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      
