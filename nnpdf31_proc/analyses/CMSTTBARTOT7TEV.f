c
c Analysis for CMSTTBARTOT7TEV.
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      implicit none
      integer nwgt
      character*(*) weights_info(*)
      integer i,kk,l
      character*11 cc(3)
      data cc/'|T@NLOQCDEW','|T@NLOQCD  ','|T@LO      '/
      call HwU_inithist(nwgt,weights_info)
      do i=1,3
         l=(i-1)*1
         call HwU_book(l+ 1,'total rate    '//cc(i),  1,0.5d0,1.5d0)
      enddo
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
      include 'cuts.inc'
      integer istatus(nexternal)
      integer iPDG(nexternal)
      double precision p(0:4,nexternal)
      double precision wgts(*)
      integer ibody,mu,count_bj,count_j
      double precision wgt,var
      integer i,kk,l,itop
      double precision www,pQCD(0:3,nexternal),palg,rfj,sycut,pjet(0:3
     $     ,nexternal),ptj1,yj1,etaj1,ptbj1,ybj1,etabj1,ptj2,yj2,etaj2
     $     ,ptbj2,ybj2,etabj2,p_top(0:3),pttop,ytop,etatop,psyst(0:3)
     $     ,ptsyst,ysyst,etasyst,p_bjet(0:3,nexternal)
      integer nQCD,nb,is_b(nexternal),njet,jet(nexternal),nbjet
      logical is_b_jet(nexternal)
      double precision getpt,getrapidity,getpseudorap
      external getpt,getrapidity,getpseudorap
      double precision pi
      parameter (pi=3.14159265358979312d0)
      ! stuff for plotting the different splitorders
      integer orders_tag_plot
      common /corderstagplot/ orders_tag_plot
c      do i=0,3
c        pttx(i)=p(i,3)+p(i,4)
c      enddo
c      mtt    = dsqrt(dot(pttx, pttx))
c      pt_t   = dsqrt(p(1,3)**2 + p(2,3)**2)
c      pt_tx  = dsqrt(p(1,4)**2 + p(2,4)**2)
c      pt_ttx = dsqrt((p(1,3)+p(1,4))**2 + (p(2,3)+p(2,4))**2)
c      yt  = getrapidity(p(0,3), p(3,3))
c      ytx = getrapidity(p(0,4), p(3,4))
c      yttx= getrapidity(pttx(0), pttx(3))
      var=1.d0
      do i=1,3
         l=(i-1)*1
      !!data cc/'|T@NLOQCDEW','|T@NLOQCD  ','|T@LO      '/
         if (ibody.ne.3 .and.i.eq.3) cycle ! fill only born for i=3
         if (orders_tag_plot.eq.204 .and.i.eq.2) cycle ! do not fill the
         ! NLO EW for i=2
         call HwU_fill(l+1,var,wgts)
C         call HwU_fill(l+2,yt,wgts)
C         call HwU_fill(l+3,ytx,wgts)
C         call HwU_fill(l+4,yttx,wgts)
C         call HwU_fill(l+5,mtt,wgts)
C         call HwU_fill(l+6,pt_t,wgts)
C         call HwU_fill(l+7,pt_tx,wgts)
C         call HwU_fill(l+8,pt_ttx,wgts)
      enddo
c
 999  return      
      end


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
