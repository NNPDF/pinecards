//
//   TFileString.cxx        
//
//    root TObject std::string std::vector class for writing std::string std::vectors
//    to root files               
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: TFileString.cxx, v0.0   Sat Mar 15 19:50:15 GMT 2008 sutt



#include "TFileString.h"

ClassImp(TFileString)


std::ostream& operator<<(std::ostream& s, const TFileString& fs) { 
  s << fs.name() << ":";
  for ( unsigned i=0 ; i<fs.size() ; i++ ) s << "\t" << fs[i];
  return s;
}



