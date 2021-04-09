cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call set_error_estimation(1)
      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'dist',16,0d0,16d0)

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
      double precision ptt,yt
      double precision getabsy
      external getabsy
      double precision bin

      ptt = dsqrt(p(1,3)**2 + p(2,3)**2)
      yt = abs(getabsy(p(0,3), p(3,3)))

      bin = -1d0

      if (yt.lt.0.35d0) then
        bin = 0d0
      elseif (yt.lt.0.85d0) then
        bin = 4d0
      elseif (yt.lt.1.45d0) then
        bin = 8d0
      elseif (yt.lt.2.5d0) then
        bin = 12d0
      else
      endif

      if (ptt.lt.80d0) then
        bin = bin + 0.5d0
      elseif (ptt.lt.150d0) then
        bin = bin + 1.5d0
      elseif (ptt.lt.250d0) then
        bin = bin + 2.5d0
      elseif (ptt.lt.600d0) then
        bin = bin + 3.5d0
      else
      endif

      if (bin.gt.0d0) then
        call HwU_fill(1,bin,wgts)
      else
      endif

 999  return
      end

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      function getabsy(en,pl)
      implicit none
      real*8 getabsy,en,pl,tmp
      tmp=pl/en
      if(abs(tmp).lt.1d0)then
        tmp=abs(atanh(tmp))
      else
        write(*,*)'Attempt to compute atanh(x) with x > 1'
        stop
      endif
      getabsy=tmp
      return
      end
