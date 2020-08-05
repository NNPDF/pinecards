// emacs: this is -*- c++ -*-
//
//   SparseMatrix3d.h        
//
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: SparseMatrix3d.h, v1.0   Wed Nov 14 14:23:49 GMT 2007 sutt


#ifndef __SPARSEMATRIX3D_H
#define __SPARSEMATRIX3D_H

#include "axis.h"
#include "tsparse3d.h"

class SparseMatrix3d : public tsparse3d<double> {

public:

  // constructors and destructor

  SparseMatrix3d( int Nx, double lx, double ux, 
		  int Ny, double ly, double uy, 
		  int Nz, double lz, double uz);

  SparseMatrix3d(const SparseMatrix3d& s); 

  // axis accessors
  const axis<double>& xaxis() const { return m_xaxis; } 
  const axis<double>& yaxis() const { return m_yaxis; } 
  const axis<double>& zaxis() const { return m_zaxis; } 


  // trim to sparse structure 
  void trim() { tsparse3d<double>::trim(); }

  void fill(double x, double y, double z, double w);
  
  /// check if the axes are all the same
  bool compare_axes(const SparseMatrix3d& s) const { 

    return ( m_xaxis == s.m_xaxis &&  
	     m_yaxis == s.m_yaxis &&
	     m_zaxis == s.m_zaxis );
   
  }


  /// checks if the actual contents are the same
  bool operator==(const SparseMatrix3d& s) const;

  bool operator!=(const SparseMatrix3d& s) const { 
    return !( *this == s );
  }


private:
  axis<double> m_xaxis;
  axis<double> m_yaxis;
  axis<double> m_zaxis;
};

#endif  // __SPARSEMATRIX3D_H 










