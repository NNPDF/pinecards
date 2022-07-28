cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'lmlp m yrap',96, 0d0, 96d0)
      call HwU_book(2,'lmlp m yrap',36,96d0,132d0)

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
      logical is_nextph_iso(nexternal),is_nextph_iso_reco(nexternal)
      double precision xyll, getabsy, xbin
      external getinvm
      external getabsy

      double precision p_reco(0:4,nexternal)
      integer iPDG_reco(nexternal)



      is_nextph_iso(:) = .false.
      call recombine_momenta(rphreco, etaphreco, lepphreco, quarkphreco,
     $                       p, iPDG, is_nextph_iso, p_reco, iPDG_reco,
     $                       is_nextph_iso_reco)

      do j = nincoming+1, nexternal
        if (iPDG_reco(j).eq.13) ppl(0:3)=p_reco(0:3,j)
        if (iPDG_reco(j).eq.-13) pplb(0:3)=p_reco(0:3,j)
      enddo
      do i=0,3
        ppv(i)=ppl(i)+pplb(i)
      enddo

      xmll=getinvm(ppv(0),ppv(1),ppv(2),ppv(3))
      xyll=getabsy(ppv(0),ppv(3))

      if (xyll.gt.2.4d0) then
        write (*,*) "error: event outside bin", xmll, xyll
        stop 1
      else
        xbin = dble(int(24d0 * xyll / 2.4d0)) + 0.5d0
      endif

      if (xmll.lt.20d0) then
        write (*,*) "error: event outside bin", xmll, xyll
        stop 1
      elseif (xmll.lt.30d0) then
        xbin=xbin+0d0
      elseif (xmll.lt.45d0) then
        xbin=xbin+24d0
      elseif (xmll.lt.60d0) then
        xbin=xbin+24d0+24d0
      elseif (xmll.lt.120d0) then
        xbin=xbin+24d0+24d0+24d0
      elseif (xmll.lt.200d0) then
        xbin=xbin+24d0+24d0+24d0+24d0
      elseif (xmll.lt.1500d0) then
        xbin=24d0+24d0+24d0+24d0+24d0
        xbin=xbin+dble(int(12d0 * xyll / 2.4d0)) + 0.5d0
      else
        write (*,*) "error: event outside bin", xmll, xyll
        stop 1
      endif

      if (xbin.lt.96d0) then
        call HwU_fill(1,xbin,wgts)
      else
        call HwU_fill(2,xbin,wgts)
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

      function getabsy(en,pl)
      implicit none
      real*8 getabsy,en,pl,tmp
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
