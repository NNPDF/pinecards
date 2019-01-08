// emacs: this is -*- c++ -*-
//
//   mcfmzjet_pdf.cxx        
//
//   pdf transform functions for z production                  
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: mcfmz_pdf.cxx, v1.0   Mon Dec 10 01:36:04 GMT 2007 sutt $



#include "appl_grid/appl_pdf.h" 

#include "mcfmzjet_pdf.h"


// fortran callable wrapper
extern "C" void fmcfmzjet_pdf__(const double* fA, const double* fB, double* H) { 
  static mcfmzjet_pdf pdf;
  pdf.evaluate(fA, fB, H);
}

