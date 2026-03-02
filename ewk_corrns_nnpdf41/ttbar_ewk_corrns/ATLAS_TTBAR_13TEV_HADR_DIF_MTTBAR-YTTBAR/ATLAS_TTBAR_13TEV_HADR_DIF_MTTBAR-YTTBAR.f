cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'dist',11,0d0,11d0)

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
      double precision mtt,ytt,pttx(0:3)
      double precision getrapidity
      external getrapidity
      double precision bin
      double precision dot

      do i=0,3
         pttx(i)=p(i,3)+p(i,4)
      enddo
      
      mtt = dsqrt(dot(pttx, pttx))
      ytt = getrapidity(pttx(0), pttx(3))

      bin = -1d0

      if (mtt.gt.0d0.and.mtt.lt.700d0) then
         if(ytt.gt.0d0.and.ytt.lt.0.46d0) then
            bin = 0.5d0
         elseif(ytt.gt.0.46d0.and.ytt.lt.0.91d0) then
            bin = 1.5d0
         elseif(ytt.gt.0.91d0.and.ytt.lt.1.55d0) then
            bin = 2.5d0
         elseif(ytt.gt.1.55d0.and.ytt.lt.2.5d0) then
            bin = 3.5d0   
         endif
      elseif(mtt.gt.700d0.and.mtt.lt.970d0) then
         if(ytt.gt.0d0.and.ytt.lt.0.33d0) then
            bin = 4.5d0
         elseif(ytt.gt.0.33d0.and.ytt.lt.0.62d0) then
            bin = 5.5d0
         elseif(ytt.gt.0.62d0.and.ytt.lt.0.95d0) then
            bin = 6.5d0
         elseif(ytt.gt.0.95d0.and.ytt.lt.2.5d0) then
            bin = 7.5d0   
         endif
      elseif(mtt.gt.970d0.and.mtt.lt.3000d0) then 
         if(ytt.gt.0d0.and.ytt.lt.0.4d0) then
            bin = 8.5d0
         elseif(ytt.gt.0.4d0.and.ytt.lt.0.79d0) then
            bin = 9.5d0
         elseif(ytt.gt.0.79d0.and.ytt.lt.2.5d0) then
            bin = 10.5d0
         endif
      endif

      if (bin.gt.0d0) then
        call HwU_fill(1,bin,wgts)
      else
      endif

 999  return
      end

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      function getrapidity(en,pl)
      implicit none
      real*8 getrapidity,en,pl,tiny,xplus,xminus,y
      parameter (tiny=1.d-8)
      xplus=en+pl
      xminus=en-pl
      if(xplus.gt.tiny.and.xminus.gt.tiny)then
         if( (xplus/xminus).gt.tiny.and.(xminus/xplus).gt.tiny)then
            y=0.5d0*log( xplus/xminus  )
         else
            y=sign(1.d0,pl)*1.d8
         endif
      else 
         y=sign(1.d0,pl)*1.d8
      endif
      getrapidity=y
      return
      end

