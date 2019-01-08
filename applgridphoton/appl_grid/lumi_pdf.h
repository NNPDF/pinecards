// emacs: this is -*- c++ -*-
//
//   @file    lumi_pdf.h        
//
//            a lumi pdf type - to read in the combinations
//            for the subprocesses from a file
//
//            the file format is as from madgraph nproc rows ...
//
//              index  npairs   p1 p2   p3 p4   p5 p6 ...  
//              index  npairs   ...
//              ...
//
//            where  npairs is the number of parton-parton pairs
//            in this subprocess
//
//  
//   Copyright (C) 2013 M.Sutton (sutt@cern.ch)    
//
//   $Id: lumi_pdf.h, v0.0   Mon 28 Jan 2013 15:41:10 GMT sutt $


#ifndef  LUMI_PDF_H
#define  LUMI_PDF_H

#include <iostream>

#include "appl_grid/appl_pdf.h" 
#include "appl_grid/combination.h"


class lumi_pdf : public appl::appl_pdf {

public:

  lumi_pdf(const std::string& s="", const std::vector<int>& combinations=std::vector<int>() );   // , int Wcharge=0 );

  lumi_pdf(const std::string& s, const std::vector<combination>& combinations, int ckmcharge=0 );             // , int Wcharge=0 );

  virtual ~lumi_pdf() {   } 

  void evaluate(const double* _fA, const double* _fB, double* H);

  /// additional user defined functions to actually initialise 
  /// based on the input file

  /// how many combinations of subprocesses are there ?
  unsigned size() const { return m_combinations.size(); }
 
  //  void initialise(const std::string& filename);

  //  bool initialised() const { return m_initialised; }

  const combination& operator[](int i) const { return m_combinations.at(i); }  

  int  decideSubProcess(const int iflav1, const int iflav2) const ;

  std::vector<int> serialise() const;

  void write(const std::string& filename) const;
 
  void write(std::ostream& s=std::cout) const; 

  //  std::string summary(std::ostream& s=std::cout) const; 
  std::string summary() const; 
  
private:

  /// add a combination
  void add(const combination& c) {  m_combinations.push_back(c); }

  void create_lookup();

private:

  /// this might eventually become a std::string encoding the grid
  std::string m_filename;  

  /// has this been initialised yet?
  //  bool m_initialised;

  /// ckm matrices should they be needed ...
  //std::vector<double>                m_ckmsum;
  //std::vector<std::vector<double> >  m_ckm2;

  std::vector<combination> m_combinations;

  /// flag that this is an amcatnlo pdf
  //  bool m_amcflag;

  /// lookup table for decideSubprocess
  std::vector<std::vector<int> >  m_lookup;

};



inline std::ostream& operator<<( std::ostream& s, const lumi_pdf& _g ) { 
  s << "lumi_pdf:   " << _g.name() << "\t processes " << _g.Nproc() << "\n";
  for ( int i=0 ; i<_g.Nproc() ; i++ ) s << _g[i] << std::endl;
  return s;
}



#endif  // LUMI_PDF_H 












