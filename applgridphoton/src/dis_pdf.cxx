// emacs: this is -*- c++ -*-
//
//   dis_pdf.cxx        
//
//   pdf transform function for nlojet                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: dis_pdf.cxx, v1.0   Sat 10 11:27:04 GMT 2009 sutt $



#include "appl_grid/appl_pdf.h" 

#include "dis_pdf.h"


extern "C" void fdis_pdf__(const double* fA, const double* fB, double* H) { 
  static dis_pdf pdf;
  pdf.evaluate(fA, fB, H);
}




