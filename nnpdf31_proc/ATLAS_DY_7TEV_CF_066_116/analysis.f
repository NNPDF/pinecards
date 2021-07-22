cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'lmlp inv m yrap',15,0d0,15d0)
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
      double precision xyll, getabsy, xbin
      external getinvm
      external getabsy

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
      xyll=getabsy(ppv(0),ppv(3))

      xbin = 0d0

      if (xmll.lt.66d0) then
        write (*,*) "error: event outside bins", xmll, xyll
        stop 1
      elseif (xmll.lt.116d0) then
        if (xyll.lt.1.2d0) then
          write (*,*) "error: event outside bins", xmll, xyll
          stop 1
        else if (xyll.lt.1.4d0) then
          xbin = 0.5d0
        else if (xyll.lt.1.6d0) then
          xbin = 1.5d0
        else if (xyll.lt.1.8d0) then
          xbin = 2.5d0
        else if (xyll.lt.2.0d0) then
          xbin = 3.5d0
        else if (xyll.lt.2.2d0) then
          xbin = 4.5d0
        else if (xyll.lt.2.4d0) then
          xbin = 5.5d0
        else if (xyll.lt.2.8d0) then
          xbin = 6.5d0
        else if (xyll.lt.3.2d0) then
          xbin = 7.5d0
        else if (xyll.lt.3.6d0) then
          xbin = 8.5d0
        else
          write (*,*) "error: event outside bins", xmll, xyll
          stop 1
        endif
      elseif (xmll.lt.150d0) then
        if (xyll.lt.1.2d0) then
          write (*,*) "error: event outside bins", xmll, xyll
          stop 1
        else if (xyll.lt.1.6d0) then
          xbin = 9.5d0
        else if (xyll.lt.2.0d0) then
          xbin = 10.5d0
        else if (xyll.lt.2.4d0) then
          xbin = 11.5d0
        else if (xyll.lt.2.8d0) then
          xbin = 12.5d0
        else if (xyll.lt.3.2d0) then
          xbin = 13.5d0
        else if (xyll.lt.3.6d0) then
          xbin = 14.5d0
        else
          write (*,*) "error: event outside bins", xmll, xyll
          stop 1
        endif
      else
        write (*,*) "error: event outside bins", xmll, xyll
        stop 1
      endif

      call HwU_fill(1,xbin,wgts)

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
