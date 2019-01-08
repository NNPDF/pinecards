// emacs: this is -*- c++ -*-
//
//   @file    Cache.h        
//            evaluate a pdf function and store values in a cache.
//            if this x, Q2 node has been requested before then 
//            retrieve the values and return them if not, generate 
//            and then add to the cache and then retiurn them
//
//            the new LHAPDF (version 6) may implement something 
//            like this internally, but a retieval from the cache 
//            is still around 20-40 times faster than LHAPDF 6                   
//  
//   Copyright (C) 2013 M.Sutton (sutt@cern.ch)    
//
//   $Id: Cache.h, v0.0   Sat 19 Oct 2013 14:12:42 CEST sutt $


#pragma  once

#include <iostream>
#include <map>
#include <utility>

/// Pass an LHAPDF (version 5) function pointer into the cache, 
/// then call using the evaluate() method instead of the calling 
/// the pdf directly. 

template<typename T>
class Cache : public std::map<T, std::vector<double> > {
 
private: 

  /// actual type def of the map for convenience
  typedef std::map<T,std::vector<double> > _map;

  /// function pointer type
  typedef void (*pdffunction)(const double& , const double&, double* );

  /// for fast copy 
  struct partons { double p[14]; };

public:

  /// give it the pdf function to use
  Cache( pdffunction pdf=0, unsigned mx=20000 ) : _pdf(pdf), _max(mx), _ncalls(0), _ncached(0), _reset(0), _disabled(false), _printstats(false) { } 

  virtual ~Cache() { } 

  
  /// evaluate the pdf function
  /// if this node has been requested before, retrieve values 
  /// from the cache, if not, generate and then add to cache
  /// for next time 
  void evaluate( const double& x, const double& Q2, double*  xf ) { 
    
    if ( _pdf==0 ) { 
      /// should really throw an exception here
      std::cerr << "whoops, pdf cache has no pdf!!" << std::endl; 
      return; 
    }

    _ncalls++;

    /// if we don't want to use the cache for some reason (it can get quite large)     
    if ( _disabled ) return _pdf( x, Q2, xf ); 

    //    _reset++;

    T t(x, Q2);

    /// find out if this x, Q2 node is in the cache ...
       
    typename _map::const_iterator itr = this->find( t );
    
    if ( itr!=this->end() ) { 
      /// in the cache, simply copy to output ...
      (*(partons*)xf) = (*(partons*)(&itr->second[0])); 
      _ncached++;
    } 
    else { 
      /// not in cache, call pdf function 
      static std::vector<double> _xf(14);

      _pdf( x, Q2, &_xf[0] ); 

      /// add to cache if enough room
      if ( this->size()<_max ) this->insert( typename _map::value_type( t, _xf ) );

      /// copy to output 
      (*(partons*)xf) = ( *((partons*)&_xf[0]) ); 
    }
  }
  



  /// print some useful stats
  void stats() const { if (_printstats) std::cout << *this << std::endl; }

  /// return the actual stored pdf - very handy 
  pdffunction pdf() { return _pdf; }

  unsigned max()        const { return _max; } 
  unsigned ncalls()     const { return _ncalls; } 
  unsigned ncached()    const { return _ncached; } 
  unsigned ngenerated() const { return _ncalls-_ncached; } 

  double   fraction()  const { 
    if ( _ncalls>0 ) return this->size()*1.0/_ncalls; 
    return 0;
  }

  void reset() { this->clear(); _reset=_ncalls=_ncached=0; }

  void disable() { _disabled = true; }
  void enable()  { _disabled = false; }


private:

  //  void (*_pdf)(const double& , const double&, double* );
  pdffunction _pdf;

  /// maximum cache size
  unsigned _max;

  /// some stats - how many nodes generated and how many from the cache
  unsigned _ncalls;
  unsigned _ncached;
  unsigned _ngenerated;

  unsigned _reset;

  bool     _disabled;

  bool     _printstats;

};




template<typename T>
inline std::ostream& operator<<( std::ostream& s, const Cache<T>& _c ) { 
  s << "Cache:: " 
    << "\tgenerated "  << _c.ngenerated() 
    << "\tfrom cache " << _c.ncached() 
    << "\tsize "       << _c.size() <<  " ( " << int(_c.fraction()*1000)*0.1 << "% )\t"
    << " :maximum "    << _c.max();
    return s;
}

/// useful typdef
typedef Cache<std::pair<double,double> > NodeCache;









