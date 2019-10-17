// emacs: this is -*- c++ -*-
//
//   sparse.h        
//   
//   define 1, 2 and 3d block sparse matrices for doubles                  
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: sparse.h, v1.0   Thu Nov 22 15:46:00 GMT 2007 sutt


#ifndef __SPARSE_H
#define __SPARSE_H


#include "tsparse3d.h"

typedef tsparse1d<double> sparse1d;
typedef tsparse2d<double> sparse2d;
typedef tsparse3d<double> sparse3d;


#endif  // __SPARSE_H 










