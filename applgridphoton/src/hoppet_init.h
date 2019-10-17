// emacs: this is -*- c++ -*-
//
//   @file    hoppet_init.h        
//
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@cern.ch)    
//
//   $Id: hoppet_init.h, v0.0   Sat 26 Sep 2009 13:24:26 BST sutt $


#ifndef __HOPPETINIT_H
#define __HOPPETINIT_H

#include <iostream>
#include <vector>



class hoppet_init : public std::vector<double> {

public:

  /// default maximum scale for hoppet initialisation
  hoppet_init( double Qmax=15000 );

  virtual ~hoppet_init();

  void fillCache( void (*pdf)(const double&, const double&, double* )  );
  
  bool compareCache( void (*pdf)(const double&, const double&, double* )  );

  static void assign( void (*pdf)(const double&, const double&, double* )  );

};



inline std::ostream& operator<<( std::ostream& s, const hoppet_init& _h ) { 
  return s << "hoppet_init size: " << _h.size();
}


#endif  // __HOPPETINIT_H 










