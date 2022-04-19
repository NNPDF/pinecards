cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'tt m ', 1, 300d0, 360d0)
      call HwU_book(2,'tt m ', 2, 360d0, 500d0)
      call HwU_book(3,'tt m ', 1, 500d0, 580d0)
      call HwU_book(4,'tt m ', 1, 580d0, 680d0)
      call HwU_book(5,'tt m ', 1, 680d0, 800d0)
      call HwU_book(6,'tt m ', 2, 800d0,1200d0)
      call HwU_book(7,'tt m ', 1,1200d0,1500d0)
      call HwU_book(8,'tt m ', 1,1500d0,2500d0)

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

      mtt    = dsqrt(dot(pttx, pttx))

      call HwU_fill(1,mtt,wgts)
      call HwU_fill(2,mtt,wgts)
      call HwU_fill(3,mtt,wgts)
      call HwU_fill(4,mtt,wgts)
      call HwU_fill(5,mtt,wgts)
      call HwU_fill(6,mtt,wgts)
      call HwU_fill(7,mtt,wgts)
      call HwU_fill(8,mtt,wgts)

 999  return
      end
