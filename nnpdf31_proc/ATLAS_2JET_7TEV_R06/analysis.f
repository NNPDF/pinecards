cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call set_error_estimation(1)
      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'2D diff',90,0d0,90d0)

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
      double precision xystar,xmjj,ptjet(nexternal),yjet(nexternal)
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

      palg = -1.0d0
      rfj = 0.6d0
      sycut = 50d0
      yjmax = 3.0d0

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

      xystar = 0.5d0 * dabs(yjet(1) - yjet(2))
      xmjj = getinvm(pjet(0,1)+pjet(0,2),
     $               pjet(1,1)+pjet(1,2),
     $               pjet(2,1)+pjet(2,2),
     $               pjet(3,1)+pjet(3,2))

      xbin = -1d0

      if (xystar.lt.0.5d0) then
        if (xmjj.lt.260d0) then
            xbin = -2d0
        elseif (xmjj.lt.310d0) then
            xbin = 0d0
        elseif (xmjj.lt.370d0) then
            xbin = 1d0
        elseif (xmjj.lt.440d0) then
            xbin = 2d0
        elseif (xmjj.lt.510d0) then
            xbin = 3d0
        elseif (xmjj.lt.590d0) then
            xbin = 4d0
        elseif (xmjj.lt.670d0) then
            xbin = 5d0
        elseif (xmjj.lt.760d0) then
            xbin = 6d0
        elseif (xmjj.lt.850d0) then
            xbin = 7d0
        elseif (xmjj.lt.950d0) then
            xbin = 8d0
        elseif (xmjj.lt.1060d0) then
            xbin = 9d0
        elseif (xmjj.lt.1180d0) then
            xbin = 10d0
        elseif (xmjj.lt.1310d0) then
            xbin = 11d0
        elseif (xmjj.lt.1450d0) then
            xbin = 12d0
        elseif (xmjj.lt.1600d0) then
            xbin = 13d0
        elseif (xmjj.lt.1760d0) then
            xbin = 14d0
        elseif (xmjj.lt.1940d0) then
            xbin = 15d0
        elseif (xmjj.lt.2120d0) then
            xbin = 16d0
        elseif (xmjj.lt.2330d0) then
            xbin = 17d0
        elseif (xmjj.lt.2550d0) then
            xbin = 18d0
        elseif (xmjj.lt.2780d0) then
            xbin = 19d0
        elseif (xmjj.lt.4270d0) then
            xbin = 20d0
        else
            xbin = -3d0
        endif
      else if (xystar.lt.1.0d0) then
        if (xmjj.lt.310d0) then
            xbin = -4d0
        elseif (xmjj.lt.370d0) then
            xbin = 21d0
        elseif (xmjj.lt.440d0) then
            xbin = 22d0
        elseif (xmjj.lt.510d0) then
            xbin = 23d0
        elseif (xmjj.lt.590d0) then
            xbin = 24d0
        elseif (xmjj.lt.670d0) then
            xbin = 25d0
        elseif (xmjj.lt.760d0) then
            xbin = 26d0
        elseif (xmjj.lt.850d0) then
            xbin = 27d0
        elseif (xmjj.lt.950d0) then
            xbin = 28d0
        elseif (xmjj.lt.1060d0) then
            xbin = 29d0
        elseif (xmjj.lt.1180d0) then
            xbin = 30d0
        elseif (xmjj.lt.1310d0) then
            xbin = 31d0
        elseif (xmjj.lt.1450d0) then
            xbin = 32d0
        elseif (xmjj.lt.1600d0) then
            xbin = 33d0
        elseif (xmjj.lt.1760d0) then
            xbin = 34d0
        elseif (xmjj.lt.1940d0) then
            xbin = 35d0
        elseif (xmjj.lt.2120d0) then
            xbin = 36d0
        elseif (xmjj.lt.2330d0) then
            xbin = 37d0
        elseif (xmjj.lt.2550d0) then
            xbin = 38d0
        elseif (xmjj.lt.2780d0) then
            xbin = 39d0
        elseif (xmjj.lt.3040d0) then
            xbin = 40d0
        elseif (xmjj.lt.4270d0) then
            xbin = 41d0
        else
            xbin = -5d0
        endif
      else if (xystar.lt.1.5d0) then
        if (xmjj.lt.510d0) then
            xbin = -6d0
        elseif (xmjj.lt.590d0) then
            xbin = 42d0
        elseif (xmjj.lt.670d0) then
            xbin = 43d0
        elseif (xmjj.lt.760d0) then
            xbin = 44d0
        elseif (xmjj.lt.850d0) then
            xbin = 45d0
        elseif (xmjj.lt.950d0) then
            xbin = 46d0
        elseif (xmjj.lt.1060d0) then
            xbin = 47d0
        elseif (xmjj.lt.1180d0) then
            xbin = 48d0
        elseif (xmjj.lt.1310d0) then
            xbin = 49d0
        elseif (xmjj.lt.1450d0) then
            xbin = 50d0
        elseif (xmjj.lt.1600d0) then
            xbin = 51d0
        elseif (xmjj.lt.1760d0) then
            xbin = 52d0
        elseif (xmjj.lt.1940d0) then
            xbin = 53d0
        elseif (xmjj.lt.2120d0) then
            xbin = 54d0
        elseif (xmjj.lt.2330d0) then
            xbin = 55d0
        elseif (xmjj.lt.2550d0) then
            xbin = 56d0
        elseif (xmjj.lt.2780d0) then
            xbin = 57d0
        elseif (xmjj.lt.3040d0) then
            xbin = 58d0
        elseif (xmjj.lt.3310d0) then
            xbin = 59d0
        elseif (xmjj.lt.4640d0) then
            xbin = 60d0
        else
            xbin = -7d0
        endif
      else if (xystar.lt.2.0d0) then
        if (xmjj.lt.760d0) then
            xbin = -8d0
        elseif (xmjj.lt.850d0) then
            xbin = 61d0
        elseif (xmjj.lt.950d0) then
            xbin = 62d0
        elseif (xmjj.lt.1060d0) then
            xbin = 63d0
        elseif (xmjj.lt.1180d0) then
            xbin = 64d0
        elseif (xmjj.lt.1310d0) then
            xbin = 65d0
        elseif (xmjj.lt.1450d0) then
            xbin = 66d0
        elseif (xmjj.lt.1600d0) then
            xbin = 67d0
        elseif (xmjj.lt.1760d0) then
            xbin = 68d0
        elseif (xmjj.lt.1940d0) then
            xbin = 69d0
        elseif (xmjj.lt.2120d0) then
            xbin = 70d0
        elseif (xmjj.lt.2330d0) then
            xbin = 71d0
        elseif (xmjj.lt.2550d0) then
            xbin = 72d0
        elseif (xmjj.lt.2780d0) then
            xbin = 73d0
        elseif (xmjj.lt.3040d0) then
            xbin = 74d0
        elseif (xmjj.lt.3310d0) then
            xbin = 75d0
        elseif (xmjj.lt.3610d0) then
            xbin = 76d0
        elseif (xmjj.lt.4640d0) then
            xbin = 77d0
        else
            xbin = -9d0
        endif
      else if (xystar.lt.2.5d0) then
        if (xmjj.lt.1310d0) then
            xbin = -10d0
        elseif (xmjj.lt.1450d0) then
            xbin = 78d0
        elseif (xmjj.lt.1600d0) then
            xbin = 79d0
        elseif (xmjj.lt.1760d0) then
            xbin = 80d0
        elseif (xmjj.lt.2120d0) then
            xbin = 81d0
        elseif (xmjj.lt.2550d0) then
            xbin = 82d0
        elseif (xmjj.lt.3040d0) then
            xbin = 83d0
        elseif (xmjj.lt.3610d0) then
            xbin = 84d0
        elseif (xmjj.lt.5040d0) then
            xbin = 85d0
        else
            xbin = -11d0
        endif
      else if (xystar.lt.3.0d0) then
        if (xmjj.lt.2120d0) then
            xbin = -12d0
        elseif (xmjj.lt.2550d0) then
            xbin = 86d0
        elseif (xmjj.lt.3040d0) then
            xbin = 87d0
        elseif (xmjj.lt.3930d0) then
            xbin = 88d0
        elseif (xmjj.lt.5040d0) then
            xbin = 89d0
        else
            xbin = -12d0
        endif
      else
        xbin = -13d0
      endif

      if (xbin.lt.0d0) then
        write (*,*) "error: event outside bins", xystar, xmjj, xbin
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
