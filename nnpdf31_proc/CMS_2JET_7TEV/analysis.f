cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call set_error_estimation(1)
      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'2D diff',1,0d0,54d0)

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
      integer ibody
      integer i
      integer j
      double precision p(0:4,nexternal)
      double precision wgts(*)
      double precision ppl(0:3), pplb(0:3), ppv(0:3), ptv, getinvm
      double precision xmjj, xymax
      external getinvm

      double precision p_reco(0:4,nexternal)
      integer iPDG_reco(nexternal)

      call recombine_momenta(rphreco, etaphreco, lepphreco, quarkphreco,
     $                       p, iPDG, p_reco, iPDG_reco)

c      do j = nincoming+1, nexternal
c        if (iPDG_reco(j).eq.13) ppl(0:3)=p_reco(0:3,j)
c        if (iPDG_reco(j).eq.-13) pplb(0:3)=p_reco(0:3,j)
c      enddo
c      do i=0,3
c        ppv(i)=ppl(i)+pplb(i)
c      enddo
c
c      ptv=sqrt(ppv(1)**2+ppv(2)**2)

c     TODO: calculate real values
      xymax = 0.25d0
      xmjj = 200.0d0

      if (xymax.lt.0.5d0) then
        if ((xmjj.gt.197d0).and.(xmjj.lt.296d0)) then
        elseif (xmjj.lt.419d0) then
        elseif (xmjj.lt.565d0) then
        elseif (xmjj.lt.740d0) then
        elseif (xmjj.lt.944d0) then
        elseif (xmjj.lt.1181d0) then
        elseif (xmjj.lt.1455d0) then
        elseif (xmjj.lt.1770d0) then
        elseif (xmjj.lt.2132d0) then
        elseif (xmjj.lt.2546d0) then
        elseif (xmjj.lt.3019d0) then
        elseif (xmjj.lt.3416d0) then
        elseif (xmjj.lt.4010d0) then
        else
          stop 1
        endif
      else if (xymax.lt.1.0d0) then
        if ((xmjj.gt.270d0).and.(xmjj.lt.386d0)) then
        elseif (xmjj.lt.526d0) then
        elseif (xmjj.lt.693d0) then
        elseif (xmjj.lt.890d0) then
        elseif (xmjj.lt.1118d0) then
        elseif (xmjj.lt.1383d0) then
        elseif (xmjj.lt.1687d0) then
        elseif (xmjj.lt.2037d0) then
        elseif (xmjj.lt.2438d0) then
        elseif (xmjj.lt.2895d0) then
        elseif (xmjj.lt.3416d0) then
        elseif (xmjj.lt.4010d0) then
        else
          stop 1
        endif
      else if (xymax.lt.1.5d0) then
        if ((xmjj.gt.419d0).and.(xmjj.lt.565d0)) then
        elseif (xmjj.lt.740d0) then
        elseif (xmjj.lt.944d0) then
        elseif (xmjj.lt.1181d0) then
        elseif (xmjj.lt.1455d0) then
        elseif (xmjj.lt.1770d0) then
        elseif (xmjj.lt.2132d0) then
        elseif (xmjj.lt.2546d0) then
        elseif (xmjj.lt.3147d0) then
        elseif (xmjj.lt.3854d0) then
        elseif (xmjj.lt.4509d0) then
        else
          stop 1
        endif
      else if (xymax.lt.2.0d0) then
        if ((xmjj.gt.565d0).and.(xmjj.lt.740d0)) then
        elseif (xmjj.lt.944d0) then
        elseif (xmjj.lt.1181d0) then
        elseif (xmjj.lt.1455d0) then
        elseif (xmjj.lt.1770d0) then
        elseif (xmjj.lt.2132d0) then
        elseif (xmjj.lt.2546d0) then
        elseif (xmjj.lt.3019d0) then
        elseif (xmjj.lt.3558d0) then
        elseif (xmjj.lt.5058d0) then
        else
          stop 1
        endif
      else if (xymax.lt.2.5d0) then
        if ((xmjj.gt.1000d0).and.(xmjj.lt.1246d0)) then
        elseif (xmjj.lt.1530d0) then
        elseif (xmjj.lt.1856d0) then
        elseif (xmjj.lt.2231d0) then
        elseif (xmjj.lt.2659d0) then
        elseif (xmjj.lt.3147d0) then
        elseif (xmjj.lt.3704d0) then
        elseif (xmjj.lt.5058d0) then
        else
          stop 1
        endif
      else
        stop 1
      endif

 999  return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      function getinvm(en,ptx,pty,pl)
      implicit none
      real*8 getinvm,en,ptx,pty,pl,tiny,tmp
      parameter (tiny=1.d-5)
c
      tmp=en**2-ptx**2-pty**2-pl**2
      if(tmp.gt.0.d0)then
        tmp=sqrt(tmp)
      elseif(tmp.gt.-tiny)then
        tmp=0.d0
      else
        write(*,*)'Attempt to compute a negative mass'
        stop
      endif
      getinvm=tmp
      return
      end
