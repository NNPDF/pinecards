cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'lmlp m yrap', 24,  20d0*2.4,   30d0*2.4)
      call HwU_book(2,'lmlp m yrap', 24,  30d0*2.4,   45d0*2.4)
      call HwU_book(3,'lmlp m yrap', 24,  45d0*2.4,   60d0*2.4)
      call HwU_book(4,'lmlp m yrap', 24,  60d0*2.4,  120d0*2.4)
      call HwU_book(5,'lmlp m yrap', 24, 120d0*2.4,  200d0*2.4)
      call HwU_book(6,'lmlp m yrap', 12, 200d0*2.4, 1500d0*2.4)

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
      double precision xyll, getabsy
      external getinvm
      external getabsy
      integer bin
      double precision minmll, maxmll

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
      xyll=getabsy(ppv(0),ppv(1),ppv(2),ppv(3))

      bin = -1

      if (xmll.ge.20d0.and.xmll.lt.30d0) then
        bin=1
        minmll=20d0
        maxmll=30d0
      elseif (xmll.ge.30d0.and.xmll.lt.45d0) then
        bin=2
        minmll=30d0
        maxmll=45d0
      elseif (xmll.ge.45d0.and.xmll.lt.60d0) then
        bin=3
        minmll=45d0
        maxmll=60d0
      elseif (xmll.ge.60d0.and.xmll.lt.120d0) then
        bin=4
        minmll=60d0
        maxmll=120d0
      elseif (xmll.ge.120d0.and.xmll.lt.200d0) then
        bin=5
        minmll=120d0
        maxmll=200d0
      elseif (xmll.ge.200d0.and.xmll.lt.1500d0) then
        bin=6
        minmll=200d0
        maxmll=1500d0
      endif

      call HwU_fill(bin,minmll*2.4+xyll*(maxmll-minmll),wgts)


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

      function getabsy(en,ptx,pty,pl)
      implicit none
      real*8 getabsy,en,ptx,pty,pl,tmp
c
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

