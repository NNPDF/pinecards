
// emacs: this is -*- c++ -*-
//
//   Sparse3d.h        
//
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: Sparse3d.h, v   Thu Nov 29 19:28:51 CET 2007 sutt


#ifndef __SPARSE3D_H
#define __SPARSE3D_H

#include <iostream>


#include "TMatrixDSparse.h"
#include "tsparse1d.h"
#include "axis.h"


class Sparse3d : public tsparse_base {

public:

  Sparse3d(int nx, double lx, double ux, 
	   int ny, double ly, double uy, 
	   int nz, double lz, double uz) :
    tsparse_base(nx), 
    mv(m_Nx), 
    mxaxis(nx, lx, ux), 
    myaxis(ny, ly, uy), 
    mzaxis(nz, lz, uz) { 
    
    for ( int i=0 ; i<m_Nx ; i++ ) { 
      mv(i)=new TMatrixDSparse(ny, nz);
    }
  }


  ~Sparse3d() { 
    for ( int i=m_ux-m_lx+1 ; i-- ; ) delete mv(i);
  }

  double& operator()(int i, int j, int k) { 
    return (*mv(i))(j,k);
  }

  double operator()(int i, int j, int k) const { 
    return (*mv(i))(j,k);
  }

private:
  
  tsparse1d<TMatrixDSparse*> mv;
  
  axis<double> mxaxis;
  axis<double> myaxis;
  axis<double> mzaxis;

};


#endif  // __SPARSE3D_H 










