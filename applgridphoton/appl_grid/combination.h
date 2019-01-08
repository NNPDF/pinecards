// emacs: this is -*- c++ -*-
//
//   @file    combination.h        
//
//                   
//  
//   Copyright (C) 2013 M.Sutton (sutt@cern.ch)    
//
//   $Id: combination.h, v0.0   Thu  4 Jul 2013 23:20:40 CEST sutt $


#ifndef  COMBINATION_H
#define  COMBINATION_H

#include <iostream>


#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <vector>
#include <utility>



class combination { 

public:

  typedef std::pair<int,int> cpair;

public:

  /// constructors and destructor
  combination(const std::vector<int>& v);

  combination(const std::string& line);

  virtual ~combination() {}

  /// evaluate the actual combination

  double evaluate( const double* xfA, const double* xfB,
		   const std::vector<double>& ckmsum=std::vector<double>(), 
		   const std::vector<std::vector<double> >& ckm2=std::vector<std::vector<double> >()  ) const;  

  /// number of pairs
  unsigned size() const { return m_pairs.size(); }  

  /// do any of these remaining need to be public?

  /// index
  int    index()     const { return m_index; }

  /// accessors to the pairs themselves
  cpair  pair(int i) const { return m_pairs[i]; }

  const std::vector<cpair>& pairs() const { return m_pairs; }

  cpair&       operator[](int i)       { return m_pairs[i]; } 
  const cpair& operator[](int i) const { return m_pairs[i]; } 

private:

  /// actually construct combination from the pair list
  void construct(const std::vector<int>& v);

  /// remap from the pdg to lhapdf code
  int remap(int i) 
  {
    if (i == 21) return 0;
    else if (i == 22) return 7;
    else return i;		   
  } 

private:

  int                m_index;
  unsigned           m_size;
  
  std::vector<cpair> m_pairs;

  static int         gindex;
  
};




inline std::ostream& operator<<( std::ostream& s, const combination& _c ) { 
  s << "[ " << _c.index() << " : ";
  if ( _c.size()>10 ) { 
    for ( int i=0 ; i<4 ; i++ ) s << "\t(" << _c[i].first << ", " << _c[i].second << ")";
    s << "\t  ... ";
    for ( unsigned i=_c.size()-3 ; i<_c.size() ; i++ ) s << "\t(" << _c[i].first << ", " << _c[i].second << ")";
  }
  else { 
    for ( unsigned i=0 ; i<_c.size() ; i++ ) s << "\t(" << _c[i].first << ", " << _c[i].second << ")";
  }
  s << " ]";
  return s;
}



#endif  // COMBINATION_H 










