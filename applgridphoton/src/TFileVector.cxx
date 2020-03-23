//
//   TFileVector.cxx        
//
//    root TObject std::string std::vector class for writing std::string std::vectors
//    to root files               
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: TFileString.cxx, v0.0   Sat Mar 15 19:50:15 GMT 2008 sutt



#include "TFileVector.h"

ClassImp(TFileVector)


std::ostream& operator<<(std::ostream& s, const TFileVector& fs) { 
  for ( unsigned i=0 ; i<fs.size() ; i++ ) { 
    for ( unsigned j=0 ; j<fs[i].size() ; j++ ) s << "\t" << fs[i][j]; 
    s << std::endl;
  }
  return s;
}



