// emacs: this is -*- c++ -*-
//
//   @file    amcfast_interface.h        
//
//                   
//  
//   Copyright (C) 2013 M.Sutton (sutt@cern.ch)    
//
//   $Id: amcfast_interface.h, v0.0   Thu  4 Jul 2013 10:25:18 CEST sutt $

#ifndef  AMCFAST_INTERFACE_H
#define  AMCFAST_INTERFACE_H

namespace amcfast {
  void init();
  void fill();
  void fill_ref();
  void fill_ref_out();
  void reco();
  void term();
}

#endif  // AMCFAST_INTERFACE_H 
