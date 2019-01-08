// emacs: this is -*- c++ -*-
//
//   jetrad_pdf.cxx        
//
//   pdf transform function for jetrad                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: jetrad_pdf.cxx, v1.0   Mon Dec 10 01:36:04 GMT 2007 sutt $



#include "appl_grid/appl_pdf.h"

#include "jetrad_pdf.h"


// fortran callable wrapper
extern "C" void fjetrad_pdf__(const double* fA, const double* fB, double* H) { 
  static jetrad_pdf pdf;
  pdf.evaluate(fA, fB, H);
}
