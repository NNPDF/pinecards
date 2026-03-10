cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call set_error_estimation(1)
      call HwU_inithist(nwgt,weights_info)
      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'tt y ', 1,-2.6d0,-1.6d0)
      call HwU_book(2,'tt y ', 1,-1.6d0,-1.2d0)
      call HwU_book(3,'tt y ', 1,-1.2d0,-0.8d0)
      call HwU_book(4,'tt y ', 1,-0.8d0,-0.4d0)
      call HwU_book(5,'tt y ', 1,-0.4d0,0.0d0)
      call HwU_book(6,'tt y ', 1,0.0d0,0.4d0)
      call HwU_book(7,'tt y ', 1,0.4d0,0.8d0)
      call HwU_book(8,'tt y ', 1,0.8d0,1.2d0)
      call HwU_book(9,'tt y ', 1,1.2d0,1.6d0)
      call HwU_book(10,'tt y ', 1,1.6d0,2.6d0)
      
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
      double precision pttx(0:3), ytt
      double precision getrapidity
      external getrapidity

      do i=0,3
         pttx(i)=p(i,3)+p(i,4)
      enddo

      ytt = getrapidity(pttx(0), pttx(3))
      
      call HwU_fill(1,ytt,wgts)
      call HwU_fill(2,ytt,wgts)
      call HwU_fill(3,ytt,wgts)
      call HwU_fill(4,ytt,wgts)
      call HwU_fill(5,ytt,wgts)
      call HwU_fill(6,ytt,wgts)
      call HwU_fill(7,ytt,wgts)
      call HwU_fill(8,ytt,wgts)
      call HwU_fill(9,ytt,wgts)
      call HwU_fill(10,ytt,wgts)
      
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
