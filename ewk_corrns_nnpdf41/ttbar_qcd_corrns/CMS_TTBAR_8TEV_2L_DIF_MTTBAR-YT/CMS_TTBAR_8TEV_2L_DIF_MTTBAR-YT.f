cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

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
      double precision mtt,yt,pttx(0:3)
      double precision dot
      double precision getrapidity
      external getrapidity
      double precision bin

      do i=0,3
         pttx(i)=p(i,3)+p(i,4)
      enddo
      
      mtt = dsqrt(dot(pttx, pttx))
      yt  = getrapidity(p(0,4), p(3,4))

      bin = -1d0

      if (mtt.gt.340d0.and.mtt.lt.400d0) then
         if(yt.gt.0d0.and.yt.lt.0.35d0) then
            bin = 0.5d0
         elseif(yt.gt.0.35d0.and.yt.lt.0.85d0) then
            bin = 1.5d0
         elseif(yt.gt.0.85d0.and.yt.lt.1.45d0) then
            bin = 2.5d0
         elseif(yt.gt.1.45d0.and.yt.lt.2.5d0) then
            bin = 3.5d0
         endif
      elseif (mtt.gt.400d0.and.mtt.lt.500d0) then
         if(yt.gt.0d0.and.yt.lt.0.35d0) then
            bin = 4.5d0
         elseif(yt.gt.0.35d0.and.yt.lt.0.85d0) then
            bin = 5.5d0
         elseif(yt.gt.0.85d0.and.yt.lt.1.45d0) then
            bin = 6.5d0
         elseif(yt.gt.1.45d0.and.yt.lt.2.5d0) then
            bin = 7.5d0
         endif  
      elseif (mtt.gt.500d0.and.mtt.lt.650d0) then
         if(yt.gt.0d0.and.yt.lt.0.35d0) then
            bin = 8.5d0
         elseif(yt.gt.0.35d0.and.yt.lt.0.85d0) then
            bin = 9.5d0
         elseif(yt.gt.0.85d0.and.yt.lt.1.45d0) then
            bin = 10.5d0
         elseif(yt.gt.1.45d0.and.yt.lt.2.5d0) then
            bin = 11.5d0
         endif
      elseif (mtt.gt.650d0.and.mtt.lt.1500d0) then
         if(yt.gt.0d0.and.yt.lt.0.35d0) then
            bin = 12.5d0
         elseif(yt.gt.0.35d0.and.yt.lt.0.85d0) then
            bin = 13.5d0
         elseif(yt.gt.0.85d0.and.yt.lt.1.45d0) then
            bin = 14.5d0
         elseif(yt.gt.1.45d0.and.yt.lt.2.5d0) then
            bin = 15.5d0
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
