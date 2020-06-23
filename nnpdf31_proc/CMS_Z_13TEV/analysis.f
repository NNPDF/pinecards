cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine analysis_begin(nwgt,weights_info)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      implicit none
      integer nwgt
      character*(*) weights_info(*)

      call set_error_estimation(1)
      call HwU_inithist(nwgt,weights_info)
      call HwU_book( 1,'Z pT', 14,   0.5d0,  13.5d0)
      call HwU_book( 2,'Z pT',  1,  13.5d0,  15.0d0)
      call HwU_book( 3,'Z pT',  4,  15.0d0,  21.0d0)
      call HwU_book( 4,'Z pT',  1,  21.0d0,  23.5d0)
      call HwU_book( 5,'Z pT',  1,  23.5d0,  26.5d0)
      call HwU_book( 6,'Z pT',  1,  26.5d0,  30.0d0)
      call HwU_book( 7,'Z pT',  1,  30.0d0,  34.5d0)
      call HwU_book( 8,'Z pT',  1,  34.5d0,  40.0d0)
      call HwU_book( 9,'Z pT',  1,  40.0d0,  47.5d0)
      call HwU_book(10,'Z pT',  1,  47.5d0,  58.5d0)
      call HwU_book(11,'Z pT',  1,  58.5d0,  75.0d0)
      call HwU_book(12,'Z pT',  1,  75.0d0, 102.5d0)
      call HwU_book(13,'Z pT',  1, 102.5d0, 140.0d0)
      call HwU_book(14,'Z pT',  1, 140.0d0, 175.0d0)
      call HwU_book(15,'Z pT',  2, 175.0d0, 235.0d0)
      call HwU_book(16,'Z pT',  1, 235.0d0, 275.0d0)
      call HwU_book(17,'Z pT',  1, 275.0d0, 350.0d0)
      call HwU_book(18,'Z pT',  1, 350.0d0, 450.0d0)
      call HwU_book(19,'Z pT',  1, 450.0d0, 650.0d0)
      call HwU_book(20,'Z pT',  1, 650.0d0,1150.0d0)
      
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
      double precision ppl(0:3), pplb(0:3), ppv(0:3), ptv, getinvm
      external getinvm

      double precision p_reco(0:4,nexternal)
      integer iPDG_reco(nexternal)



      call recombine_momenta(rphreco, etaphreco, lepphreco, quarkphreco,
     $                       p, iPDG, p_reco, iPDG_reco)

      do j = nincoming+1, nexternal
        if (iPDG_reco(j).eq.11) ppl(0:3)=p_reco(0:3,j)
        if (iPDG_reco(j).eq.-11) pplb(0:3)=p_reco(0:3,j)
      enddo
      do i=0,3
        ppv(i)=ppl(i)+pplb(i)
      enddo
      
      ptv=sqrt(ppv(1)**2+ppv(2)**2)

      call HwU_fill(1,ptv,wgts)
      call HwU_fill(2,ptv,wgts)
      call HwU_fill(3,ptv,wgts)
      call HwU_fill(4,ptv,wgts)
      call HwU_fill(5,ptv,wgts)
      call HwU_fill(6,ptv,wgts)
      call HwU_fill(7,ptv,wgts)
      call HwU_fill(8,ptv,wgts)
      call HwU_fill(9,ptv,wgts)
      call HwU_fill(10,ptv,wgts)
      call HwU_fill(11,ptv,wgts)
      call HwU_fill(12,ptv,wgts)
      call HwU_fill(13,ptv,wgts)
      call HwU_fill(14,ptv,wgts)
      call HwU_fill(15,ptv,wgts)
      call HwU_fill(16,ptv,wgts)
      call HwU_fill(17,ptv,wgts)
      call HwU_fill(18,ptv,wgts)
      call HwU_fill(19,ptv,wgts)
      call HwU_fill(20,ptv,wgts)

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

