// emacs: this is -*- c++ -*-
//
//   mcfmz_pdf.cxx        
//
//   pdf transform functions for z production                  
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: mcfmz_pdf.cxx, v1.0   Mon Dec 10 01:36:04 GMT 2007 sutt $



#include "appl_grid/appl_pdf.h" 

#include "mcfmz_pdf.h"


// fortran callable wrapper
extern "C" void fmcfmz_pdf__(const double* fA, const double* fB, double* H) { 
  static mcfmz_pdf pdf;
  pdf.evaluate(fA, fB, H);
}

