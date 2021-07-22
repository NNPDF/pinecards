cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call HwU_inithist(nwgt,weights_info)
      call HwU_book(1,'dist', 66,   0d0,  66d0)
      call HwU_book(2,'dist', 66,  66d0, 132d0)
      call HwU_book(3,'dist', 68, 132d0, 200d0)
      call HwU_book(4,'dist', 68, 200d0, 268d0)
      call HwU_book(5,'dist', 66, 268d0, 334d0)
      call HwU_book(6,'dist', 64, 334d0, 398d0)
      call HwU_book(7,'dist', 60, 398d0, 458d0)
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
      double precision xcos, getcostheta
      double precision obs, xlimit
      external getinvm
      external getabsy
      external getcostheta

      double precision p_reco(0:4,nexternal)
      integer iPDG_reco(nexternal),grid



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
      xcos=getcostheta(ppl(0),ppl(1),ppl(2),ppl(3),pplb(0),pplb(1),
     &                 pplb(2),pplb(3))

      grid=-1
      obs=0d0
      xlimit=-1.0d0

      if (xmll.lt.46d0) then
        grid=-2
      elseif (xmll.lt.66d0) then
        grid=1
        obs=0d0
        if (xcos.lt.-0.7d0) then
          obs=obs
          xlimit=2.0d0
        elseif (xcos.lt.-0.4d0) then
          obs=obs+10d0
          xlimit=2.2d0
        elseif (xcos.lt.0.0d0) then
          obs=obs+10d0+11d0
          xlimit=2.4d0
        elseif (xcos.lt.0.4d0) then
          obs=obs+10d0+11d0+12d0
          xlimit=2.4d0
        elseif (xcos.lt.0.7d0) then
          obs=obs+10d0+11d0+12d0+12d0
          xlimit=2.2d0
        else
          obs=obs+10d0+11d0+12d0+12d0+11d0
          xlimit=2.0d0
        endif
      elseif (xmll.lt.80d0) then
        grid=2
        obs=66d0
        if (xcos.lt.-0.7d0) then
          obs=obs
          xlimit=2.0d0
        elseif (xcos.lt.-0.4d0) then
          obs=obs+10d0
          xlimit=2.2d0
        elseif (xcos.lt.0.0d0) then
          obs=obs+10d0+11d0
          xlimit=2.4d0
        elseif (xcos.lt.0.4d0) then
          obs=obs+10d0+11d0+12d0
          xlimit=2.4d0
        elseif (xcos.lt.0.7d0) then
          obs=obs+10d0+11d0+12d0+12d0
          xlimit=2.2d0
        else
          obs=obs+10d0+11d0+12d0+12d0+11d0
          xlimit=2.0d0
        endif
      elseif (xmll.lt.91d0) then
        grid=3
        obs=66d0+66d0
        if (xcos.lt.-0.7d0) then
          obs=obs
          xlimit=2.2d0
        elseif (xcos.lt.-0.4d0) then
          obs=obs+11d0
          xlimit=2.2d0
        elseif (xcos.lt.0.0d0) then
          obs=obs+11d0+11d0
          xlimit=2.4d0
        elseif (xcos.lt.0.4d0) then
          obs=obs+11d0+11d0+12d0
          xlimit=2.4d0
        elseif (xcos.lt.0.7d0) then
          obs=obs+11d0+11d0+12d0+12d0
          xlimit=2.2d0
        else
          obs=obs+11d0+11d0+12d0+12d0+11d0
          xlimit=2.2d0
        endif
      elseif (xmll.lt.102d0) then
        grid=4
        obs=66d0+66d0+68d0
        if (xcos.lt.-0.7d0) then
          obs=obs
          xlimit=2.2d0
        elseif (xcos.lt.-0.4d0) then
          obs=obs+11d0
          xlimit=2.2d0
        elseif (xcos.lt.0.0d0) then
          obs=obs+11d0+11d0
          xlimit=2.4d0
        elseif (xcos.lt.0.4d0) then
          obs=obs+11d0+11d0+12d0
          xlimit=2.4d0
        elseif (xcos.lt.0.7d0) then
          obs=obs+11d0+11d0+12d0+12d0
          xlimit=2.2d0
        else
          obs=obs+11d0+11d0+12d0+12d0+11d0
          xlimit=2.2d0
        endif
      elseif (xmll.lt.116d0) then
        grid=5
        obs=66d0+66d0+68d0+68d0
        if (xcos.lt.-0.7d0) then
          obs=obs
          xlimit=2.0d0
        elseif (xcos.lt.-0.4d0) then
          obs=obs+10d0
          xlimit=2.2d0
        elseif (xcos.lt.0.0d0) then
          obs=obs+10d0+11d0
          xlimit=2.4d0
        elseif (xcos.lt.0.4d0) then
          obs=obs+10d0+11d0+12d0
          xlimit=2.4d0
        elseif (xcos.lt.0.7d0) then
          obs=obs+10d0+11d0+12d0+12d0
          xlimit=2.2d0
        else
          obs=obs+10d0+11d0+12d0+12d0+11d0
          xlimit=2.0d0
        endif
      elseif (xmll.lt.150d0) then
        grid=6
        obs=66d0+66d0+68d0+68d0+66d0
        if (xcos.lt.-0.7d0) then
          obs=obs
          xlimit=1.8d0
        elseif (xcos.lt.-0.4d0) then
          obs=obs+9d0
          xlimit=2.2d0
        elseif (xcos.lt.0.0d0) then
          obs=obs+9d0+11d0
          xlimit=2.4d0
        elseif (xcos.lt.0.4d0) then
          obs=obs+9d0+11d0+12d0
          xlimit=2.4d0
        elseif (xcos.lt.0.7d0) then
          obs=obs+9d0+11d0+12d0+12d0
          xlimit=2.2d0
        else
          obs=obs+9d0+11d0+12d0+12d0+11d0
          xlimit=1.8d0
        endif
      elseif (xmll.lt.200d0) then
        grid=7
        obs=66d0+66d0+68d0+68d0+66d0+64d0
        if (xcos.lt.-0.7d0) then
          obs=obs
          xlimit=1.6d0
        elseif (xcos.lt.-0.4d0) then
          obs=obs+8d0
          xlimit=2.0d0
        elseif (xcos.lt.0.0d0) then
          obs=obs+8d0+10d0
          xlimit=2.4d0
        elseif (xcos.lt.0.4d0) then
          obs=obs+8d0+10d0+12d0
          xlimit=2.4d0
        elseif (xcos.lt.0.7d0) then
          obs=obs+8d0+10d0+12d0+12d0
          xlimit=2.0d0
        else
          obs=obs+8d0+10d0+12d0+12d0+10d0
          xlimit=1.6d0
        endif
      else
        grid=-3
      endif

      if (xyll.gt.xlimit) then
        write (*,*) "error: event outside bins", grid, obs, xyll, xlimit
        stop 1
      endif

      obs=obs+floor(xyll/0.2d0)

      if (grid.gt.0d0) then
        call HwU_fill(grid,obs+0.5d0,wgts)
      else
        write (*,*) "error: event outside bins", grid, obs
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

      function getcostheta(enl,ptxl,ptyl,pzl,enal,ptxal,ptyal,pzal)
      implicit none
      real*8 getcostheta,enl,ptxl,ptyl,pzl,enal,ptxal,ptyal,pzal
      real*8 pzll,mll,p1p,p1m,p2p,p2m,pt2ll
c     implementation of first formula on page 6 of https://arxiv.org/abs/1710.05167
      p1p = enl + pzl
      p1m = enl - pzl
      p2p = enal + pzal
      p2m = enal - pzal
      pzll = pzl + pzal
      pt2ll = (ptxl + ptxal) * (ptxl + ptxal) +
     &        (ptyl + ptyal) * (ptyl + ptyal)
      mll = sqrt((enl + enal) * (enl + enal) - (pt2ll + pzll * pzll))
      getcostheta = sign((p1p*p2m-p1m*p2p)/sqrt(mll*mll+pt2ll)/mll,pzll)

      if (abs(getcostheta).gt.1d0) then
        write(*,*) 'Calculating cos with cos > 1'
        stop
      endif
      return
      end
