cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call set_error_estimation(1)
      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'2D diff',54,0d0,54d0)

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
c     subroutine parameters
      double precision p(0:4,nexternal)
      integer istatus(nexternal)
      integer iPDG(nexternal)
      double precision wgts(*)
      integer ibody

c     variables for amcatnlo_fastjetppgenkt
      double precision pQCD(0:3,nexternal),rfj,sycut,palg
     $     ,pjet(0:3,nexternal),etajet(nexternal)
      integer nQCD,jet(nexternal),njet

c     observables
      integer xbin
      double precision xymax,xmjj,ptjet(nexternal),yjet(nexternal)
     $     ,yjmax

c     functions
      double precision getptv4,getinvm,getrapidityv4
      external getptv4,getinvm,getrapidityv4

c     miscellaneous
      integer i,j

      nQCD=0
      do j=nincoming+1,nexternal
         if (abs(ipdg(j)).le.5 .or. ipdg(j).eq.21 .or.
     $                              ipdg(j).eq.22) then
            nQCD=nQCD+1
            do i=0,3
               pQCD(i,nQCD)=p(i,j)
            enddo
         endif
      enddo

      palg = -1.d0
      rfj = 0.7d0
      sycut = 30d0
      yjmax = 2.5d0

      do i=1,nexternal
         do j=0,3
            pjet(j,i)=0d0
         enddo
         jet(i)=0
      enddo

c     recombine momenta
      call amcatnlo_fastjetppgenkt_etamax(pQCD,nQCD,rfj,sycut,yjmax
     $     ,palg,pjet,njet,jet)

      if (njet.lt.2) then
        return
      endif

      do i=1,njet
        ptjet(i)=getptv4(pjet(0,i))
        if(i.gt.1)then
          if (ptjet(i).gt.ptjet(i-1)) then
            write (*,*) "Error 1: jets should be ordered in pt"
            stop
          endif
        endif
        yjet(i)=getrapidityv4(pjet(0,i))
      enddo

      xymax = max(dabs(yjet(1)), dabs(yjet(2)))
      xmjj = getinvm(pjet(0,1)+pjet(0,2),
     $               pjet(1,1)+pjet(1,2),
     $               pjet(2,1)+pjet(2,2),
     $               pjet(3,1)+pjet(3,2))

      xbin = -1d0

      if (xymax.lt.0.5d0) then
        if (xmjj.lt.197d0) then
            xbin = -2d0
        elseif (xmjj.lt.296d0) then
            xbin = 0d0
        elseif (xmjj.lt.419d0) then
            xbin = 1d0
        elseif (xmjj.lt.565d0) then
            xbin = 2d0
        elseif (xmjj.lt.740d0) then
            xbin = 3d0
        elseif (xmjj.lt.944d0) then
            xbin = 4d0
        elseif (xmjj.lt.1181d0) then
            xbin = 5d0
        elseif (xmjj.lt.1455d0) then
            xbin = 6d0
        elseif (xmjj.lt.1770d0) then
            xbin = 7d0
        elseif (xmjj.lt.2132d0) then
            xbin = 8d0
        elseif (xmjj.lt.2546d0) then
            xbin = 9d0
        elseif (xmjj.lt.3019d0) then
            xbin = 10d0
        elseif (xmjj.lt.3416d0) then
            xbin = 11d0
        elseif (xmjj.lt.4010d0) then
            xbin = 12d0
        else
            xbin = -3d0
        endif
      else if (xymax.lt.1.0d0) then
        if (xmjj.lt.270d0) then
            xbin = -4d0
        elseif (xmjj.lt.386d0) then
            xbin = 13d0
        elseif (xmjj.lt.526d0) then
            xbin = 14d0
        elseif (xmjj.lt.693d0) then
            xbin = 15d0
        elseif (xmjj.lt.890d0) then
            xbin = 16d0
        elseif (xmjj.lt.1118d0) then
            xbin = 17d0
        elseif (xmjj.lt.1383d0) then
            xbin = 18d0
        elseif (xmjj.lt.1687d0) then
            xbin = 19d0
        elseif (xmjj.lt.2037d0) then
            xbin = 20d0
        elseif (xmjj.lt.2438d0) then
            xbin = 21d0
        elseif (xmjj.lt.2895d0) then
            xbin = 22d0
        elseif (xmjj.lt.3416d0) then
            xbin = 23d0
        elseif (xmjj.lt.4010d0) then
            xbin = 24d0
        else
            xbin = -5d0
        endif
      else if (xymax.lt.1.5d0) then
        if (xmjj.lt.419d0) then
            xbin = -6d0
        elseif (xmjj.lt.565d0) then
            xbin = 25d0
        elseif (xmjj.lt.740d0) then
            xbin = 26d0
        elseif (xmjj.lt.944d0) then
            xbin = 27d0
        elseif (xmjj.lt.1181d0) then
            xbin = 28d0
        elseif (xmjj.lt.1455d0) then
            xbin = 29d0
        elseif (xmjj.lt.1770d0) then
            xbin = 30d0
        elseif (xmjj.lt.2132d0) then
            xbin = 31d0
        elseif (xmjj.lt.2546d0) then
            xbin = 32d0
        elseif (xmjj.lt.3147d0) then
            xbin = 33d0
        elseif (xmjj.lt.3854d0) then
            xbin = 34d0
        elseif (xmjj.lt.4509d0) then
            xbin = 35d0
        else
            xbin = -7d0
        endif
      else if (xymax.lt.2.0d0) then
        if (xmjj.lt.565d0) then
            xbin = -8d0
        elseif (xmjj.lt.740d0) then
            xbin = 36d0
        elseif (xmjj.lt.944d0) then
            xbin = 37d0
        elseif (xmjj.lt.1181d0) then
            xbin = 38d0
        elseif (xmjj.lt.1455d0) then
            xbin = 39d0
        elseif (xmjj.lt.1770d0) then
            xbin = 40d0
        elseif (xmjj.lt.2132d0) then
            xbin = 41d0
        elseif (xmjj.lt.2546d0) then
            xbin = 42d0
        elseif (xmjj.lt.3019d0) then
            xbin = 43d0
        elseif (xmjj.lt.3558d0) then
            xbin = 44d0
        elseif (xmjj.lt.5058d0) then
            xbin = 45d0
        else
            xbin = -9d0
        endif
      else if (xymax.lt.2.5d0) then
        if (xmjj.lt.1000d0) then
            xbin = -10d0
        elseif (xmjj.lt.1246d0) then
            xbin = 46d0
        elseif (xmjj.lt.1530d0) then
            xbin = 47d0
        elseif (xmjj.lt.1856d0) then
            xbin = 48d0
        elseif (xmjj.lt.2231d0) then
            xbin = 49d0
        elseif (xmjj.lt.2659d0) then
            xbin = 50d0
        elseif (xmjj.lt.3147d0) then
            xbin = 51d0
        elseif (xmjj.lt.3704d0) then
            xbin = 52d0
        elseif (xmjj.lt.5058d0) then
            xbin = 53d0
        else
            xbin = -11d0
        endif
      else
        xbin = -12d0
      endif

      if (xbin.lt.0d0) then
        write (*,*) "error: event outside bins", xymax, xmjj, xbin
      else
        call HwU_fill(1,xbin + 0.5d0,wgts)
      endif

 999  return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      function getptv4(p)
      implicit none
      real*8 getptv4,p(0:3)
c
      getptv4=sqrt(p(1)**2+p(2)**2)
      return
      end

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

      function getrapidity(en,pl)
      implicit none
      real*8 getrapidity,en,pl,tiny,xplus,xminus,y
      parameter (tiny=1.d-8)
c
      xplus=en+pl
      xminus=en-pl
      if(xplus.gt.tiny.and.xminus.gt.tiny)then
        if( (xplus/xminus).gt.tiny.and.(xminus/xplus).gt.tiny )then
          y=0.5d0*log( xplus/xminus )
        else
          y=sign(1.d0,pl)*1.d8
        endif
      else
        y=sign(1.d0,pl)*1.d8
      endif
      getrapidity=y
      return
      end

      function getrapidityv4(p)
      implicit none
      real*8 getrapidityv4,p(0:3)
      real*8 getrapidity
c
      getrapidityv4=getrapidity(p(0),p(3))
      return
      end
