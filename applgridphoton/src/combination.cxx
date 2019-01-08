//
//   @file    combination.cxx         
//
//            this is a class which allows the sum of a std::vector of 
//            pairs of parton-parton initial states to be calculated  
//
//   @author M.Sutton
// 
//   Copyright (C) 2013 M.Sutton (sutt@cern.ch)    
//
//   $Id: combination.cxx, v0.0   Thu  4 Jul 2013 23:20:23 CEST sutt $


/// automatically keeps count of how many subprocess combinations 
/// there have been 
 
/// read each of the pairs to be summed for a specific 
/// subprocess combination 


#include "appl_grid/appl_pdf.h"
#include "appl_grid/combination.h"


combination::combination(const std::vector<int>& v) { 
  //  std::cout << "combination::combination() std::vector" << std::endl;
  construct(v); 
}


combination::combination(const std::string& line) : m_index(-1), m_size(0) { 

  //  std::cout << "combination::combination() std::string " << line << std::endl;

  std::istringstream iss(line);
  
  std::vector<int> v;
  v.reserve(11);
  
  int i;
  
  while (iss >> i) v.push_back(i);

  construct(v);
}



void combination::construct(const std::vector<int>& v) { // : m_index(v.at(0)), m_size(v.at(1)) { 

  //  std::cout << "combination::construct()" << std::endl;

  if ( v.size()==0 ) return; 

  //  if ( v.size()<4 || v.size()%2!=0 ) throw appl::appl_pdf::exception("not enough entries for combination");

  m_index = v[0];
  m_size  = v[1];

  for ( unsigned i=2 ; i<v.size() ; i+=2 ) { 
    m_pairs.push_back( cpair( remap(v[i]), remap(v[i+1]) ) ); 
  }

  //  if ( m_size!=m_pairs.size() ) throw appl::appl_pdf::exception("mismatch in entries for this for combination");

}



/// evaluate the sum over all the (paired) subprocesses

double combination::evaluate( const double* xfA, const double* xfB,  
			      const std::vector<double>& _ckmsum, 
			      const std::vector<std::vector<double> >& _ckm2 ) const {  

  xfA += 6;  /// offset the pointer for xf1 and xf2 so can use 
  xfB += 6;  /// lhapdf code for indexing

  double H = 0; 

  if ( _ckmsum.size()==0 ) {
    /// without using the ckm matrix 
    for ( unsigned i=size() ; i-- ; ) { 
      H += xfA[m_pairs[i].first]*xfB[m_pairs[i].second]; 
    }
  }
  else {
    /// using the ckm matrix 
    for ( unsigned i=size() ; i-- ; ) { 
      if ( m_pairs[i].first!=0  ) {
	if ( m_pairs[i].second!=0  )  H += xfA[m_pairs[i].first]*xfB[m_pairs[i].second]*_ckm2[m_pairs[i].first+6][m_pairs[i].second+6]; //q-q 
	else 	                      H += xfA[m_pairs[i].first]*xfB[m_pairs[i].second]*_ckmsum[m_pairs[i].first+6]; /// q-gluon
      }
      else {
	if ( m_pairs[i].second!=0  )  H += xfA[m_pairs[i].first]*xfB[m_pairs[i].second]*_ckmsum[m_pairs[i].second+6]; // gluon-q 
	else                          H += xfA[m_pairs[i].first]*xfB[m_pairs[i].second]; /// gluon-gluon
      }
    }  /// looop over pairs 
  } /// use ckm matrix 
  
  return H;
}

