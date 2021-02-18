cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call set_error_estimation(1)
      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'Z pT', 74, 0d0, 74d0)

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
      double precision p(0:4,nexternal)
      double precision wgts(*)
      double precision ppl(0:3),pplb(0:3),ppv(0:3),ptv,xmll,xbin
      double precision getinvm
      external getinvm

      double precision p_reco(0:4,nexternal)
      integer iPDG_reco(nexternal)

      call recombine_momenta(rphreco, etaphreco, lepphreco, quarkphreco,
     $                       p, iPDG, p_reco, iPDG_reco)

      do i = nincoming+1, nexternal
        if (iPDG_reco(i).eq.13) ppl(0:3)=p_reco(0:3,i)
        if (iPDG_reco(i).eq.-13) pplb(0:3)=p_reco(0:3,i)
      enddo
      do i=0,3
        ppv(i)=ppl(i)+pplb(i)
      enddo

      xmll=getinvm(ppv(0),ppv(1),ppv(2),ppv(3))
      ptv=sqrt(ppv(1)**2+ppv(2)**2)

      if (xmll.lt.12d0) then
        write (*,*) "error: event outside bins", xmll
        stop 1
      elseif (xmll.lt.20d0) then
        if (ptv.lt.45d0) then
          write (*,*) "error: event outside bins", ptv
          stop 1
        elseif (ptv.lt.55d0) then
          xbin = 0.5d0
        elseif (ptv.lt.65d0) then
          xbin = 1.5d0
        elseif (ptv.lt.75d0) then
          xbin = 2.5d0
        elseif (ptv.lt.85d0) then
          xbin = 3.5d0
        elseif (ptv.lt.105d0) then
          xbin = 4.5d0
        elseif (ptv.lt.150d0) then
          xbin = 5.5d0
        elseif (ptv.lt.200d0) then
          xbin = 6.5d0
        elseif (ptv.lt.900d0) then
          xbin = 7.5d0
        else
          write (*,*) "error: event outside bins", ptv
          stop 1
        endif
      elseif (xmll.lt.30d0) then
        if (ptv.lt.45d0) then
          write (*,*) "error: event outside bins", ptv
          stop 1
        elseif (ptv.lt.55d0) then
          xbin = 8.5d0
        elseif (ptv.lt.65d0) then
          xbin = 9.5d0
        elseif (ptv.lt.75d0) then
          xbin = 10.5d0
        elseif (ptv.lt.85d0) then
          xbin = 11.5d0
        elseif (ptv.lt.105d0) then
          xbin = 12.5d0
        elseif (ptv.lt.150d0) then
          xbin = 13.5d0
        elseif (ptv.lt.200d0) then
          xbin = 14.5d0
        elseif (ptv.lt.900d0) then
          xbin = 15.5d0
        else
          write (*,*) "error: event outside bins", ptv
          stop 1
        endif
      elseif (xmll.lt.46d0) then
        if (ptv.lt.45d0) then
          write (*,*) "error: event outside bins", ptv
          stop 1
        elseif (ptv.lt.55d0) then
          xbin = 16.5d0
        elseif (ptv.lt.65d0) then
          xbin = 17.5d0
        elseif (ptv.lt.75d0) then
          xbin = 18.5d0
        elseif (ptv.lt.85d0) then
          xbin = 19.5d0
        elseif (ptv.lt.105d0) then
          xbin = 20.5d0
        elseif (ptv.lt.150d0) then
          xbin = 21.5d0
        elseif (ptv.lt.200d0) then
          xbin = 22.5d0
        elseif (ptv.lt.900d0) then
          xbin = 23.5d0
        else
          write (*,*) "error: event outside bins", ptv
          stop 1
        endif
      elseif (xmll.lt.66d0) then
        if (ptv.lt.30d0) then
          write (*,*) "error: event outside bins", ptv
          stop 1
        elseif (ptv.lt.37d0) then
          xbin = 24.5d0
        elseif (ptv.lt.45d0) then
          xbin = 25.5d0
        elseif (ptv.lt.55d0) then
          xbin = 26.5d0
        elseif (ptv.lt.65d0) then
          xbin = 27.5d0
        elseif (ptv.lt.75d0) then
          xbin = 28.5d0
        elseif (ptv.lt.85d0) then
          xbin = 29.5d0
        elseif (ptv.lt.105d0) then
          xbin = 30.5d0
        elseif (ptv.lt.150d0) then
          xbin = 31.5d0
        elseif (ptv.lt.200d0) then
          xbin = 32.5d0
        elseif (ptv.lt.900d0) then
          xbin = 33.5d0
        else
          write (*,*) "error: event outside bins", ptv
          stop 1
        endif
      elseif (xmll.lt.116d0) then
        if (ptv.lt.27.5d0) then
          write (*,*) "error: event outside bins", ptv
          stop 1
        elseif (ptv.lt.30d0) then
          xbin = 34.5d0
        elseif (ptv.lt.33d0) then
          xbin = 35.5d0
        elseif (ptv.lt.36d0) then
          xbin = 36.5d0
        elseif (ptv.lt.39d0) then
          xbin = 37.5d0
        elseif (ptv.lt.42d0) then
          xbin = 38.5d0
        elseif (ptv.lt.45d0) then
          xbin = 39.5d0
        elseif (ptv.lt.48d0) then
          xbin = 40.5d0
        elseif (ptv.lt.51d0) then
          xbin = 41.5d0
        elseif (ptv.lt.54d0) then
          xbin = 42.5d0
        elseif (ptv.lt.57d0) then
          xbin = 43.5d0
        elseif (ptv.lt.61d0) then
          xbin = 44.5d0
        elseif (ptv.lt.65d0) then
          xbin = 45.5d0
        elseif (ptv.lt.70d0) then
          xbin = 46.5d0
        elseif (ptv.lt.75d0) then
          xbin = 47.5d0
        elseif (ptv.lt.80d0) then
          xbin = 48.5d0
        elseif (ptv.lt.85d0) then
          xbin = 49.5d0
        elseif (ptv.lt.95d0) then
          xbin = 50.5d0
        elseif (ptv.lt.105d0) then
          xbin = 51.5d0
        elseif (ptv.lt.125d0) then
          xbin = 52.5d0
        elseif (ptv.lt.150d0) then
          xbin = 53.5d0
        elseif (ptv.lt.175d0) then
          xbin = 54.5d0
        elseif (ptv.lt.200d0) then
          xbin = 55.5d0
        elseif (ptv.lt.250d0) then
          xbin = 56.5d0
        elseif (ptv.lt.300d0) then
          xbin = 57.5d0
        elseif (ptv.lt.350d0) then
          xbin = 58.5d0
        elseif (ptv.lt.400d0) then
          xbin = 59.5d0
        elseif (ptv.lt.470d0) then
          xbin = 60.5d0
        elseif (ptv.lt.550d0) then
          xbin = 61.5d0
        elseif (ptv.lt.650d0) then
          xbin = 62.5d0
        elseif (ptv.lt.900d0) then
          xbin = 63.5d0
        else
          write (*,*) "error: event outside bins", ptv
          stop 1
        endif
      elseif (xmll.lt.150d0) then
        if (ptv.lt.30d0) then
          write (*,*) "error: event outside bins", ptv
          stop 1
        elseif (ptv.lt.37d0) then
          xbin = 64.5d0
        elseif (ptv.lt.45d0) then
          xbin = 65.5d0
        elseif (ptv.lt.55d0) then
          xbin = 66.5d0
        elseif (ptv.lt.65d0) then
          xbin = 67.5d0
        elseif (ptv.lt.75d0) then
          xbin = 68.5d0
        elseif (ptv.lt.85d0) then
          xbin = 69.5d0
        elseif (ptv.lt.105d0) then
          xbin = 70.5d0
        elseif (ptv.lt.150d0) then
          xbin = 71.5d0
        elseif (ptv.lt.200d0) then
          xbin = 72.5d0
        elseif (ptv.lt.900d0) then
          xbin = 73.5d0
        else
          write (*,*) "error: event outside bins", ptv
          stop 1
        endif
      else
        write (*,*) "error: event outside bins", xmll
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
