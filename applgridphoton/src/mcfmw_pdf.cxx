// emacs: this is -*- c++ -*-
//
//   mcfmw_pdf.cxx        
//
//   pdf transform functions                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: mcfmw_pdf.cxx, v1.0   Mon Dec 10 01:36:04 GMT 2007 sutt $



#include "appl_grid/appl_pdf.h" 

#include "mcfmw_pdf.h"

// fortran callable wrapper

extern "C" void fmcfmwp_pdf__(const double* fA, const double* fB, double* H) { 
  static mcfmwp_pdf pdf;
  pdf.evaluate(fA, fB, H);
}


// fortran callable wrapper

extern "C" void fmcfmwm_pdf__(const double* fA, const double* fB, double* H) { 
  static mcfmwm_pdf pdf;
  pdf.evaluate(fA, fB, H);
}




