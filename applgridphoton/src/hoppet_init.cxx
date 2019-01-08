//
//   @file    hoppet_init.cxx         
//   
//
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@cern.ch)    
//
//   $Id: hoppet_init.cxx, v0.0   Sat 26 Sep 2009 13:24:31 BST sutt $

#include "hoppet_init.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>

#include "amconfig.h"
#ifdef HAVE_HOPPET
#include "hoppet_v1.h"
#endif


hoppet_init::hoppet_init( double Qmax ) {

  double dy = 0.1;   
  int nloop = 2;         

  std::cout << "hoppet_init::hoppet_init()  dy = " << dy << "\tnloop = " << nloop << "\tQmax " << Qmax << std::endl; 
  // the internal grid spacing (smaller->higher accuarcy)
  // 0.1 should provide at least 10^{-3} accuracy
  // the number of loops to initialise (max=3!)  

#ifdef HAVE_HOPPET
  //  hoppetstart_( dy, nloop );
  /// NB: these parameters (except Qmax) should all be the default
  ///     parameters from hoppet 
  int MSbar = 1;
  // NNPDF modification, ymax from 12 to 13 to support some of our applgrids
  hoppetstartextended_( 13.0, dy, 1.0, Qmax, 0.25*dy, nloop, -6,  MSbar );
#else
  std::cerr << " hoppet_init::hoppet_init() called but hoppet not included" << std::endl;
  exit(0);
#endif
}


hoppet_init::~hoppet_init() {
  //  std::cout << "hoppet_init::~hoppet_init()" << std::endl;
}


void hoppet_init::fillCache( void (*pdf)(const double&, const double&, double* )  ) {

  //  fill the cache

  //  hoppetassign_( pdf );

  //  std::cout << "hoppet_init::fillCache()" << std::endl; 

  clear();

  for ( double lQ=1 ; lQ<=4 ; lQ+=2 ) { 
    double Q = std::pow(10,lQ);
    for ( double lx=-5 ; lx<0 ; lx+=1 ) { 
      double x = std::pow(10,lx);
      double xf[13];
      pdf( x, Q, xf ); 
      for ( int i=0 ; i<13 ; i++ ) push_back( xf[i] ); 

      // for ( int i=0 ; i<13 ; i++ ) std::cout << "\t" << xf[i];
      // std::cout << std::endl;

      //      if ( lQ==2 || lQ==5 ) { 
      //	std::cout << Q << "\t" << x << "\t\t";
      //	for ( int i=0 ; i<13 ; i++ ) std::cout << xf[i] << "\t";
      //	std::cout << std::endl;
      //      }

    }
  }  

}



bool hoppet_init::compareCache( void (*pdf)(const double&, const double&, double* )  ) {
 
  // fill a seperate cache for comparison

  // flag whether the cache has changed
  bool changed = false;

  //  std::cout << "hoppet_init::compareCache()" << std::endl; 


  // is the cahche empty?
  if ( size()==0 ) {
#     ifdef HAVE_HOPPET 
      hoppetassign_( pdf );
#     endif
      fillCache( pdf );
      return changed = true;  
  }

  // fill the new cache for comparison
  //  std::cout << "ccache.size() = " << ccache.size() << "\tsize() = " << size() << std::endl;  

  std::vector<double> ccache;
    
  for ( double lQ=1 ; lQ<=4 ; lQ+=2 ) { 
    double Q = std::pow(10,lQ);
    for ( double lx=-5 ; lx<0 ; lx+=1 ) { 
      double x = std::pow(10,lx);
      double xf[13];
      pdf( x, Q, xf ); 
      for ( int i=0 ; i<13 ; i++ ) ccache.push_back( xf[i] ); 
      
      //      if ( lQ==2 || lQ==5 ) { 
      //	std::cout << Q << "\t" << x << "\t\t";
      //	for ( int i=0 ; i<13 ; i++ ) std::cout << xf[i] << "\t";
      //	std::cout << std::endl;
      //      }

    }
  }
  

  // now compare with existing cache
  
  //  std::cout << "ccache.size() = " << ccache.size() << "\tsize() = " << size() << std::endl;  
  
  if ( ccache.size()!= size() ) changed = true;  

  for ( unsigned i=0 ; i<ccache.size() ; i++ ) if ( ccache[i]!=at(i) ) changed = true;
  
  // cache (and hence the pdf) has changed, so replace the old cached 
  // values with the new and reinitialise hoppet with this new pdf 
  if ( changed ) { 
    
    assign( pdf );
    (*(std::vector<double>*)this) = ccache;
  }
  
  //  std::cout << "hoppet_init::compareCache() changed = " << changed << std::endl; 

  return changed;
  
}



void hoppet_init::assign( void (*pdf)(const double&, const double&, double* )  ) { 
  //  std::cout << "hoppet_init::assign()" << std::endl; 
#   ifdef HAVE_HOPPET 
    hoppetassign_( pdf );
#   endif
}
