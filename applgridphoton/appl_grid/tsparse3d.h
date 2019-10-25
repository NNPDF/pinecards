// emacs: this is -*- c++ -*-
//
//   tsparse3d.h        
//
//   very basic 3d sparse matrix class, needs to be improved, 
//   and extended to 3d                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: tsparse3d.h, v   Fri Nov  9 00:17:31 CET 2007 sutt


#ifndef __TSPARSE3D_H
#define __TSPARSE3D_H

#include <iostream>

#include "tsparse2d.h"


template<class T> 
class tsparse3d : public tsparse_base {

public:

  tsparse3d(int nx, int ny, int nz);

  tsparse3d(const tsparse3d& t);

  ~tsparse3d();

  // accessors
  int Ny() const { return m_Ny; } 
  int Nz() const { return m_Nz; } 



  void trim();

  void untrim();

  bool trimmed() const { return m_trimmed; }

  bool trimmed(int i, int j, int k) const;

  T operator()(int i, int j, int k) const;

  const tsparse2d<T>* operator[](int i) const;

  T& operator()(int i, int j, int k);

  int size() const;

  void print() const;

  int ymin();

  int ymax();

  int zmin();

  int zmax();


  // algebraic operators

  tsparse3d& operator=(const tsparse3d& t);

  tsparse3d& operator*=(const double& d);

  tsparse3d& operator+=(const tsparse3d& t) {
    m_trimmed = false;
    if ( Nx()!=t.Nx() || Ny()!=t.Ny() || Nz()!=t.Nz() ) throw out_of_range("bin mismatch");
    for ( int i=0 ; i<Nx() ; i++ ) {
      for ( int j=0 ; j<Ny() ; j++ ) {
	for ( int k=0 ; k<Nz() ; k++ ) (*this)(i,j,k) += t(i,j,k);
      }
    }
    return *this;
  }

private:

  void grow(int i);

protected:
  
  int m_Ny;
  int m_Nz;
  
  tsparse2d<T>**  m_v;
  
  //  T** m_fastindex;
  
  bool m_trimmed;

};


// stream IO template
template<class T> std::ostream& operator<<(std::ostream& s, const tsparse3d<T>& sp) { 
  for ( int i=0 ; i<sp.Nx() ; i++ ) { 
    for ( int j=0 ; j<sp.Ny() ; j++ ) { 
      for ( int k=0 ; k<sp.Nz() ; k++ ) { 
	s << sp(i,j,k) << "\t"; 
      }
      s << "\n";
    }
    s << "\n";
  }
  return s;
}


#endif  // __TSPARSE3D_H 










