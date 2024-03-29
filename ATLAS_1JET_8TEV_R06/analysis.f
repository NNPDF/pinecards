cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'dist',90,  0d0, 90d0)
      call HwU_book(2,'dist',81, 90d0,171d0)

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
      double precision pQCD(0:3,nexternal),pjet(0:3,nexternal),
     $                 etajet(nexternal)
      integer nQCD,jet(nexternal),njet

c     observables
      double precision ptjet,yjet,xbin

c     functions
      double precision getptv4,getinvm,getrapidityv4
      external getptv4,getinvm,getrapidityv4

c     miscellaneous
      integer i,j,xbinneg(3)
      logical xjet

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

      do i=1,nexternal
         do j=0,3
            pjet(j,i)=0d0
         enddo
         jet(i)=0
      enddo

c     recombine momenta
      call amcatnlo_fastjetppgenkt_etamax(pQCD,nQCD,jetradius,ptj,
     $     etaj,jetalgo,pjet,njet,jet)

      xjet=.false.
      xbinneg(1)=0
      xbinneg(2)=0
      xbinneg(3)=0

      do i=1,njet
        ptjet=getptv4(pjet(0,i))
        yjet=dabs(getrapidityv4(pjet(0,i)))

        xbin = -1d0

        if (ptjet.lt.70d0) then
          xbin = -2d0
        elseif (ptjet.lt.85d0) then
          xbin = 0d0
        elseif (ptjet.lt.100d0) then
          xbin = 1d0
        elseif (ptjet.lt.116d0) then
          xbin = 2d0
        elseif (ptjet.lt.134d0) then
          xbin = 3d0
        elseif (ptjet.lt.152d0) then
          xbin = 4d0
        elseif (ptjet.lt.172d0) then
          xbin = 5d0
        elseif (ptjet.lt.194d0) then
          xbin = 6d0
        elseif (ptjet.lt.216d0) then
          xbin = 7d0
        elseif (ptjet.lt.240d0) then
          xbin = 8d0
        elseif (ptjet.lt.264d0) then
          xbin = 9d0
        elseif (ptjet.lt.290d0) then
          xbin = 10d0
        elseif (ptjet.lt.318d0) then
          xbin = 11d0
        elseif (ptjet.lt.346d0) then
          xbin = 12d0
        elseif (ptjet.lt.376d0) then
          xbin = 13d0
        elseif (ptjet.lt.408d0) then
          xbin = 14d0
        elseif (ptjet.lt.442d0) then
          xbin = 15d0
        elseif (ptjet.lt.478d0) then
          xbin = 16d0
        elseif (ptjet.lt.516d0) then
          xbin = 17d0
        elseif (ptjet.lt.556d0) then
          xbin = 18d0
        elseif (ptjet.lt.598d0) then
          xbin = 19d0
        elseif (ptjet.lt.642d0) then
          xbin = 20d0
        elseif (ptjet.lt.688d0) then
          xbin = 21d0
        elseif (ptjet.lt.736d0) then
          xbin = 22d0
        elseif (ptjet.lt.786d0) then
          xbin = 23d0
        elseif (ptjet.lt.838d0) then
          xbin = 24d0
        elseif (ptjet.lt.894d0) then
          xbin = 25d0
        elseif (ptjet.lt.952d0) then
          xbin = 26d0
        elseif (ptjet.lt.1012d0) then
          xbin = 27d0
        elseif (ptjet.lt.1076d0) then
          xbin = 28d0
        elseif (ptjet.lt.1162d0) then
          xbin = 29d0
        elseif (ptjet.lt.1310d0) then
          xbin = 30d0
        elseif (ptjet.lt.1530d0) then
          xbin = 31d0
        elseif (ptjet.lt.1992d0) then
          xbin = 32d0
        elseif (ptjet.lt.2500d0) then
          xbin = 33d0
        else
          xbin = -3d0
        endif

        if (yjet.lt.0.5d0) then
        else if (yjet.lt.1.0d0) then
c         two bins are merged
          if (xbin.eq.33d0) then
            xbin = 32d0
          endif
          xbin = xbin + 34d0
        else if (yjet.lt.1.5d0) then
          if (ptjet.gt.1992d0) then
            xbin = -4d0
          else
c           two bins are merged
            if (xbin.eq.32d0) then
              xbin = 31d0
            endif
            xbin = xbin + 34d0 + 33d0
          endif
        else if (yjet.lt.2.0d0) then
          if (ptjet.gt.1310d0) then
            xbin = -5d0
          else
c           two bins are merged
            if (xbin.eq.30d0) then
              xbin = 29d0
            endif
            xbin = xbin + 34d0 + 33d0 + 32d0
          endif
        else if (yjet.lt.2.5d0) then
          if (ptjet.gt.838d0) then
            xbin = -6d0
          else
c           two bins are merged
            if (xbin.eq.24d0) then
              xbin = 23d0
            endif
            xbin = xbin + 34d0 + 33d0 + 32d0 + 30d0
          endif
        else if (yjet.lt.3.0d0) then
          if (ptjet.gt.556d0) then
            xbin = -6d0
          else
c           two bins are merged
            if (xbin.eq.18d0) then
              xbin = 17d0
            endif
            xbin = xbin + 34d0 + 33d0 + 32d0 + 30d0 + 24d0
          endif
        else
          xbin = -7d0
        endif

        if (xbin.lt.0d0) then
          xbinneg(i) = xbin
        else if (xbin.lt.90d0) then
          call HwU_fill(1,xbin + 0.5d0,wgts)
          xjet=.true.
        else
          call HwU_fill(2,xbin + 0.5d0,wgts)
          xjet=.true.
        endif
      enddo

      if (.not. xjet) then
        write (*,*) "error: event outside bins", xbinneg(1), xbinneg(2),
     &                                           xbinneg(3), njet
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
