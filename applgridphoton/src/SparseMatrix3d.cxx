//   SparseMatrix3d.cxx        
//
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: SparseMatrix3d.cxx, v1.0   Fri Nov 23 00:49:26 GMT 2007 sutt


#include "appl_grid/SparseMatrix3d.h"
#include "appl_grid/tsparse2d.h"

#include "TH1D.h"


SparseMatrix3d::SparseMatrix3d( int Nx, double lx, double ux, 
				int Ny, double ly, double uy, 
				int Nz, double lz, double uz) :
  tsparse3d<double>(Nx, Ny, Nz),
  m_xaxis(Nx, lx, ux),
  m_yaxis(Ny, ly, uy),
  m_zaxis(Nz, lz, uz) {
} 


SparseMatrix3d::SparseMatrix3d(const SparseMatrix3d& s) : 
  tsparse3d<double>(s),
  m_xaxis(s.m_xaxis),
  m_yaxis(s.m_yaxis),
  m_zaxis(s.m_zaxis) {
} 

void SparseMatrix3d::fill(double x, double y, double z, double w) {

  int i=xaxis().bin(x);
  int j=yaxis().bin(y);
  int k=zaxis().bin(z);

  if ( i<0 || i>=Nx() || j<0 || j>=Ny() || k<0 || k>=Nz() ) return;

  (*this)(i,j,k) += w;
}

/// check if the actual contents are the same
bool SparseMatrix3d::operator==(const SparseMatrix3d& s) const { 

  /// first if the axes are different, cannot be the same
  if ( !compare_axes( s ) ) return false;

  int nvalue = 0;

  /// now do a deep comparison ...
  for ( int i=0 ; i<m_xaxis.N() ; i++ ) { 
    for ( int j=0 ; j<m_yaxis.N() ; j++ ) { 
      for ( int k=0 ; k<m_zaxis.N() ; k++ ) { 

	//	std::cout << i << " " << j << " " << k
	//		  << " : " << (*this)(i,j,k) 
	//		  << " " << s(i,j,k)  << "  : " << ((*this)(i,j,k)-s(i,j,k) ) << std::endl;

	if ( (*this)(i,j,k) != s(i,j,k) ) return false;
	if ( (*this)(i,j,k)!=0 ) nvalue++;
      }  
    }  
  }

  return true;
}
