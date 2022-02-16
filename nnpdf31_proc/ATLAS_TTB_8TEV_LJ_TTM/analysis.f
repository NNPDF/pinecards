cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'tt m ', 1,345d0,400.0d0)
      call HwU_book(2,'tt m ', 1,400.0d0,470.0d0)
      call HwU_book(3,'tt m ', 1,470.0d0,550.0d0)
      call HwU_book(4,'tt m ', 1,550.0d0,650.0d0)
      call HwU_book(5,'tt m ', 1,650.0d0,800.0d0)
      call HwU_book(6,'tt m ', 1,800.0d0,1100.0d0)
      call HwU_book(7,'tt m ', 1,1100.0d0,1600.0d0)

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

 999  return
      end

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
