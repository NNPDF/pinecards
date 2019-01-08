// emacs: this is -*- c++ -*-
//
//   appl_pdf.cxx        
//
//   pdf transform functions                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: appl_pdf.cxx, v1.0   Mon Dec 10 01:36:04 GMT 2007 sutt $

#include <fstream>

#include "appl_grid/appl_pdf.h" 

#include "mcfmz_pdf.h"
#include "mcfmzjet_pdf.h"
#include "mcfmw_pdf.h"
#include "mcfmwjet_pdf.h"
#include "mcfmwc_pdf.h"
#include "mcfmQQ_pdf.h"
#include "nlojet_pdf.h"
#include "jetrad_pdf.h"
#include "nlojetpp_pdf.h"
#include "dis_pdf.h"
#include "vrapz_pdf.h"
#include "appl_grid/basic_pdf.h"
// #include "generic_pdf.h"




namespace appl { 


// initialise the std::map with some default instances
// although the user could create these themselves
// if they wanted
pdfmap appl_pdf::__pdfmap; 

std::vector<std::string> appl_pdf::__pdfpath; 


/// constructor and destructor
appl_pdf::appl_pdf(const std::string& name) : 
  m_Nproc(0), m_name(name), m_ckmcharge(0) { 
   if ( m_name!="" ) addtopdfmap(m_name, this);
}
  
appl_pdf::~appl_pdf() { 
  // when I'm destroyed, remove my entry from the std::map 
  pdfmap::iterator mit = __pdfmap.find(m_name);
  if ( mit!=__pdfmap.end() ) __pdfmap.erase(mit);
} 


/// retrieve an instance from the std::map 
appl_pdf* appl_pdf::getpdf(const std::string& s, bool ) {
  /// initialise the factory
  if ( __pdfmap.size()==0 ) appl::appl_pdf::create_map(); 
  pdfmap::iterator itr = __pdfmap.find(s);
  if ( itr!=__pdfmap.end() ) return itr->second; 
 
  /// not found in std::map - used to throw (and catch) an exception 
  return 0;
  //  throw exception( std::cerr << "getpdf() " << s << " not instantiated in std::map " );
}





std::ifstream& appl_pdf::openpdf( const std::string& filename ) { 

  /// if not set up yet, set up the search path for 
  /// the pdf config files
  if ( __pdfpath.size()==0 ) { 
    __pdfpath.push_back("");
    __pdfpath.push_back(std::string(DATADIR)+"/");
  }

  static std::ifstream infile;

  for ( unsigned i=0 ; i<__pdfpath.size() ; i++ ) { 
    /// try file
    infile.open( (__pdfpath[i]+filename).c_str() );
    /// if found return
    if ( !infile.fail() ) { 
      std::cout << "appl_pdf::openpdf() opening " << __pdfpath[i]+filename << std::endl;
      return infile;
    }
  }

  /// haven't found the config file, so throw an exception ...
  throw exception( std::cerr << "appl_pdf::appl_pdf() cannot open file " << filename << std::endl ); 
	
  /// this should never be executed - just here so the function doesn't fall off the end
  return infile;
} 


int appl_pdf::decideSubProcess( const int , const int  ) const { return -1; }



bool appl_pdf::create_map() { 

#ifdef DBG
  std::cout << "appl_pdf::create_map() creating pdf combination factory" << std::endl;
#endif

  if ( __pdfmap.size()==0 ) { 
    
    /// the appl_pdf add their own pointers to the 
    /// pdf std::map so we don;t need to remember their 
    /// pointers ourselves
    new  mcfmz_pdf;
    new  mcfmzjet_pdf;
    new  mcfmwp_pdf;
    new  mcfmwm_pdf;
    new  mcfmwpjet_pdf;
    new  mcfmwmjet_pdf;
    
    new  mcfmwpc_pdf;
    new  mcfmwmc_pdf;
    
    new  mcfmCC_pdf;
    new  mcfmBB_pdf;
    new  mcfmTT_pdf;

    new nlojet_pdf;
    new nlojetpp_pdf;
    new jetrad_pdf;
    new dis_pdf;

    new vrapzLO_pdf;
    new vrapzNLO_pdf;
    new vrapzNNLO_pdf;

    new basic_pdf;

    //    printmap( std::cerr );

  }

  return true;
}




/// get ckm related information 


void appl_pdf::make_ckm( bool Wp ) { 
  
  // std::cout << "make_ckm() initialising" << std::endl;
  std::vector<std::vector<double> > _ckm2(14, std::vector<double>(14,0));
  
  if ( Wp ) { 
    
    m_ckmcharge = +1;

    //  std::cout << "creating ckm matrix terms for Wplus production" << std::endl;
    
    _ckm2[3][8]  =   0.049284000000000001417976847051249933 ;
    _ckm2[8][3]  =   0.049284000000000001417976847051249933 ;
    
    _ckm2[5][8]  =   0.950624999999999942268402719491859898 ;
    _ckm2[8][5]  =   0.950624999999999942268402719491859898 ;
    
    _ckm2[5][10] =   0.049284000000000001417976847051249933 ;
    _ckm2[10][5] =   0.049284000000000001417976847051249933 ;
    
    _ckm2[3][10] =   0.950624999999999942268402719491859898 ;
    _ckm2[10][3] =   0.950624999999999942268402719491859898 ;
    
  }
  else { 

    m_ckmcharge = -1;
    
    //    std::cout << "creating ckm matrix terms for Wminus production" << std::endl;
    
    _ckm2[4][9] =   0.049284000000000001417976847051249933 ;
    _ckm2[9][4] =   0.049284000000000001417976847051249933 ;
        
    _ckm2[7][4] =   0.950624999999999942268402719491859898 ;
    _ckm2[4][7] =   0.950624999999999942268402719491859898 ;
    
    _ckm2[7][2] =   0.049284000000000001417976847051249933 ;
    _ckm2[2][7] =   0.049284000000000001417976847051249933 ;
    
    _ckm2[9][2] =   0.950624999999999942268402719491859898 ;
    _ckm2[2][9] =   0.950624999999999942268402719491859898 ;
    
  }  
 
  setckm2( _ckm2 );

}



void appl_pdf::setckm( const std::vector<std::vector<double> >& ckm ) { 

  m_ckm = ckm; 
  
  /// calculate ckm2 and pass into setckm2  

  //  std::cout << "appl_pdf::setckm() ";
  
  if ( m_ckmcharge==0 ) return;
  
  std::vector<std::vector<double> > ckm2( 14, std::vector<double>(14,0) );
  for ( int iU=0 ; iU<3 ; iU++ ) {
    for ( int iD=0 ; iD<3 ; iD++ ) {

      //  std::cout << " " << int(10000*ckm[iU][iD]+0.5)*0.0001; // print rounded values

      /// lhapdf codes ...
      int utype = 2*iU+2;
      int dtype = 2*iD+1;

      /// W- 
      if ( m_ckmcharge<0 ) utype *= -1;

      /// W+ 
      if ( m_ckmcharge>0 ) dtype *= -1;

      /// translate to array indices
      utype += 6;
      dtype += 6;

      double V2 = ckm[iU][iD]*ckm[iU][iD];

      ckm2[utype][dtype] = V2;
      ckm2[dtype][utype] = V2;
      
    }
  }
  
  //  std::cout << std::endl;

  setckm2( ckm2 );
} 




void appl_pdf::setckm2( const std::vector<std::vector<double> >& ckm2 ) { 
  m_ckm2 = ckm2; 
  m_ckmsum = std::vector<double>(m_ckm2.size(),0);
  for ( unsigned i=0 ; i<m_ckm2.size() ; i++ ) { 
    for ( unsigned j=0 ; j<m_ckm2[i].size() ; j++ ) m_ckmsum[i] += m_ckm2[i][j]; 
  }  
} 





};




