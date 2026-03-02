cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call set_error_estimation(1)
      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'tt m ', 1,250d0,400d0)
      call HwU_book(2,'tt m ', 1,400d0,480d0)
      call HwU_book(3,'tt m ', 1,480d0,560d0)
      call HwU_book(4,'tt m ', 1,560d0,640d0)
      call HwU_book(5,'tt m ', 1,640d0,720d0)
      call HwU_book(6,'tt m ', 1,720d0,800d0)
      call HwU_book(7,'tt m ', 1,800d0,900d0)
      call HwU_book(8,'tt m ', 1,900d0,1000d0)
      call HwU_book(9,'tt m ', 1,1000d0,1150d0)
      call HwU_book(10,'tt m ', 1,1150d0,1300d0)
      call HwU_book(11,'tt m ', 1,1300d0,1500d0)
      call HwU_book(12,'tt m ', 1,1500d0,1700d0)
      call HwU_book(13,'tt m ', 1,1700d0,2000d0)
      call HwU_book(14,'tt m ', 1,2000d0,2300d0)
      call HwU_book(15,'tt m ', 1,2300d0,3500d0)
      
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
      double precision pttx(0:3), mtt
      double precision dot

      do i=0,3
         pttx(i)=p(i,3)+p(i,4)
      enddo
      
      mtt = dsqrt(dot(pttx, pttx))
      
      call HwU_fill(1,mtt,wgts)
      call HwU_fill(2,mtt,wgts)
      call HwU_fill(3,mtt,wgts)
      call HwU_fill(4,mtt,wgts)
      call HwU_fill(5,mtt,wgts)
      call HwU_fill(6,mtt,wgts)
      call HwU_fill(7,mtt,wgts)
      call HwU_fill(8,mtt,wgts)
      call HwU_fill(9,mtt,wgts)
      call HwU_fill(10,mtt,wgts)
      call HwU_fill(11,mtt,wgts)
      call HwU_fill(12,mtt,wgts)
      call HwU_fill(13,mtt,wgts)
      call HwU_fill(14,mtt,wgts)
      call HwU_fill(15,mtt,wgts)

 999  return      
      end

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      
