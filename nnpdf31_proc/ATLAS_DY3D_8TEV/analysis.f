cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call set_error_estimation(1)
      call HwU_inithist(nwgt,weights_info)
c     each HwU contains a complete invariant mass slice, with 12 rapidities bins, and 6 bins of the
c     CS angle for each rapidity bin
      call HwU_book(1,'dist', 12*6,  46d0*2.4d0*2d0,  66d0*2.4d0*2d0)
      call HwU_book(2,'dist', 12*6,  66d0*2.4d0*2d0,  80d0*2.4d0*2d0)
      call HwU_book(3,'dist', 12*6,  80d0*2.4d0*2d0,  91d0*2.4d0*2d0)
      call HwU_book(4,'dist', 12*6,  91d0*2.4d0*2d0, 102d0*2.4d0*2d0)
      call HwU_book(5,'dist', 12*6, 102d0*2.4d0*2d0, 116d0*2.4d0*2d0)
      call HwU_book(6,'dist', 12*6, 116d0*2.4d0*2d0, 150d0*2.4d0*2d0)
      call HwU_book(7,'dist', 12*6, 150d0*2.4d0*2d0, 200d0*2.4d0*2d0)

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
      external getinvm
      external getabsy
      external getcostheta
      integer bin,xcosbin
      double precision minmll, maxmll, mincos, maxcos

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
      xcos=getcostheta(ppl(0),ppl(1),ppl(2),ppl(3),pplb(0),pplb(1),
     &                 pplb(2),pplb(3))

      bin = -1

      if (xmll.ge.46d0.and.xmll.lt.66d0) then
        bin=1
        minmll=46d0
        maxmll=66d0
      elseif (xmll.ge.66d0.and.xmll.lt.80d0) then
        bin=2
        minmll=66d0
        maxmll=80d0
      elseif (xmll.ge.80d0.and.xmll.lt.91d0) then
        bin=3
        minmll=80d0
        maxmll=91d0
      elseif (xmll.ge.91d0.and.xmll.lt.102d0) then
        bin=4
        minmll=91d0
        maxmll=102d0
      elseif (xmll.ge.102d0.and.xmll.lt.116d0) then
        bin=5
        minmll=102d0
        maxmll=116d0
      elseif (xmll.ge.116d0.and.xmll.lt.150d0) then
        bin=6
        minmll=116d0
        maxmll=150d0
      elseif (xmll.ge.150d0.and.xmll.lt.200d0) then
        bin=7
        minmll=150d0
        maxmll=200d0
      endif

c     add 1d0 because we want to remap the cosine to [0, 2]
      if (xcos.lt.-0.7d0) then
        mincos = -1.0d0 + 1d0
        maxcos = -0.7d0 + 1d0
      elseif (xcos.ge.-0.7d0.and.xcos.lt.-0.4d0) then
        mincos = -0.7d0 + 1d0
        maxcos = -0.4d0 + 1d0
      elseif (xcos.ge.-0.4d0.and.xcos.lt.0.0d0) then
        mincos = -0.4d0 + 1d0
        maxcos =  0.0d0 + 1d0
      elseif (xcos.ge.0.0d0.and.xcos.lt.0.4d0) then
        mincos =  0.0d0 + 1d0
        maxcos =  0.4d0 + 1d0
      elseif (xcos.ge.0.4d0.and.xcos.lt.0.7d0) then
        mincos =  0.4d0 + 1d0
        maxcos =  0.7d0 + 1d0
      else
        mincos =  0.7d0 + 1d0
        maxcos =  1.0d0 + 1d0
      endif

      call HwU_fill(bin,minmll*2d0*2.4d0+(maxmll-minmll)*(2.4d0*
     &                  mincos+xyll*(maxcos-mincos)),wgts)


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
