cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'tt m ',1, 250.0d0, 450.0d0)
      call HwU_book(2,'tt m ',1, 450.0d0, 570.0d0)
      call HwU_book(3,'tt m ',1, 570.0d0, 700.0d0)
      call HwU_book(4,'tt m ',1, 700.0d0, 850.0d0)
      call HwU_book(5,'tt m ',1, 850.0d0,1000.0d0)
      call HwU_book(6,'tt m ',1,1000.0d0,2700.0d0)

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

 999  return
      end

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
