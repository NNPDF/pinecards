cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'dist',15,0d0,15d0)

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
      double precision pttx(0:3), ptt,mtt
      double precision bin
      double precision dot

      do i=0,3
         pttx(i)=p(i,3)+p(i,4)
      enddo
      
      mtt = dsqrt(dot(pttx, pttx))
      ptt = dsqrt(p(1,3)**2 + p(2,3)**2)

      bin = -1d0

      if (mtt.gt.325d0.and.mtt.lt.500d0) then
         if(ptt.gt.0d0.and.ptt.lt.90d0) then
            bin = 0.5d0
         elseif(ptt.gt.90d0.and.ptt.lt.180d0) then
            bin = 1.5d0
         elseif(ptt.gt.180d0.and.ptt.lt.1000d0) then
            bin = 2.5d0
         endif
      elseif(mtt.gt.500d0.and.mtt.lt.700d0) then
         if(ptt.gt.0d0.and.ptt.lt.80d0) then
            bin = 3.5d0
         elseif(ptt.gt.80d0.and.ptt.lt.170d0) then
            bin = 4.5d0
         elseif(ptt.gt.170d0.and.ptt.lt.280d0) then
            bin = 5.5d0
         elseif(ptt.gt.280d0.and.ptt.lt.1000d0) then
            bin = 6.5d0   
         endif
      elseif(mtt.gt.700d0.and.mtt.lt.1000d0) then
         if(ptt.gt.0d0.and.ptt.lt.80d0) then
            bin = 7.5d0
         elseif(ptt.gt.80d0.and.ptt.lt.170d0) then
            bin = 8.5d0
         elseif(ptt.gt.170d0.and.ptt.lt.270d0) then
            bin = 9.5d0
         elseif(ptt.gt.270d0.and.ptt.lt.370d0) then
            bin = 10.5d0
         elseif(ptt.gt.370d0.and.ptt.lt.1000d0) then
            bin = 11.5d0
         endif
      elseif(mtt.gt.1000d0.and.mtt.lt.2000d0) then
         if(ptt.gt.0d0.and.ptt.lt.180d0) then
            bin = 12.5d0
         elseif(ptt.gt.180d0.and.ptt.lt.280d0) then
            bin = 13.5d0
         elseif(ptt.gt.280d0.and.ptt.lt.1000d0) then
            bin = 14.5d0
         endif
      endif

      if (bin.gt.0d0) then
        call HwU_fill(1,bin,wgts)
      else
      endif

 999  return
      end

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

