cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'lmlp inv m yrap', 24, 0d0,   10d0*2.4)
c      call HwU_book(2,'lmlp inv m yrap', 24, 0d0,   15d0*2.4)
c      call HwU_book(3,'lmlp inv m yrap', 24, 0d0,   15d0*2.4)
c      call HwU_book(4,'lmlp inv m yrap', 24, 0d0,   60d0*2.4)
c      call HwU_book(5,'lmlp inv m yrap', 24, 0d0,   80d0*2.4)
c      call HwU_book(6,'lmlp inv m yrap', 12, 0d0, 1300d0*2.4)

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
      double precision ppl(0:3), pplb(0:3), ppv(0:3), xmll, getinvm
      double precision xyll, gety
      external getinvm
      external gety

      double precision p_reco(0:4,nexternal)
      integer iPDG_reco(nexternal)



      call recombine_momenta(rphreco, etaphreco, lepphreco, quarkphreco,
     $                       p, iPDG, p_reco, iPDG_reco)

      do j = nincoming+1, nexternal
        if (iPDG_reco(j).eq.13) ppl(0:3)=p_reco(0:3,j)
        if (iPDG_reco(j).eq.-13) pplb(0:3)=p_reco(0:3,j)
      enddo
      do i=0,3
        ppv(i)=ppl(i)+pplb(i)
      enddo

      xmll=getinvm(ppv(0),ppv(1),ppv(2),ppv(3))
      xyll=gety(ppv(0),ppv(1),ppv(2),ppv(3))

      if (xmll.ge.20d0.and.xmll.lt.30d0) then
        call HwU_fill(1,xyll*10d0,wgts)
c      elseif (xmll.ge.30d0.and.xmll.lt.45d0) then
c        call HwU_fill(2,xmll,wgts)
c      elseif (xmll.ge.45d0.and.xmll.lt.60d0) then
c        call HwU_fill(3,xmll,wgts)
c      elseif (xmll.ge.60d0.and.xmll.lt.120d0) then
c        call HwU_fill(4,xmll,wgts)
c      elseif (xmll.ge.120d0.and.xmll.lt.200d0) then
c        call HwU_fill(5,xmll,wgts)
c      elseif (xmll.ge.200d0.and.xmll.lt.1500d0) then
c        call HwU_fill(6,xmll,wgts)
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

      function gety(en,ptx,pty,pl)
      implicit none
      real*8 gety,en,ptx,pty,pl,tmp
c
      tmp=pl/en
      if(abs(tmp).lt.1d0)then
        tmp=atanh(tmp)
      else
        write(*,*)'Attempt to compute atanh(x) with x > 1'
        stop
      endif
      gety=tmp
      return
      end

