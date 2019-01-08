
//  appl_grid.cxx        

//   grid class - all the functions needed to create and 
//   fill the grid from an NLO calculation program. 
//  
//  Copyright (C) 2007 Mark Sutton (sutt@hep.ucl.ac.uk)    

// $Id: appl_grid.cxx, v1.00 2007/10/16 17:01:39 sutt $

#include <cstdlib>
#include <stdio.h>
#include <sys/stat.h>

#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>

#include "appl_grid/appl_pdf.h"
#include "appl_grid/appl_timer.h"
#include "appl_grid/Directory.h"
#include "appl_grid/appl_grid.h"

#include "appl_grid/generic_pdf.h"
#include "appl_grid/lumi_pdf.h"

#include "appl_igrid.h"
#include "Cache.h"


#include "TFileString.h"
#include "TFileVector.h"

#include "TFile.h"
#include "TObjString.h"
#include "TVectorT.h"


#include "amconfig.h"




template<typename T>
std::ostream& operator<<(std::ostream& s, const std::vector<T>& v) {
  for ( unsigned i=0 ; i<v.size() ; i++ ) s << "\t" << v[i];
  return s;
}


/// this is a compatability flag for persistent versions 
/// of the grid
/// NB: ONLY change the major version if the persistent 
///     class changes in a non-backwards compatible way.

// const std::string appl::grid::m_version = "version-3.3";
const std::string appl::grid::m_version = PACKAGE_VERSION;

std::string appl::grid::appl_version() const { return PACKAGE_VERSION; }

#include "hoppet_init.h"

#ifdef HAVE_HOPPET

#include "hoppet_v1.h"

// include hoppet splitting function code

static hoppet_init* hoppet = 0;

void Splitting(const double& x, const double& Q, double* xf) {
  static const int nLoops    = 1;
  static const int nFlavours = 5;
  hoppetevalsplit_( x, Q, nLoops, nFlavours, xf); 
  return;
}

#else

void Splitting(const double& x, const double& Q, double* xf) {
  throw appl::grid::exception( std::cerr << "hoppet library not included - cannot call splitting function"  ); 
  return; // technically, don't need this - should throw an exception
}

#endif


#define appl_line() std::cout << __FILE__ << " " << __LINE__ << std::endl; 


/// helper function
static bool contains(const std::string& s, const std::string& reg ) { 
  return s.find(reg)!=std::string::npos;
}

/// make sure pdf std::map is initialised
// bool pdf_ready = appl::appl_pdf::create_map(); 


appl::grid::grid(int NQ2, double Q2min, double Q2max, int Q2order, 
		 int Nx,  double xmin,  double xmax,  int xorder,
		 int Nobs,  double obsmin, double obsmax, 
		 std::string genpdfname,
		 int leading_order, int nloops, 
		 std::string transform ) :
  m_leading_order(leading_order), m_order(nloops+1), 
  m_run(0), m_optimised(false), m_trimmed(false), m_normalised(false), m_symmetrise(false), 
  m_transform(transform), m_genpdfname(genpdfname), m_cmsScale(0), m_dynamicScale(0),
  m_applyCorrections(false),
  m_documentation(""),
  m_type(STANDARD),
  m_read(false),
  m_subproc(-1),
  m_bin(-1)
{
  // Initialize histogram that saves the correspondence obsvalue<->obsbin
  m_obs_bins=new TH1D("referenceInternal","Bin-Info for Observable", Nobs, obsmin, obsmax);
  m_obs_bins->SetDirectory(0);
  m_obs_bins->Sumw2(); /// grrr root is so rubbish - not scaling errors properly

  m_obs_bins_combined = m_obs_bins;

  /// check to see if we require a generic pdf from a text file, and 
  /// and if so, create the required generic pdf
  //  if      ( m_genpdfname.find(".dat")!=std::string::npos ) addpdf(m_genpdfname);
  //  else if ( m_genpdfname.find(".dat")!=std::string::npos ) addpdf(m_genpdfname);
  if ( contains(m_genpdfname, ".dat") ||  contains(m_genpdfname, ".config") ) addpdf(m_genpdfname);
  findgenpdf( m_genpdfname );

  construct(Nobs, NQ2, Q2min, Q2max, Q2order, Nx, xmin, xmax, xorder, m_order, m_transform); 
}




appl::grid::grid(int Nobs, const double* obsbins, 
		 int NQ2,  double Q2min, double Q2max, int Q2order, 
		 int Nx,   double xmin, double xmax,   int xorder, 
		 std::string genpdfname, 
		 int leading_order, int nloops, 
		 std::string transform ) :
  m_leading_order(leading_order), m_order(nloops+1), 
  m_run(0), m_optimised(false), m_trimmed(false),  m_normalised(false), m_symmetrise(false),
  m_transform(transform), m_genpdfname(genpdfname), m_cmsScale(0), m_dynamicScale(0),
  m_applyCorrections(false),
  m_documentation(""),
  m_type(STANDARD),
  m_read(false),
  m_subproc(-1),
  m_bin(-1)
{
  
  // Initialize histogram that saves the correspondence obsvalue<->obsbin
  m_obs_bins=new TH1D("referenceInternal","Bin-Info for Observable", Nobs, obsbins);
  m_obs_bins->SetDirectory(0);
  m_obs_bins->Sumw2(); /// grrr root is so rubbish - not scaling errors properly

  m_obs_bins_combined = m_obs_bins;

  /// check to see if we require a generic pdf from a text file, and 
  /// and if so, create the required generic pdf
  //  if ( m_genpdfname.find(".dat")!=std::string::npos ) addpdf(m_genpdfname);
  if ( contains(m_genpdfname, ".dat") ||  contains(m_genpdfname, ".config") ) addpdf(m_genpdfname);

  findgenpdf( m_genpdfname );

  construct(Nobs, NQ2, Q2min, Q2max, Q2order, Nx, xmin, xmax, xorder, m_order, m_transform );
}




appl::grid::grid(const std::vector<double>& obs, 
		 int NQ2, double Q2min, double Q2max, int Q2order,
		 int Nx,  double xmin,  double xmax,  int xorder, 
		 std::string genpdfname,
		 int leading_order, int nloops, 
		 std::string transform )  :
  m_leading_order(leading_order), m_order(nloops+1), 
  m_run(0), m_optimised(false), m_trimmed(false), m_normalised(false), m_symmetrise(false),  
  m_transform(transform), m_genpdfname(genpdfname), m_cmsScale(0), m_dynamicScale(0),
  m_applyCorrections(false),
  m_documentation(""),
  m_type(STANDARD),
  m_read(false),
  m_subproc(-1),
  m_bin(-1)
{
  
  if ( obs.size()==0 ) { 
    std::cerr << "grid::not enough bins in observable" << std::endl;
    std::exit(0);
  } 
  
  //  double* obsbins = new double[obs.size()];  
  //  for ( unsigned i=0 ; i<obs.size() ; i++ ) obsbins[i] = obs[i];
  //  int Nobs = obs.size()-1;

  // Initialize histogram that saves the correspondence obsvalue<->obsbin
  m_obs_bins=new TH1D("referenceInternal","Bin-Info for Observable", obs.size()-1, &obs[0] );
  m_obs_bins->SetDirectory(0);
  m_obs_bins->Sumw2(); /// grrr root is so rubbish - not scaling errors properly
  //  delete[] obsbins;

  m_obs_bins_combined = m_obs_bins;

  /// check to see if we require a generic pdf from a text file, and 
  /// and if so, create the required generic pdf
  //   if ( m_genpdfname.find(".dat")!=std::string::npos ) addpdf(m_genpdfname);
  if ( contains(m_genpdfname, ".dat") ||  contains(m_genpdfname, ".config") ) addpdf(m_genpdfname);
  findgenpdf( m_genpdfname );

  construct( obs.size()-1, NQ2, Q2min, Q2max, Q2order, Nx, xmin, xmax, xorder, m_order, m_transform); 
}



appl::grid::grid(const std::vector<double>& obs, 
		 std::string genpdfname,
		 int leading_order, int nloops, 
		 std::string transform )  :
  m_leading_order(leading_order), m_order(nloops+1), 
  m_run(0), m_optimised(false), m_trimmed(false), m_normalised(false), m_symmetrise(false),  
  m_transform(transform), m_genpdfname(genpdfname), m_cmsScale(0), m_dynamicScale(0),
  m_applyCorrections(false),  
  m_documentation(""),
  m_type(STANDARD),
  m_read(false),
  m_subproc(-1),
  m_bin(-1)
{ 

  if ( obs.size()==0 ) { 
    std::cerr << "grid::not enough bins in observable" << std::endl;
    std::exit(0);
  } 
  
  //  double* obsbins = new double[obs.size()];  
  //  for ( unsigned i=0 ; i<obs.size() ; i++ ) obsbins[i] = obs[i];
  //  int Nobs = obs.size()-1;

  // Initialize histogram that saves the correspondence obsvalue<->obsbin
  m_obs_bins=new TH1D("referenceInternal","Bin-Info for Observable", obs.size()-1, &obs[0] );
  m_obs_bins->SetDirectory(0);
  m_obs_bins->Sumw2(); /// grrr root is so rubbish - not scaling errors properly
  //  delete[] obsbins;

  m_obs_bins_combined = m_obs_bins;

  /// check to see if we require a generic pdf from a text file, and 
  /// and if so, create the required generic pdf
  //  if ( m_genpdfname.find(".dat")!=std::string::npos ) addpdf(m_genpdfname);
  if ( contains(m_genpdfname, ".dat") ||  contains(m_genpdfname, ".config") ) addpdf(m_genpdfname);
  findgenpdf( m_genpdfname );

  for ( int iorder=0 ; iorder<m_order ; iorder++ ) m_grids[iorder] = new igrid*[obs.size()-1];

}



appl::grid::grid(const std::string& filename, const std::string& dirname)  :
  m_leading_order(0),  m_order(0),
  m_optimised(false),  m_trimmed(false), 
  m_normalised(false),
  m_symmetrise(false), m_transform(""), 
  m_dynamicScale(0),
  m_applyCorrections(false),
  m_documentation(""),
  m_type(STANDARD),
  m_read(false),
  m_subproc(-1),
  m_bin(-1)
{
  m_obs_bins_combined = m_obs_bins = 0;

  struct timeval tstart = appl_timer_start();
  
  struct stat _fileinfo;
  if ( stat(filename.c_str(),&_fileinfo) )   {    
    throw exception(std::cerr << "grid::grid() cannot open file " << filename << std::endl ); 
  }

  std::cout << "appl::grid() reading grid from file " << filename;
  
  TFile* gridfilep = TFile::Open(filename.c_str());

  if (gridfilep==0 ) {
    std::cout << std::endl;
    throw exception(std::cerr << "grid::grid() cannot open file: " << filename << std::endl ); 
  }

  if (gridfilep->IsZombie()) {
    std::cout << std::endl;
    delete gridfilep;
    throw exception(std::cerr << "grid::grid() cannot open file: zombie " << filename << std::endl ); 
  }

  // TFile gridfile(filename.c_str());
  
  //  gDirectory->cd(dirname.c_str());

  //  std::cout << "pwd=" << gDirectory->GetName() << std::endl;

  //  gDirectory->cd(dirname.c_str());

  //  std::cout << "pwd=" << gDirectory->GetName() << std::endl;

  //  Directory d(dirname);
  //  d.push();

  TFileString* _tagsp = (TFileString*)gridfilep->Get((dirname+"/Tags").c_str());  

  if ( _tagsp==0 ) { 
    std::cout << std::endl;
    throw exception(std::cerr << "grid::grid() cannot get tags: " << filename << std::endl ); 
  }

  TFileString _tags = *_tagsp;
  // TFileString _tags = *(TFileString*)gridfile.Get("Tags");  

  m_transform  = _tags[0];
  m_genpdfname = _tags[1];
  std::string _version = _tags[2];

  //  std::cout << "tags:: transform: " << m_transform << "\tpdfname: " << m_genpdfname << "\tdoc: " << m_documentation << std::endl;  

  if ( _tags.size()>3 ) m_documentation = _tags[3];

  // check it has the correct version
  // if ( _version > m_version ) { 
  //      throw exception(cerr << "incorrect version " << _version << " expected " << m_version ); 
  // }
  //  m_version = _version;
  
  std::cout << "\tversion " << _version;
  if ( _version != m_version ) std::cout  << "(transformed to " << m_version << ")";
  std::cout << std::endl;

  if ( getDocumentation()!="" ) std::cout << getDocumentation() << std::endl; 
  
  bool added = false;

  if ( m_genpdfname=="basic" ) { 
    added = true;
    m_genpdfname = "basic.config"; 
    if ( contains(m_genpdfname, ".dat") ||  contains(m_genpdfname, ".config") ) addpdf(m_genpdfname);
    findgenpdf( m_genpdfname );
  }

  //  std::cout << "Tags=" << _tags << std::endl;

  //  std::cout << "grid::grid() use transform " << m_transform << std::endl;

  // read state information
  // hmmm, have to use TVectorT<double> since TVector<int> 
  // apparently has no constructor (???)
  TVectorT<double>* setup=(TVectorT<double>*)gridfilep->Get((dirname+"/State").c_str());
 
  m_run        = (*setup)(0);
  m_optimised  = ( (*setup)(1)!=0 ? true : false );
  m_symmetrise = ( (*setup)(2)!=0 ? true : false );  

  m_leading_order = int((*setup)(3)+0.5);  
  m_order         = int((*setup)(4)+0.5);  

  if ( setup->GetNoElements()>5 ) m_cmsScale = (*setup)(5);
  else                            m_cmsScale = 0;
 
  if ( setup->GetNoElements()>6 ) m_normalised = ( (*setup)(6)!=0 ? true : false );
  else                            m_normalised = true;

  if ( setup->GetNoElements()>7 ) m_applyCorrections = ( (*setup)(7)!=0 ? true : false );
  else                            m_applyCorrections = false;

  int n_userdata = 0;
  if ( setup->GetNoElements()>10 ) n_userdata = int((*setup)(10)+0.5);

  //  std::vector<double> _ckmsum;
  std::vector<std::vector<double> > _ckm2;
  std::vector<std::vector<double> > _ckm;



  /// check whether we need to read in the ckm matrices

  if ( setup->GetNoElements()>8 && (*setup)(8)!=0 ) {

    std::cout << "grid::grid() read ckm matrices" << std::endl;
    
    /// try 14x14 squared ckm matrix 

    TVectorT<double>* ckm2flat=(TVectorT<double>*)gridfilep->Get((dirname+"/CKM2").c_str());

    if ( ckm2flat ) { 
      if ( ckm2flat->GetNrows()>0 ) { 
	_ckm2 = std::vector<std::vector<double> >(14, std::vector<double>(14,0) );
      
  // Note in normal APPLgrids only 13 partons are put in the flat CKM array
	for ( int ic=0 ; ic<13 ; ic++ ) { 
	  for ( int id=0 ; id<13 ; id++ ) _ckm2[ic][id] = (*ckm2flat)(ic*13+id); 
	}
      }  
    }

    /// now try usual 3x3 matrix

    TVectorT<double>* ckmflat=(TVectorT<double>*)gridfilep->Get((dirname+"/CKM").c_str());

    if ( ckmflat ) { 
      if ( ckmflat->GetNrows()>0 ) { 
	_ckm = std::vector<std::vector<double> >(3, std::vector<double>(3) );
	
	for ( int ic=0 ; ic<3 ; ic++ ) { 
	  for ( int id=0 ; id<3 ; id++ ) _ckm[ic][id] = (*ckmflat)(ic*3+id); 
	}
      }  
    }

  }

  if ( setup->GetNoElements()>9 ) m_type = (CALCULATION)int( (*setup)(9)+0.5 );
  else                            m_type = STANDARD;

  //  std::cout << "appl::grid() reading grid calculation type: " << _calculation(m_type) << std::endl;

  //  std::cout << "appl::grid() normalised: " << getNormalised() << std::endl;

  /// check to see if we require a generic pdf from a text file, and 
  /// and if so, create the required generic pdf (or lumi_pdf for amcatnlo)
  //  if ( m_genpdfname.find(".dat")!=std::string::npos ) addpdf(m_genpdfname);

  //  std::cout << "appl::grid() requested pdf combination " << m_genpdfname << std::endl;
  

  if ( !added && contains(m_genpdfname, ".config") ) { 
    /// decode the pdf combination if appropriate

    // find out if we have one combination per order or one overall
    std::vector<std::string> namevec = parse( m_genpdfname, ":" );

    std::string label = dirname+"/Combinations";

    for ( unsigned i=0 ; i<namevec.size() ; i++ ) { 

      /// again have to use TVectorT<double> because TVectorT<int> has no constructor!!!
      /// I ask you!! what's the point of a template if it doesn't actually instantiate
      /// it's pathetic!

      TVectorT<double>* _combinations = (TVectorT<double>*)gridfilep->Get( label.c_str() );

      label += "N"; /// add an N for each order, N-LO, NN-LO etc

      if ( _combinations==0 ) throw exception(std::cerr << "grid::grid() cannot read pdf combination " << namevec[i] << std::endl );

      std::vector<int> combinations(_combinations->GetNoElements());

      for ( unsigned ic=0 ; ic<combinations.size() ; ic++ )  combinations[ic] = int((*_combinations)(ic)); 
    
      //      addpdf(m_genpdfname, combinations);
      addpdf( namevec[i], combinations);

    }
  }
  else { 
    /// of just create the generic from the file
    if ( contains(m_genpdfname, ".dat") ) addpdf(m_genpdfname);
  }
  
  /// retrieve the pdf routine 
  findgenpdf( m_genpdfname );

  //  std::cout << "grid::grid() read " << m_genpdfname << " " << m_genpdf[0]->getckmsum().size() << std::endl; 

  // set the ckm matrices 
  if      ( _ckm.size()>0 )  setckm( _ckm );
  else if ( _ckm2.size()>0 ) setckm2( _ckm2 );

  delete setup;

  //  std::cout << "grid::grid() read setup" << std::endl;

  // Read observable bins information
  //  gridfile.GetObject("obs_bins", m_obs_bins );

  m_obs_bins = (TH1D*)gridfilep->Get((dirname+"/reference_internal").c_str());
  if ( m_obs_bins ) { 
    m_obs_bins_combined = (TH1D*)gridfilep->Get((dirname+"/reference").c_str());
    m_obs_bins_combined->SetDirectory(0);
    m_obs_bins_combined->Scale(run());
  }
  else { 
    m_obs_bins = (TH1D*)gridfilep->Get((dirname+"/reference").c_str());
    m_obs_bins_combined = m_obs_bins;
  }

  m_obs_bins->SetDirectory(0);
  m_obs_bins->Scale(run());
  m_obs_bins->SetName("referenceInternal");
  if ( m_normalised && m_optimised ) m_read = true;


  //  std::cout << "grid::grid() read obs bins" << std::endl;

  for( int iorder=0 ; iorder<m_order ; iorder++ ) {
    //    std::cout << "grid::grid() iorder=" << iorder << std::endl;
    m_grids[iorder] = new igrid*[Nobs_internal()];  
    for( int iobs=0 ; iobs<Nobs_internal() ; iobs++ ) {
      char name[128];  sprintf(name, (dirname+"/weight[alpha-%d][%03d]").c_str(), iorder, iobs);
      //   std::cout << "grid::grid() reading " << name << "\tiobs=" << iobs << std::endl;

      m_grids[iorder][iobs] = new igrid(*gridfilep, name);
      m_grids[iorder][iobs]->setparent( this ); 

      //    _size += m_grids[iorder][iobs]->size();
      //      std::cout << "grid::grid() done" << std::endl;
    }
  }

  //  d.pop();

  /// bin-by-bin correction labels                                       
  TFileString* correctionLabels = (TFileString*)gridfilep->Get((dirname+"/CorrectionLabels").c_str());  
  if ( correctionLabels ) { 
    for ( unsigned i=0 ; i<correctionLabels->size() ; i++ ) {
      m_correctionLabels.push_back( (*correctionLabels)[i] ); // copy the correction label
    }
  }

  /// bin-by-bin correction values
  TFileVector* corrections = (TFileVector*)gridfilep->Get((dirname+"/Corrections").c_str());  
  if ( corrections ) { 
    for ( unsigned i=0 ; i<corrections->size() ; i++ ) {
      m_corrections.push_back( (*corrections)[i] ); // copy the correction histograms
      m_applyCorrection.push_back(false);
    }
  }


  /// now read in vector of bins to be combined if present
  TVectorT<double>* _combine = (TVectorT<double>*)gridfilep->Get((dirname+"/CombineBins").c_str());

  if ( _combine!=0 ) { 
    //    std::cout << "read in " << _combine->GetNrows() << " entries in combine" << std::endl;
    m_combine = std::vector<int>(_combine->GetNrows(),0);
    for ( unsigned i=_combine->GetNrows() ; i-- ; ) m_combine[i] = int((*_combine)(i));

    if ( m_combine.size() ) combineReference();
  }

  //  std::cout << "grid::grid() read from file Nobs = " << Nobs_internal() << std::endl;

  //  std::cout << "read grid" << std::endl;

  /// read in the user data
  if ( n_userdata ) { 
    TVectorT<double>* userdata=(TVectorT<double>*)gridfilep->Get((dirname+"/UserData").c_str());
    m_userdata.clear();
    for ( int i=0 ; i<n_userdata ; i++ ) m_userdata.push_back( (*userdata)(i) );
  }

  gridfilep->Close();
  delete gridfilep;

  double tstop = appl_timer_stop( tstart );

  unsigned usize = size();

  struct timeval tstart2 =  appl_timer_start();

  trim();

  double tstop2 = appl_timer_stop( tstart2 );

  std::cout << "appl::grid() read grid, size ";
  if ( usize>1024*10 ) std::cout << usize/1024/1024 << " MB";
  else                 std::cout << usize/1024      << " kB";
  std::cout << "\tin " << tstop << " ms";

  std::cout << "\ttrim in " << tstop2 << " ms" << std::endl;

}


appl::grid::grid(const grid& g) : 
  m_obs_bins(new TH1D(*g.m_obs_bins)), 
  m_leading_order(g.m_leading_order), m_order(g.m_order), 
  m_run(g.m_run), m_optimised(g.m_optimised), m_trimmed(g.m_trimmed), 
  m_normalised(g.m_normalised),
  m_symmetrise(g.m_symmetrise),
  m_transform(g.m_transform),
  m_genpdfname(g.m_genpdfname), 
  m_cmsScale(g.m_cmsScale),
  m_dynamicScale(g.m_dynamicScale),
  m_applyCorrections(g.m_applyCorrections), 
  m_documentation(g.m_documentation),
  m_ckmsum(g.m_ckmsum), /// need a deep copy of the contents
  m_ckm2(g.m_ckm2),     /// need a deep copy of the contents
  m_ckm(g.m_ckm),       /// need a deep copy of the contents
  m_type(g.m_type),
  m_read(g.m_read),
  m_bin(-1)
{
  m_obs_bins->SetDirectory(0);
  m_obs_bins->Sumw2();

  m_obs_bins_combined = m_obs_bins;
  if ( g.m_obs_bins_combined != g.m_obs_bins) { 
    m_obs_bins_combined = new TH1D(*g.m_obs_bins_combined); 
    m_obs_bins_combined->SetDirectory(0);
  }

  /// check to see if we require a generic pdf from a text file, and 
  /// and if so, create the required generic pdf
  //  if ( m_genpdfname.find(".dat")!=std::string::npos ) addpdf(m_genpdfname);
  if ( contains(m_genpdfname, ".dat") ||  contains(m_genpdfname, ".config") ) addpdf(m_genpdfname);
  findgenpdf( m_genpdfname );

  for ( int iorder=0 ; iorder<m_order ; iorder++ ) { 
    m_grids[iorder] = new igrid*[Nobs_internal()];
    for ( int iobs=0 ; iobs<Nobs_internal() ; iobs++ )  { 
      m_grids[iorder][iobs] = new igrid(*g.m_grids[iorder][iobs]);
      m_grids[iorder][iobs]->setparent( this ); 
    }
  }
} 


// Initialize histogram that saves the correspondence obsvalue<->obsbin

// constructor common internals 
void appl::grid::construct(int Nobs, 
			   int NQ2,  double Q2min, double Q2max, int Q2order,
			   int Nx,   double xmin,  double xmax,  int xorder, 
			   int order, 
			   std::string transform ) { 
  
  //  std::cout << "appl::grid::construct() m_order " << m_order << "\tNobs " << Nobs << std::endl; 

  if ( m_order!=order ) std::cerr << "appl::grid::construct() order mismatch" << std::endl;

  for ( int iorder=0 ; iorder<m_order ; iorder++ ) m_grids[iorder] = 0;

  for ( int iorder=0 ; iorder<m_order ; iorder++ ) { 
    m_grids[iorder] = new igrid*[Nobs];
    for ( int iobs=0 ; iobs<Nobs ; iobs++ ) {
      m_grids[iorder][iobs] = new igrid(NQ2, Q2min, Q2max, Q2order, 
					Nx, xmin, xmax, xorder, 
					transform, m_genpdf[iorder]->Nproc());
      m_grids[iorder][iobs]->setparent( this ); 
    }
  }
  //  std::cout << "appl::grid::construct() return" << std::endl; 
}




  // number of subprocesses 
int appl::grid::subProcesses(int i) const { 
  if ( i<0 || i>=m_order ) throw exception( std::cerr << "grid::subProcess(int i) " << i << " out or range [0-" << m_order-1 << "]" << std::endl );
  return m_grids[i][0]->SubProcesses();     
}  


/// access the transform functions for the appl::igrid so that the 
/// igrid can be hidden 
double appl::grid::transformvar()         { return igrid::transformvar(); }
double appl::grid::transformvar(double v) { return igrid::transformvar(v); }



// add a single grid
void appl::grid::add_igrid(int bin, int order, igrid* g) { 

  if ( !(order>=0 && order<m_order) ) { 
    std::cerr << "grid::add_igrid() order out of range " << order << std::endl; 
    return;
  } 

  if ( !(bin>=0 && bin<Nobs_internal() ) ) {
    std::cerr << "grid::add_igrid() observable bin out of range " << bin << std::endl; 
    return;
  }

  m_grids[order][bin] = g;
  m_grids[order][bin]->setparent(this);

  if ( g->transform()!=m_transform ) { 
    std::cerr << "grid::add_igrid() transform " << m_transform 
	      << " does not match that from added igrid, " << g->transform() << std::endl;
    m_transform = g->transform();
  }

}




appl::grid::~grid() {
  for( int iorder=0 ; iorder<m_order ; iorder++ ) {  
    if( m_grids[iorder] ) { 
      for ( int iobs=0 ; iobs<Nobs_internal() ; iobs++ ) { 
	delete m_grids[iorder][iobs];
      }
      delete[] m_grids[iorder];
      m_grids[iorder] = 0;
    }
  }
  if (m_obs_bins_combined) {
    if ( m_obs_bins_combined!=m_obs_bins) delete m_obs_bins_combined;
  }
  if (m_obs_bins) delete m_obs_bins;
  m_obs_bins=0;
  m_obs_bins_combined = 0;

#ifdef HAVE_HOPPET
  if ( hoppet ) delete hoppet; 
  hoppet=0; 
#endif

}




// algebraic operators

appl::grid& appl::grid::operator=(const appl::grid& g) { 
  // clear out the old...
  if ( m_obs_bins_combined!=m_obs_bins ) delete m_obs_bins_combined;
  delete m_obs_bins;
  for ( int iorder=0 ; iorder<m_order ; iorder++ ) { 
    for ( int iobs=0 ; iobs<Nobs_internal() ; iobs++ )  delete m_grids[iorder][iobs];
    delete m_grids[iorder];
  }
  
  // copy the new
  m_obs_bins = new TH1D(*g.m_obs_bins);
  m_obs_bins->SetDirectory(0);
  m_obs_bins->Sumw2();

  m_obs_bins_combined = m_obs_bins;

  // copy the state
  m_leading_order = g.m_leading_order;
  m_order         = g.m_order;

  m_run       = g.m_run;
  m_optimised = g.m_optimised;
  m_trimmed   = g.m_trimmed;

  for ( int iorder=0 ; iorder<m_order ; iorder++ ) { 
    m_grids[iorder] = new igrid*[Nobs_internal()];
    for ( int iobs=0 ; iobs<Nobs_internal() ; iobs++ )  { 
      m_grids[iorder][iobs] = new igrid(*g.m_grids[iorder][iobs]);
      m_grids[iorder][iobs]->setparent( this ); 
    }
  }
  return *this;
} 
  

appl::grid& appl::grid::operator*=(const double& d) { 
  for( int iorder=0 ; iorder<m_order ; iorder++ ) {
    for( int iobs=0 ; iobs<Nobs_internal() ; iobs++ ) (*m_grids[iorder][iobs])*=d; 
  }
  getReference()->Scale( d );
  combineReference(true);
  return *this;
}


double appl::grid::fx(double x) const { 
  if ( m_order>0 && Nobs_internal()>0 ) return m_grids[0][0]->fx(x);
  else return 0;
}

double appl::grid::fy(double x) const { 
  if ( m_order>0 && Nobs_internal()>0 ) return m_grids[0][0]->fy(x);
  else return 0;
}


appl::grid& appl::grid::operator+=(const appl::grid& g) {
  m_run      += g.m_run;
  m_optimised = g.m_optimised;
  m_trimmed   = g.m_trimmed;
  if ( Nobs_internal()!=g.Nobs_internal() )   throw exception("grid::operator+ Nobs bin mismatch");
  if ( m_order!=g.m_order ) throw exception("grid::operator+ different order grids");
  if ( m_leading_order!=g.m_leading_order ) throw exception("grid::operator+ different order processes in grids");
  for( int iorder=0 ; iorder<m_order ; iorder++ ) {
    for( int iobs=0 ; iobs<Nobs_internal() ; iobs++ ) (*m_grids[iorder][iobs]) += (*g.m_grids[iorder][iobs]); 
  }

  /// grrr use root TH1::Add() even though I don't like it. 
  getReference()->Add( g.getReference() );
  combineReference(true);

  return *this;
}




/// check grids match
bool appl::grid::operator==(const appl::grid& g) const {
  
  bool match = true; 

  if ( Nobs_internal()!=g.Nobs_internal() )    match = false;
  if ( m_order!=g.m_order )  match = false;
  if ( m_leading_order!=g.m_leading_order ) match = false;
  for( int iorder=0 ; iorder<m_order ; iorder++ ) {
    //    for( int iobs=0 ; iobs<Nobs_internal() ; iobs++ ) match &= ( (*m_grids[iorder][iobs]) == (*g.m_grids[iorder][iobs]) ); 
    for( int iobs=0 ; iobs<Nobs_internal() ; iobs++ ) match &= ( m_grids[iorder][iobs]->compare_axes( *g.m_grids[iorder][iobs] ) ); 
  }
  return match;
}



// fill the appropriate igrid with these weights
void appl::grid::fill(const double x1, const double x2, const double Q2, 
		      const double obs, 
		      const double* weight, const int iorder)  {  
  int iobs = m_obs_bins->FindBin(obs)-1;
  if ( iobs<0 || iobs>=Nobs_internal() ) {
    //    cerr << "grid::fill() iobs out of range " << iobs << "\tobs=" << obs << std::endl;
    //    cerr << "obs=" << obs << "\tobsmin=" << obsmin() << "\tobsmax=" << obsmax() << std::endl;
    return;
  }
  
  //  std::cout << "iobs=" << iobs << "\tobs=" << obs;
  //  for ( int i=0 ; i<subProcesses(iorder) ; i++ ) std::cout << "\t" << weight[i];
  //  std::cout << std::endl;

  //  std::cout << "\tiobs=" << iobs << std::endl;
  if ( m_symmetrise && x2<x1 )  m_grids[iorder][iobs]->fill(x2, x1, Q2, weight);
  else                          m_grids[iorder][iobs]->fill(x1, x2, Q2, weight);
}


// fast fill pre-optimisation don't perform the interpolation and so on
void appl::grid::fill_phasespace(const double x1, const double x2, const double Q2, 
				 const double obs, 
				 const double* weight, const int iorder) {
  int iobs = m_obs_bins->FindBin(obs)-1;
  if ( iobs<0 || iobs>=Nobs_internal() ) {
    //  cerr << "grid::fill() iobs out of range " << iobs << "\tobs=" << obs << std::endl;
    //  cerr << "obs=" << obs << "\tobsmin=" << obsmin() << "\tobsmax=" << obsmax() << std::endl;
    return;
  }
  if ( m_symmetrise && x2<x1 )  m_grids[iorder][iobs]->fill_phasespace(x2, x1, Q2, weight);
  else                          m_grids[iorder][iobs]->fill_phasespace(x1, x2, Q2, weight);
}




// fast fill pre-optimisation don't perform the interpolation and so on
void appl::grid::fill_index(const int ix1, const int ix2, const int iQ2, 
			    const int iobs, 
			    const double* weight, const int iorder) {

  if ( iobs<0 || iobs>=Nobs_internal() ) {
    //  cerr << "grid::fill() iobs out of range " << iobs << "\tobs=" << obs << std::endl;
    //  cerr << "obs=" << obs << "\tobsmin=" << obsmin() << "\tobsmax=" << obsmax() << std::endl;
    return;
  }
  if ( m_symmetrise && ix2<ix1 )  m_grids[iorder][iobs]->fill_index(ix2, ix1, iQ2, weight);
  else                            m_grids[iorder][iobs]->fill_index(ix1, ix2, iQ2, weight);
}


void appl::grid::trim() {
  m_trimmed = true;
  for( int iorder=0 ; iorder<m_order ; iorder++ ) {
    for( int iobs=0 ; iobs<Nobs_internal() ; iobs++ ) m_grids[iorder][iobs]->trim(); 
  }
}

void appl::grid::untrim() {
  m_trimmed = false;
  for( int iorder=0 ; iorder<m_order ; iorder++ ) {
    for( int iobs=0 ; iobs<Nobs_internal() ; iobs++ ) m_grids[iorder][iobs]->untrim(); 
  }
}

std::ostream& appl::grid::print(std::ostream& s) const {
  for( int iorder=0 ; iorder<m_order ; iorder++ ) {
    for( int iobs=0 ; iobs<Nobs_internal() ; iobs++ ) {     
      s << iobs << "\t" 
	<< std::setprecision(5) << std::setw(6) << getReference()->GetBinLowEdge(iobs+1) << "\t- " 
	<< std::setprecision(5) << std::setw(6) << getReference()->GetBinLowEdge(iobs+2) << "\t"; 
      m_grids[iorder][iobs]->print(s);       
    }
  }
  return s;
}




 /// get the required pdf combinations from those registered   
void appl::grid::findgenpdf( std::string s ) { 
    std::vector<std::string> names = parse( s, ":" );
    if ( names.size()==unsigned(m_order) ) for ( int i=0 ; i<m_order ; i++ ) m_genpdf[i] = appl_pdf::getpdf( names[i] );
    else  if ( names.size()==1 )           for ( int i=0 ; i<m_order ; i++ ) m_genpdf[i] = appl_pdf::getpdf( names[0] );
    else  { 
      throw exception( std::cerr << "requested " << m_order << " pdf combination but given " << names.size() << std::endl );
    }
}


void appl::grid::addpdf( const std::string& s, const std::vector<int>& combinations ) {

  //  std::cout << "addpdf() in " << std::endl;

    /// parse names, if they contain .dat, then create the new generic pdfs
    /// they will be added to the pdf std::map automatically 
    std::vector<std::string> names = parse( s, ":" );

    unsigned imax = unsigned(m_order); 

    /// check to see whether we have a different pdf for each order
    if ( names.size()!=imax ) { 
      if ( names.size()==1 ) imax = 1;
      else { 
	throw exception( std::cerr << "requested " << m_order << " pdf combination but given " << names.size() << std::endl );
      }
    }

    //    std::cout << "imax " << imax << std::endl; 

    /// loop through all the required pdfs checking whether they exist already,
    /// if not (from thrown exception) then create it, otherwise, don't need to 
    /// do anything 
    for ( unsigned i=0 ; i<imax ; i++ ) { 

      //      std::cout << "\ti " << i<< std::endl; 

      if ( names[i].find(".dat")!=std::string::npos ) {

	if ( appl_pdf::getpdf(names[i])==0 ) { 
 	  std::cout << "appl::grid::addpdf() creating new generic_pdf " << names[i] << std::endl;
	  new generic_pdf(names[i]);
	}

	// 	try {
	// 	  appl_pdf::getpdf(names[i]); // , false);
	// 	}
	// 	catch ( appl_pdf::exception e ) { 
	// 	  std::cout << "creating new generic_pdf " << names[i] << std::endl;
	// 	  new generic_pdf(names[i]);
	// 	}

      }
      else if ( names[i].find(".config")!=std::string::npos ) { 

	if ( appl_pdf::getpdf(names[i])==0 ) { 
	  std::cout << "appl::grid::addpdf() creating new lumi_pdf " << names[i] << std::endl;
	  new lumi_pdf(names[i], combinations);
	}

	// 	try {
	// 	  appl_pdf::getpdf(names[i]); // , false);
	// 	}
	// 	catch ( appl_pdf::exception e ) { 
	// 	  std::cout << "creating new lumi_pdf " << names[i] << std::endl;
	// 	  new lumi_pdf(names[i], combinations);
	// 	  //	  std::cout << "created" << names[i] << std::endl;
	
	// 	}

      }

    }

}



void appl::grid::setckm2( const std::vector<std::vector<double> >& ckm2 ) { 
  for ( int i=0 ; i<m_order ; i++ ) m_genpdf[i]->setckm2(ckm2);
}


void appl::grid::setckm( const std::vector<std::vector<double> >& ckm ) { 
  for ( int i=0 ; i<m_order ; i++ ) m_genpdf[i]->setckm(ckm);
}


void appl::grid::setckm( const std::vector<double>& ckm ) {
  std::vector<std::vector<double> > _ckm(3, std::vector<double>(3,0) );
  for ( unsigned i=0 ; i<ckm.size() && i<9 ; i++ ) _ckm[i/3][i%3] = ckm[i]; 
  for ( int i=0 ; i<m_order ; i++ ) m_genpdf[i]->setckm(_ckm);
}

void appl::grid::setckm( const double* ckm ) { 
  std::vector<std::vector<double> > _ckm(3, std::vector<double>(3,0) );
  for ( unsigned i=0 ; i<9 ; i++ ) _ckm[i/3][i%3] = ckm[i]; 
  for ( int i=0 ; i<m_order ; i++ ) m_genpdf[i]->setckm(_ckm);
}


const std::vector<std::vector<double> >& appl::grid::getckm()  const { return m_genpdf[0]->getckm(); }  

const std::vector<std::vector<double> >& appl::grid::getckm2() const { return m_genpdf[0]->getckm2(); }  


// void appl::grid::setuppdf(void (*pdf)(const double&, const double&, double* ) )  {  }
// void grid::pdfinterp(double x, double Q2, double* f) {  }



// set the rewight flag of the internal grids
bool appl::grid::reweight(bool t) { 
  for( int iorder=0 ; iorder<m_order ; iorder++ ) {
    for( int iobs=0 ; iobs<Nobs_internal() ; iobs++ ) m_grids[iorder][iobs]->reweight(t);       
  }
  return t;
}


bool exists( const std::string& filename ) { 
  struct stat sb;
  if ( stat( filename.c_str(), &sb)==0 ) return true; // && S_ISREG(sb.st_mode ))
  else return false;
}


// dump to file
void appl::grid::Write(const std::string& filename, 
		       const std::string& dirname, 
		       const std::string& pdfname) 
{ 

  std::cout << "appl::grid::Write() " << filename << "\tdirname " << dirname << "\tpdfname " << pdfname << std::endl; 

  if ( exists( filename ) ) { 
    std::string filename_save = filename + "-save";
    if ( std::rename( filename.c_str(), filename_save.c_str() ) ) std::cerr << "could not rename grid file " << filename << std::endl;
  } 

  if ( pdfname!="" ) shrink( pdfname, m_genpdf[0]->getckmcharge() );

  //  std::cout << "grid::Write() writing to file " << filename << std::endl;
  TFile rootfile(filename.c_str(),"recreate");

  //  std::cout << "pwd=" << gDirectory->GetName() << std::endl;

  Directory d(dirname);
  d.push();
  
  //  std::cout << "pwd=" << gDirectory->GetName() << std::endl;

  // write the name of the transform pair and the
  // generalised pdf
  TFileString _tags("Tags");
  _tags.add(m_transform);
  _tags.add(m_genpdfname);
  _tags.add(m_version);
  if ( m_documentation!="" ) _tags.add(m_documentation);
  _tags.Write();

  //  TH1D* _transform = new TH1D("Transform", m_transform.c_str(), 1, 0, 1);  
  //  _transform->Write();

  //  TH1D* _genpdfname = new TH1D("GenPDF", m_genpdfname.c_str(), 1, 0, 1);
  //  _genpdfname->Write();


  //  std::cout << "state std::vector=" << std::endl;

  // state information
  TVectorT<double>* setup=new TVectorT<double>(12); // add a few extra just in case 
  (*setup)(0) = m_run;
  (*setup)(1) = ( m_optimised  ? 1 : 0 );
  (*setup)(2) = ( m_symmetrise ? 1 : 0 );
  (*setup)(3) =   m_leading_order ;
  (*setup)(4) =   m_order ;
  (*setup)(5) =   m_cmsScale ;
  (*setup)(6) = ( m_normalised ? 1 : 0 );
  (*setup)(7) = ( m_applyCorrections ? 1 : 0 );

  if ( m_genpdf[0]->getckmsum().size()==0 ) (*setup)(8) = 0;
  else                                      (*setup)(8) = 1;

  (*setup)(9) = (int)m_type;

  (*setup)(10) = m_userdata.size();

  setup->Write("State");
  
  if ( (*setup)(8) == 1 ) { 
    
    /// no longer write out squared ckm matrix - just use the 3x3
    TVectorT<double>* ckmflat = new TVectorT<double>(9);
    const std::vector<std::vector<double> >& _ckm = m_genpdf[0]->getckm();

    for ( int ic=0 ; ic<3 ; ic++ ) { 
      for ( int id=0 ; id<3 ; id++ ) (*ckmflat)(ic*3+id) = _ckm[ic][id];
    }

    ckmflat->Write("CKM");

  }

  /// encode the pdf combination if appropriate
  
  if ( contains( m_genpdfname, ".config" ) ) { 
    
    // find out if we have one combination per order or one overall
    
    std::vector<std::string> namevec = parse( m_genpdfname, ":" );
    
    /// now write out all the ones we need
    
    std::string label = "Combinations";
    
    for ( unsigned i=0 ; i<namevec.size() && i<unsigned(m_order) ; i++ ) {  

      std::vector<int>   combinations = dynamic_cast<lumi_pdf*>(m_genpdf[i])->serialise();
      TVectorT<double>* _combinations = new TVectorT<double>(combinations.size());
      for ( unsigned ic=0 ; ic<combinations.size() ; ic++ ) { 
	if ( combinations[ic]<0 ) (*_combinations)(ic) = double(combinations[ic]-0.5);
	else                      (*_combinations)(ic) = double(combinations[ic]+0.5);
      }
      
      _combinations->Write( label.c_str() );
      
      std::cout << "writing " << m_genpdf[i]->name() << std::endl;
      
      label += "N";  /// add an N for each order, N-LO, NN-LO etc
    }
  } 
  
  //  std::cout << "grids Nobs = " << Nobs_internal() << std::endl;
  
  untrim();
  int untrim_size = size();
  trim();
  int trim_size = size();
  std::cout <<"grid::Write()"
	    << "size(untrimmed)=" << untrim_size/1024 << " kB"
	    << "\tsize(trimmed)=" <<   trim_size/1024 << " kB"
	    << " (" << 0.1*int(trim_size*1000./untrim_size) << "%)" << std::endl;
  

  // internal grids
  for( int iorder=0 ; iorder<m_order ; iorder++ ) {
    for( int iobs=0 ; iobs<Nobs_internal() ; iobs++ ) {
      char name[128];  sprintf(name, "weight[alpha-%d][%03d]", iorder, iobs);
      // std::cout << "writing grid " << name << std::endl;
      //   _size += m_grids[iorder][iobs]->size();
      m_grids[iorder][iobs]->write(name);
      //   trim_size += m_grids[iorder][iobs]->size();
    }
  }
 
  
  //  d.pop();

  //  std::cout << "reference" << std::endl;

  TH1D* reference          = 0;
  TH1D* reference_internal = 0;


  if ( m_obs_bins_combined != m_obs_bins )  { 
    reference = (TH1D*)m_obs_bins_combined->Clone("reference");
    reference->SetDirectory(0);

    reference_internal = (TH1D*)m_obs_bins->Clone("reference_internal");
    reference_internal->SetDirectory(0);
  }
  else { 
    reference = (TH1D*)m_obs_bins->Clone("reference");
    reference->SetDirectory(0);
  }

  /// grrr, what is this? This whole normalisation issue is a real pain - 
  /// we need to address this - but how to do it in a backwards compatible way?
  /// will need to check the grid version and do things differently probably

  //  std::cout << "normalised() " << getNormalised() << "\tread " << m_read << std::endl; 
  
  //  if ( !getNormalised() || m_read )  if ( run() ) reference->Scale(1/double(run()));
  if ( run() ) { 
    reference->Scale(1/double(run()));
    if ( reference_internal ) reference_internal->Scale(1/double(run()));
  }

  // if ( run() ) reference->Scale(1/double(run()));
  reference->Write();
  delete reference;
  if ( reference_internal ) { 
    reference_internal->Write();
    delete reference_internal;
  }

  //  std::cout << "corrections" << std::endl;

  /// correction histograms

  if ( m_corrections.size()>0 ) {

    /// Fixme: should add the labels to the actual corrections rather than save separately
    /// write labels
    TFileVector* corrections = new TFileVector("Corrections");
    for ( unsigned i=0 ; i<m_corrections.size() ; i++ )  corrections->add( m_corrections[i] );    
    corrections->Write("Corrections");

    /// write actual corrections
    TFileString correctionLabels("CorrectionLabels");
    for ( unsigned i=0 ; i<m_correctionLabels.size() ; i++ )  correctionLabels.add( m_correctionLabels[i] );
    correctionLabels.Write("CorrectionLabels");

  }

  /// now write out vector of bins to be combined if this has been set
  if ( m_combine.size()>0 ) { 
    TVectorT<double>* _combine = new TVectorT<double>(m_combine.size()); 
    for ( unsigned i=m_combine.size() ; i-- ; ) (*_combine)(i) = m_combine[i]+0.5; /// NB: add 0.5 to prevent root double -> int rounding errors
    _combine->Write( "CombineBins" );
  }


  /// now write out the user data

  if ( m_userdata.size() ) { 
    TVectorT<double>* userdata=new TVectorT<double>(m_userdata.size()); // add a few extra just in case 
    for ( unsigned i=0 ; i<m_userdata.size() ; i++ ) (*userdata)(i) = m_userdata[i];
    userdata->Write("UserData");
  }


  //  std::cout << "close file" << std::endl;

  d.pop();
  rootfile.Close();

  //  std::cout << "written" << std::endl;
}



// takes pdf as the pdf lib wrapper for the pdf set for the convolution.
// type specifies which sort of partons should be included:

std::vector<double> appl::grid::vconvolute(void (*pdf)(const double& , const double&, double* ), 
					   double (*alphas)(const double& ), 
					   int     nloops, 
					   double  rscale_factor,
					   double  fscale_factor,
					   double Escale )
{ 
  return vconvolute( pdf, 0, alphas, nloops, rscale_factor, fscale_factor, Escale );
}

std::vector<double> appl::grid::vconvolute(void (*pdf1)(const double& , const double&, double* ), 
					   void (*pdf2)(const double& , const double&, double* ), 
					   double (*alphas)(const double& ), 
					   int     nloops, 
					   double  rscale_factor,
					   double  fscale_factor,
					   double Escale )
{ 

  NodeCache cache1( pdf1 );
  NodeCache cache2;

  cache1.reset();

  NodeCache* _pdf1 = &cache1;
  NodeCache* _pdf2 = 0;
  
  if ( pdf2!=0 && pdf1!=pdf2 ) { 
    cache2 = NodeCache( pdf2 );
    cache2.reset();
  
    _pdf2    = &cache2;
  }

 

  //  struct timeval _ctimer = appl_timer_start();
  
  double Escale2 = 1;
 
  if ( Escale!=1 ) Escale2 = Escale*Escale;
  
  std::vector<double> hvec;

  double invNruns = 1;
  if ( (!m_normalised) && run() ) invNruns /= double(run());

  //  TH1D* h = new TH1D(*m_obs_bins);
  //  h->SetName("xsec");

#ifdef HAVE_HOPPET
  // check if we need to use the splitting function, and if so see if we 
  // need to initialise it again, and do so if required
  if ( fscale_factor!=1 || m_dynamicScale ) {

    if ( pdf2==0 || pdf1==pdf2 ) { 

      if ( hoppet == 0 ) { 
	double Qmax = 15000;
	if ( m_cmsScale>Qmax ) Qmax = m_cmsScale;
	hoppet = new hoppet_init( Qmax );
      } 

      bool newpdf = hoppet->compareCache( pdf1 );
      
      if ( newpdf ) hoppet->fillCache( pdf1 );

    }

  }
#endif

  std::string label;
  
  if ( nloops>=m_order ) { 
    std::cerr << "too many loops for grid nloops=" << nloops << "\tgrid=" << m_order << std::endl;   
    return hvec;
  } 
  
  
  if ( m_type==STANDARD ) { 

    static bool first = true;
 
    if ( first && m_dynamicScale ) std::cout << "** grid::vconvolute() emulating dynamic scale **" << std::endl;

    //    std::cout << "standard convolution" << std::endl;

    for ( int iobs=0 ; iobs<Nobs_internal() ; iobs++ ) {  

      /// here we see whether we need to emulate a dynamic scale by simply 
      /// changing the renormalisation and factorisation scale terms to give 
      /// what a dynamic scale would be in this bin
 
      if ( m_bin!=-1 && iobs!=m_bin ) continue;

      double dynamic_factor = 1;

      if ( m_dynamicScale ) {
	double var = m_obs_bins->GetBinCenter(iobs+1);
	dynamic_factor = var/m_dynamicScale;
	//	if ( first ) std::cout << "grid::vconvolute() bin " << iobs << "\tscale " << var << "\tdynamicScale " << m_dynamicScale << "\t scale factor " << dynamic_factor << std::endl;
      } 

      /// now do the convolution proper

      double dsigma = 0;
     
      if ( nloops==0 ) {
	label = "lo      ";

	/// leading order cross section

	if ( subproc()==-1 ) {  
	  dsigma = m_grids[0][iobs]->convolute( _pdf1, _pdf2, m_genpdf[0], alphas, m_leading_order, 0, dynamic_factor*rscale_factor, dynamic_factor*rscale_factor, Escale);
	}
	else { 
	  /// fixme: for the subproceses, this is technically incorrect - the "LO" contribution 
	  ///        includes the scale dependent "NLO" terms that are proportional to the LO 
	  ///        coefficient functions - it could use the strict "LO" part without the scale 
	  ///        dependent parts using order "0" rather than order "1" but see the comment 
	  ///        for the "NLO only" contribution. The reason for this is that the subprocesses
	  ///        can be different for LO and NLO, so this NLO part with "LO coefficients", 
	  ///        can have different subprocesses from the actual NLO part, so the correct 
	  ///        LO/NLO separation is only guaranteed for the full convolution, and not by 
	  ///        subprocess
	  dsigma = m_grids[0][iobs]->convolute( _pdf1, _pdf2, m_genpdf[0], alphas, m_leading_order, 1, dynamic_factor*rscale_factor, dynamic_factor*rscale_factor, Escale);
	}

      }
      else if ( nloops==1 ) { 
	label = "nlo     ";
	// next to leading order cross section
	// std::cout << "convolute() nloop=1" << std::endl;
	// leading order contribution and scale dependent born dependent terms
	double dsigma_lo  = m_grids[0][iobs]->convolute( _pdf1, _pdf2, m_genpdf[0], alphas, m_leading_order, 1, dynamic_factor*rscale_factor, dynamic_factor*fscale_factor, Escale);
	// std::cout << "dsigma_lo=" << dsigma_lo << std::endl;
	// next to leading order contribution
	//      double dsigma_nlo = m_grids[1][iobs]->convolute(pdf, m_genpdf, alphas, m_leading_order+1, 0);
	// GPS: the NLO piece must use the same rscale_factor and fscale_factor as
	//      the LO piece -- that's the convention that defines how NLO calculations
	//      are done.
	double dsigma_nlo = m_grids[1][iobs]->convolute( _pdf1, _pdf2, m_genpdf[1], alphas, m_leading_order+1, 0, dynamic_factor*rscale_factor, dynamic_factor*fscale_factor, Escale);
	// std::cout << "dsigma_nlo=" << dsigma_nlo << std::endl;
	dsigma = dsigma_lo + dsigma_nlo;
      }
      else if ( nloops==-1 ) {
	label = "nlo only";

	// nlo contribution only 
	if ( subproc()==-1 ) { 
	  double dsigma_log = m_grids[0][iobs]->convolute( _pdf1, _pdf2, m_genpdf[0], alphas, m_leading_order, -1, dynamic_factor*rscale_factor, dynamic_factor*fscale_factor, Escale);
	  double dsigma_nlo = m_grids[1][iobs]->convolute( _pdf1, _pdf2, m_genpdf[1], alphas, m_leading_order+1,  0, dynamic_factor*rscale_factor, dynamic_factor*fscale_factor, Escale);
	  dsigma = dsigma_nlo + dsigma_log;
	}
	else { 
	  /// fixme: this is technically incorrect - the "LO" component contains the 
	  ///        scale dependent NLO contribution dependent on the LO coefficient
	  ///        functions. This part is difficult to include for individual 
	  ///        subprocesses, since different subprocesses can be present 
	  ///        at LO and NLO, so speifying subprocess X at NLO does not 
	  ///        neccessarily correspond to subprocess X at LO, so adding the 
	  ///        subprocesses - so these terms are only strict LO And NLO when 
	  ///        *not* specifying subprocess
	  dsigma = m_grids[1][iobs]->convolute( _pdf1, _pdf2, m_genpdf[1], alphas, m_leading_order+1,  0, dynamic_factor*rscale_factor, dynamic_factor*fscale_factor, Escale);
	}

      } 
      else if ( nloops==2 ) {
	// FIXME: not implemented completely yet - no scale variation
	//      return hvec;
	label = "nnlo    ";
	// next to next to leading order contribution 
	// NB: NO scale dependendent parts so only  muR=muF=mu
	double dsigma_lo  = m_grids[0][iobs]->convolute( _pdf1, _pdf2, m_genpdf[0], alphas, m_leading_order, 0);
	// next to leading order contribution      
	double dsigma_nlo = m_grids[1][iobs]->convolute( _pdf1, _pdf2, m_genpdf[1], alphas, m_leading_order+1, 0);
	// next to next to leading order contribution
	double dsigma_nnlo = m_grids[2][iobs]->convolute( _pdf1, _pdf2, m_genpdf[2], alphas, m_leading_order+2, 0);
	dsigma = dsigma_lo + dsigma_nlo + dsigma_nnlo;
      }
      else if ( nloops==-2 ) {
	label = "nnlo only";
	// next to next to leading order contribution
	dsigma = m_grids[2][iobs]->convolute( _pdf1, _pdf2, m_genpdf[2], alphas, m_leading_order+2, 0);
      }
      else { 
	throw grid::exception( std::cerr << "invalid value for nloops " << nloops ); 
      }

      double deltaobs = m_obs_bins->GetBinLowEdge(iobs+2)-m_obs_bins->GetBinLowEdge(iobs+1);      
      hvec.push_back( invNruns*Escale2*dsigma/deltaobs );
    }

    first = false;
    
  }
  else if ( m_type==AMCATNLO ) {  

    //    std::cout << "amc@NLO convolution" << std::endl;
    
    for ( int iobs=0 ; iobs<Nobs_internal() ; iobs++ ) {  

      double dsigma = 0; 
      
      if ( nloops==0 ) {
	  /// this is the amcatnlo LO calculation (without FKS shower)
	  label = "lo";
	  /// work out how to call from the igrid - maybe just implement additional 
	  double dsigma_B = m_grids[3][iobs]->amc_convolute( _pdf1, _pdf2, m_genpdf[3], alphas, m_leading_order,   0, rscale_factor, fscale_factor,  Escale );
 
   	  dsigma = dsigma_B;
      }
      else if ( nloops==1 || nloops==-1 ) {
	  /// this is the amcatnlo NLO calculation (without FKS shower)
	  /// Next-to-leading order contribution
	  label = "nlo only"; /// for the time being ...

	  // Scale independent contribution
	  double dsigma_0 = m_grids[0][iobs]->amc_convolute( _pdf1, _pdf2, m_genpdf[0], alphas, m_leading_order+1, 0,  rscale_factor, fscale_factor,  Escale );
	  dsigma = dsigma_0;

	  // Renormalization scale dependent contribution
	  if ( rscale_factor!=1 ) { 
	    double dsigma_R = m_grids[1][iobs]->amc_convolute( _pdf1, _pdf2, m_genpdf[1], alphas, m_leading_order+1, 0,  rscale_factor, fscale_factor,  Escale );
	    dsigma += dsigma_R*std::log(rscale_factor*rscale_factor);
	  }

	  // Factorization scale dependent contribution
	  if ( fscale_factor!=1 ) { 
	    double dsigma_F = m_grids[2][iobs]->amc_convolute( _pdf1, _pdf2, m_genpdf[2], alphas, m_leading_order+1, 0,  rscale_factor, fscale_factor,  Escale );
	    dsigma += dsigma_F*std::log(fscale_factor*fscale_factor);
	  }
      
	  /// Add the LO contribution if we want full NLO 
	  /// rather than specific NLO contribution only  
	  if ( nloops==1 ) { 
	    /// this is the amcatnlo NLO calculation (without FKS shower)
	    label = "nlo";
	    /// work out how to call from the igrid - maybe just implement additional 
	    /// convolution routines and call them here
	    double dsigma_B = m_grids[3][iobs]->amc_convolute( _pdf1, _pdf2, m_genpdf[3], alphas, m_leading_order,   0,  rscale_factor, fscale_factor,  Escale );	
	    dsigma += dsigma_B;
	  }
      }
      else if ( nloops==-2 ) { 
        /// Only the convolution from the W0 grid
        label = "nlo_w0";
	double dsigma_0 = m_grids[0][iobs]->amc_convolute( _pdf1, _pdf2, m_genpdf[0], alphas, m_leading_order+1, 0,  rscale_factor, fscale_factor,  Escale );
	dsigma = dsigma_0;
      }
      else if ( nloops==-3 ) {
	/// Only the convolution from the WR grid
	label = "nlo_wR";
	double dsigma_R = m_grids[1][iobs]->amc_convolute( _pdf1, _pdf2, m_genpdf[1], alphas, m_leading_order+1, 0, rscale_factor, fscale_factor,  Escale );
	dsigma = dsigma_R * std::log(rscale_factor*rscale_factor)  ;
      }
      else if ( nloops==-4 ) { 
        /// Only the convolution from the WF grid
        label = "nlo_wF";
	double dsigma_F = m_grids[2][iobs]->amc_convolute( _pdf1, _pdf2, m_genpdf[2], alphas, m_leading_order+1, 0,  rscale_factor, fscale_factor,  Escale );
	dsigma = dsigma_F * std::log(fscale_factor*fscale_factor) ;
      }
      else { 
	throw grid::exception( std::cerr << "invalid value for nloops " << nloops ); 
      }

      double deltaobs = m_obs_bins->GetBinLowEdge(iobs+2)-m_obs_bins->GetBinLowEdge(iobs+1);      
      hvec.push_back( invNruns*Escale2*dsigma/deltaobs );
    }
  }
  else if ( m_type == SHERPA ) { 
    
    //    std::cout << "sherpa convolution" << std::endl;

    for ( int iobs=0 ; iobs<Nobs_internal() ; iobs++ ) {  
    
      double dsigma = 0; 
  
      if ( nloops==0 ) {
	label = "lo      ";
	// leading order cross section
	dsigma = m_grids[0][iobs]->convolute( _pdf1, _pdf2, m_genpdf[0], alphas, m_leading_order, 0, 1, 1, Escale);
      }
      else if ( nloops==1 ) { 
	label = "nlo     ";
	// next to leading order cross section
	// leading order contribution and scale dependent born dependent terms

	// will eventually add the other nlo terms ...
	double dsigma_lo  = m_grids[0][iobs]->convolute( _pdf1, _pdf2, m_genpdf[0], alphas, m_leading_order,   0, rscale_factor, fscale_factor, Escale);
	double dsigma_nlo = m_grids[1][iobs]->convolute( _pdf1, _pdf2, m_genpdf[1], alphas, m_leading_order+1, 0, rscale_factor, fscale_factor, Escale);
  
	dsigma = dsigma_lo + dsigma_nlo;
      }
      else if ( nloops==-1 ) { 
	label = "nlo     ";
	// next to leading order cross section
	// leading order contribution and scale dependent born dependent terms

	double dsigma_nlo = m_grids[1][iobs]->convolute( _pdf1, _pdf2, m_genpdf[1], alphas, m_leading_order+1, 0, rscale_factor, fscale_factor, Escale);

	dsigma = dsigma_nlo;
      }


      double deltaobs = m_obs_bins->GetBinLowEdge(iobs+2)-m_obs_bins->GetBinLowEdge(iobs+1);      
      hvec.push_back( invNruns*Escale2*dsigma/deltaobs );
    }

  }

  /// now combine bins if required ...

  std::vector<bool> applied(m_corrections.size(),false);

  /// apply corrrections on the *uncombined* bins
  if ( getApplyCorrections() ) applyCorrections(hvec, applied);
  else { 
    for ( unsigned i=m_corrections.size() ; i-- ; ) if ( getApplyCorrection(i) ) applied[i] = applyCorrection(i,hvec);
  }


  /// combine bins if required
  if ( m_combine.size()!=0 ) { 
    combineBins( hvec );

    /// apply additional corrrections on the *combined* bins
    /// NB: Only apply those that have not already been applied
    if ( getApplyCorrections() ) applyCorrections(hvec, applied);
    else { 
      for ( unsigned i=m_corrections.size() ; i-- ; ) if ( getApplyCorrection(i) && !applied[i] ) applied[i] = applyCorrection(i,hvec);
    }
  }

  /// check all corrections have been applied correctly
  if ( getApplyCorrections() ) { 
    unsigned appliedcorrections = 0;
    for ( unsigned i=applied.size() ; i-- ; ) if ( applied[i] ) appliedcorrections++;
    if ( appliedcorrections!=applied.size() ) throw grid::exception( std::cerr << "correction vector size does not match data "  ); 
  }
  else { 
    unsigned Ncorrections = 0;
    unsigned appliedcorrections = 0;
    for ( unsigned i=m_corrections.size() ; i-- ; ) { 
      if ( getApplyCorrection(i) ) { 
	Ncorrections++;
	if ( applied[i] ) appliedcorrections++;
      }
    }
    if ( appliedcorrections!=Ncorrections ) throw grid::exception( std::cerr << "correction vector size does not match data "  ); 
  }

  //  double _ctime = appl_timer_stop(_ctimer);
  //  std::cout << "grid::convolute() " << label << " convolution time=" << _ctime << " ms" << std::endl;
  
  cache1.stats();
  if ( cache2.ncalls() ) cache2.stats();
  
  return hvec;
}




TH1D* appl::grid::convolute(void (*pdf)(const double& , const double&, double* ), 
			    double (*alphas)(const double& ), 
			    int     nloops, 
			    double  rscale_factor,
			    double  fscale_factor,
			    double Escale )
{
  return convolute( pdf, 0, alphas, nloops, rscale_factor, fscale_factor, Escale );
}




/// a dirty hack to tell the sub grid it should only 
/// use a single subprocess

std::vector<double> appl::grid::vconvolute_subproc(int subproc,
						   void (*pdf)(const double& , const double&, double* ), 
						   double (*alphas)(const double& ), 
						   int     nloops, 
						   double  rscale_factor, double Escale ) 
{ 
  /// set the subprocess index - this is tested by the 
  /// igrid convolution
  m_subproc = subproc;
  std::vector<double> xsec = vconvolute( pdf, 0, alphas, nloops, rscale_factor, rscale_factor, Escale ); 
  m_subproc = -1;

  return xsec;

}



double appl::grid::vconvolute_bin(int bin,
				  void (*pdf)(const double& , const double&, double* ), 
				  double (*alphas)(const double&) ) {
  /// set the subprocess index - this is tested by the 
  /// igrid convolution
  m_bin = bin;
  std::vector<double> xsec = vconvolute( pdf, alphas );
  m_bin = -1;
  return xsec[bin];
}





TH1D* appl::grid::convolute(void (*pdf1)(const double& , const double&, double* ), 
			    void (*pdf2)(const double& , const double&, double* ), 
			    double (*alphas)(const double& ), 
			    int     nloops, 
			    double  rscale_factor,
			    double  fscale_factor,
			    double Escale ) 
{

  //  int nbins = m_obs_bins->GetNbinsX();

  TH1D* h = 0;
  if ( m_combine.size() ) { 
    
    //    nbins = m_combine.size();

    std::vector<double> limits(m_combine.size()+1);
    
    int i=0;
    limits[0] = m_obs_bins->GetBinLowEdge(i+1);
    for ( unsigned ib=0 ; ib<m_combine.size() ; ib++) { 
      i += m_combine[ib]; 
      limits[ib+1] = m_obs_bins->GetBinLowEdge(i+1);
    }

    h = new TH1D("xsec", "xsec", m_combine.size(), &limits[0] );
    h->SetDirectory(0);
  }
  else { 
    h = new TH1D(*m_obs_bins);
    h->SetName("xsec");
    h->SetDirectory(0);
  }
   
    
  std::vector<double> dvec = vconvolute( pdf1, pdf2, alphas, nloops, rscale_factor, fscale_factor, Escale );

  for ( unsigned i=0 ; i<dvec.size() ; i++ ) { 
      h->SetBinContent( i+1, dvec[i] );
      h->SetBinError( i+1, 0 );
  }
  
  return h;
  
}





TH1D* appl::grid::convolute_subproc(int subproc,
				    void (*pdf)(const double& , const double&, double* ), 
				    double (*alphas)(const double& ), 
				    int     nloops, 
				    double  rscale_factor, double Escale ) {
  
    TH1D* h = new TH1D(*m_obs_bins);
    h->SetName("xsec");
    
    std::vector<double> dvec = vconvolute_subproc( subproc, pdf, alphas, nloops, rscale_factor, Escale );
    
    for ( unsigned i=0 ; i<dvec.size() ; i++ ) { 
      h->SetBinContent( i+1, dvec[i] );
      h->SetBinError( i+1, 0 );
    }
  
    return h;
  
}





void appl::grid::optimise(bool force) {
  if ( !force && m_optimised ) return;
  m_optimised = true;
  m_read = false;
  for ( int iorder=0 ; iorder<m_order ; iorder++ ) { 
    for ( int iobs=0 ; iobs<Nobs_internal() ; iobs++ )  { 
      std::cout << "grid::optimise() bin " << iobs << "\t";
      m_grids[iorder][iobs]->optimise();
    }
  }
  m_obs_bins->Reset();
}



void appl::grid::optimise(int NQ2, int Nx) {  optimise(NQ2, Nx, Nx);  }



void appl::grid::optimise(int NQ2, int Nx1, int Nx2) {
  m_optimised = true;
  m_read = false;
  for ( int iorder=0 ; iorder<m_order ; iorder++ ) { 
    for ( int iobs=0 ; iobs<Nobs_internal() ; iobs++ )  { 
      std::cout << "grid::optimise() bin " << iobs << "\t";
      m_grids[iorder][iobs]->optimise(NQ2, Nx1, Nx2);
    }
  }
  m_obs_bins->Reset();
}




// redefine the limits by hand
void appl::grid::redefine(int iobs, int iorder,
			  int NQ2, double Q2min, double Q2max, 
			  int Nx,  double  xmin, double  xmax ) 
{ 
  
  if  ( iorder>=m_order ) { 
    std::cerr << "grid does not extend to this order" << std::endl;
    return;
  }
  
  if ( iobs<0 || iobs>=Nobs_internal() ) { 
    std::cerr << "observable bin out of range" << std::endl;
    return;
  }
  
  if ( iorder==0 ) { 
    std::cout << "grid::redefine() iobs=" << iobs 
	      << "NQ2="  << NQ2 << "\tQmin=" << std::sqrt(Q2min) << "\tQmax=" << std::sqrt(Q2max) 
	      << "\tNx=" << Nx  << "\txmin=" <<            xmin  << "\txmax=" <<            xmax << std::endl; 
  }
  
  igrid* oldgrid = m_grids[iorder][iobs];
  
  //  m_grids[iorder][iobs]->redefine(NQ2, Q2min, Q2max, Nx, xmin, xmax);

  m_grids[iorder][iobs] = new igrid(NQ2, Q2min, Q2max, oldgrid->tauorder(),
				    Nx,  xmin,  xmax,  oldgrid->yorder(), 
				    oldgrid->transform(), m_genpdf[iorder]->Nproc());

  m_grids[iorder][iobs]->setparent( this ); 

  delete oldgrid;
}
  


void appl::grid::setBinRange(int ilower, int iupper, double xScaleFactor) { 
  if ( ilower>=0 && iupper <Nobs_internal() ) {  
    double lower = getReference()->GetBinLowEdge(ilower+1);
    double upper = getReference()->GetBinLowEdge(iupper+2); 
    setRange( lower, upper, xScaleFactor );
  }
}



/// Fixme: need to fix this so that if the m_combine vector has values 
///        then the range of this vector is also modified
///        and then the combined reference recalculated:
///        at the moment, if the range is changed, the combine vector
///        needs to be recalculated and reset by hand

void appl::grid::setRange(double lower, double upper, double xScaleFactor) { 
  
  std::cout << "grid::SetRange() " << lower << " " << upper << std::endl; 

  std::vector<bool>   used;
  std::vector<double> limits;
  std::vector<double> contents;
  std::vector<double> errors;

  m_combine = std::vector<int>(0);

  /// get the occupied bins
  //  int Nbins = 0;
  double last = 0;
  for ( int i=1 ; i<=m_obs_bins->GetNbinsX() ; i++ ) { 
    double bin = m_obs_bins->GetBinCenter(i);
    if ( bin>lower && bin<upper ) { 
      limits.push_back( m_obs_bins->GetBinLowEdge(i) );
      contents.push_back( m_obs_bins->GetBinContent(i) );
      errors.push_back( m_obs_bins->GetBinError(i) );

      last = m_obs_bins->GetBinLowEdge(i+1);
      used.push_back(true);
    }
    else { 
      //      std::cout << "skip bin " << i << " : " << m_obs_bins->GetBinLowEdge(i) << " - " << m_obs_bins->GetBinLowEdge(i+1) << std::endl;
      used.push_back(false);
    }
  }

  int firstbin = 0;
  int lastbin  = 0;

  for ( unsigned i=0 ; i<used.size() ; i++ ) { 
    if ( used[i] ) { 
      firstbin = i;
      break;
    }
  }
  
  for ( unsigned i=used.size() ; i-- ; ) { 
    if ( used[i] ) { 
      lastbin = i;
      break;
    }
  }

  std::cout << "grid::SetRange() bins chosen " << firstbin << " - " << lastbin << std::endl;


  /// copy the range of the reference histogram
  if ( limits.size()>0 ) limits.push_back( last );
  else { 
    throw grid::exception( std::cerr << "new range does not include any bins"  ); 
  }

  if ( xScaleFactor!=1 ) { 
    for ( unsigned i=0 ; i<limits.size(); i++ ) limits[i] *= xScaleFactor;
  }

  /// delete combined reference if it exists

  if ( m_obs_bins_combined != m_obs_bins ) delete m_obs_bins_combined;
  

  /// save bins somewhere so can overwrite them 
  TH1D* h = m_obs_bins;
  
  m_obs_bins = new TH1D( h->GetTitle(), h->GetName(), limits.size()-1, &(limits[0]) );
  
  for ( int i=0 ; i<m_obs_bins->GetNbinsX() ; i++ ) { 
    m_obs_bins->SetBinContent( i+1, contents[i] );
    m_obs_bins->SetBinError( i+1, errors[i] );
  }

  /// copy the igrids for the observable bins in the range 

  igrid** grids[appl::MAXGRIDS];

  /// save old grids
  for ( int iorder=0 ; iorder<m_order ; iorder++ ) grids[iorder] = m_grids[iorder];
  
  int _Nobs = m_obs_bins->GetNbinsX();

  for ( int iorder=0 ; iorder<m_order ; iorder++ ) { 
    m_grids[iorder] = new igrid*[_Nobs];
    int iobs = 0;
    for ( int igrid=0 ; igrid<h->GetNbinsX() ; igrid++ ) {
      if ( used[igrid] ) m_grids[iorder][iobs++] = grids[iorder][igrid];
      else               delete grids[iorder][igrid];                           
    }
  }
  
  m_obs_bins_combined = m_obs_bins; 

  delete h;

}


/// methods to handle the documentation
void appl::grid::setDocumentation(const std::string& s) { m_documentation = s; }
void appl::grid::addDocumentation(const std::string& s) {   
  if ( m_documentation.size() ) m_documentation += s;
  else                          setDocumentation(s);    
}






/// methods to handle bin-by-bin corrections

/// add a correction as a std::vector
void appl::grid::addCorrection( std::vector<double>& v, const std::string& label, bool ) {
  //  std::cout << "addCorrections(vector) " << v.size() << " " << m_obs_bins->GetNbinsX() << std::endl;
  if ( v.size()==unsigned(m_obs_bins->GetNbinsX()) || v.size()==unsigned(m_obs_bins_combined->GetNbinsX()) ) {
    m_corrections.push_back(v);
    m_correctionLabels.push_back(label);
    m_applyCorrection.push_back(false);
    //  std::cout << "appl::grid::addCorrection(vector) now " << m_corrections.size() << " corrections" << std::endl;
  }
}


/// add a correction by histogram
void appl::grid::addCorrection(TH1D* h, const std::string& label, double scale, bool ) {
  
  TH1D* hobs = 0;
  
  if      ( h->GetNbinsX()==m_obs_bins->GetNbinsX() )          hobs = m_obs_bins;
  else if ( h->GetNbinsX()==m_obs_bins_combined->GetNbinsX() ) hobs = m_obs_bins_combined;


  if ( hobs ) { 
    for ( int i=1 ; i<=h->GetNbinsX()+1 ; i++ ) { 
      if ( std::fabs(h->GetBinLowEdge(i+1)*scale-hobs->GetBinLowEdge(i+1))>1e-10 ) { 
	std::cerr << "bins " << h->GetBinLowEdge(i+1) << " " << hobs->GetBinLowEdge(i+1) << std::endl; 
	std::cerr << "grid::addCorrection(TH1D* h): bin mismatch, not adding correction" << std::endl;
	return;
      }
    }

    std::vector<double> v(h->GetNbinsX());
    for ( int i=0 ; i<h->GetNbinsX() ; i++ ) v[i] = h->GetBinContent(i+1);
    if ( label=="" ) addCorrection(v, h->GetName());
    else             addCorrection(v, label);
  }
  else { 
    std::cerr << "grid::addCorrection(TH1D* h): bin mismatch, not adding correction" << std::endl;
  }
  
}



// find the number of words used for storage
int appl::grid::size() const { 
    int _size = 0;
    for( int iorder=0 ; iorder<2 ; iorder++ ) {
      for( int iobs=0 ; iobs<Nobs_internal() ; iobs++ ) _size += m_grids[iorder][iobs]->size();
    }
    return _size;
}


/// apply corrections to a std::vector
void appl::grid::applyCorrections(std::vector<double>& v, std::vector<bool>& applied) {
 
  if ( applied.size()!=m_corrections.size() ) throw grid::exception( std::cerr << "wrong number of corrections expected" ); 
 
  for ( unsigned i=m_corrections.size() ; i-- ; ) { 
 
    std::vector<double>& correction = m_corrections[i];
    
    if ( applied[i] || v.size()!=correction.size() ) continue; /// correction applied already or wrong size     

    for ( unsigned j=v.size() ; j-- ; ) v[j] *= correction[j];
    applied[i] = true;
  }
  //  std::cout << "grid::applyCorrections(vector) done" << std::endl;
}




/// apply correction to a std::vector
bool appl::grid::applyCorrection(unsigned i, std::vector<double>& v) {

  if ( i>=m_corrections.size() ) throw grid::exception( std::cerr << "correction index out of range"  ); 
 
  std::vector<double>& correction = m_corrections[i];

  if ( v.size()!=correction.size() ) return false; /// wrong size

  for ( unsigned k=v.size() ; k-- ; ) v[k] *= correction[k];
  return true;
}





/// do a deep comparison of all the different sub processes - if any 
/// are the same, then remove them
void appl::grid::shrink(const std::string& name, int ckmcharge) { 

  std::cout << "appl::grid::shrink()" << std::endl;

  std::string label[3] = { "LO", "NLO", "NNLO" };

  std::string genpdfname="";

  std::vector<int> keep[2];

  bool found = false;


  /// loop over orders 
  for( int iorder=0 ; iorder<m_order ; iorder++ ) {

    std::cout << "appl::grid::shrink() order " << iorder << std::endl;
    
    std::vector< std::vector<int> > pdf_combinations;
    pdf_combinations.reserve( Nobs_internal() );
    
    //    std::cout << "appl::grid::shrink() observable bins " << Nobs_internal() << std::endl;

    /// ... for each observable bin ...
 
    for( int iobs=0 ; iobs<Nobs_internal() ; iobs++ ) { 
      
      //      std::cout << "shrink() order: " << iorder << "\t obs: " << iobs;
      
      igrid* ig = m_grids[iorder][iobs];
    
      std::map< int, std::vector<int> > same;

      std::set<int> duplicates;

      std::set<int> empty;

      for ( int i=0 ; i<ig->SubProcesses() ; i++ ) { 

	if ( duplicates.find(i)!=duplicates.end() ) continue;

	//	std::pair< std::map< int, std::vector<int> >, bool> itr = 
	//	  same.insert( std::map< int, std::vector<int> >::value_type( i, std::vector<int>() ) );
	//	
	//  if ( itr->second==false ) std::cerr << "OH SHIT!!!" << std::endl;

	int isize = ig->weightgrid(i)->size();

	if ( isize==0 ) { 
	  empty.insert(i); 
	  continue;
	}

	//	std::cout << i << " : ";

	keep[iorder].push_back( i );

	std::vector<int> vec; 
	
	found = true;

	for ( int j=i+1 ; j<ig->SubProcesses() ; j++ ) {

	  if ( duplicates.find(j)!=duplicates.end() ) continue;
	  if ( empty.find(j)!=empty.end() ) continue;

	  int jsize = ig->weightgrid(j)->size();
	  
	  if ( jsize==0 ) { 
	    empty.insert(j); 
	    continue;
	  }
	  
	  //	  std::cout << "\t" << j << ":" << jsize;
	   
	  if ( (*ig->weightgrid(i)) == (*ig->weightgrid(j)) ) { 
	    //  std::cout << "!"; 
	    vec.push_back(j);
	    duplicates.insert(j);
	  }

	}
	//	std::cout << std::endl;

	same.insert( std::map< int, std::vector<int> >::value_type( i, vec ) );

      }


      if ( same.empty() ) continue;



      std::map< int, std::vector<int> >::iterator itr  = same.begin();
      std::map< int, std::vector<int> >::iterator iend = same.end();

      for ( int ik=0 ; ik<m_order ; ik++ ) std::cout << m_genpdf[iorder]->name() << std::endl;

      lumi_pdf*  _pdf = 0;
      if ( m_genpdf[iorder]->name().find(".config")!=std::string::npos ){ 
	_pdf = dynamic_cast<lumi_pdf*>(m_genpdf[iorder]);
      }
      else { 
	std::cerr << __FUNCTION__ << "\tpdf not found:" << m_genpdf[iorder]->name() << std::endl;
	return;
      }

      std::vector<combination> combinations;

      int i=0;
      while ( itr!=iend ) { 

	std::vector<int>& v = itr->second;
	//	std::cout << "\t" << i++ << "\tproc: " << itr->first << ":" << sizeitr->second << "\t" << itr->second << " ( " << (*_pdf)[itr->first] << " )" << std::endl;
	//	std::cout << "\t" << i << "\tproc: " << itr->first << ":" << sizeitr->second << "\t" << (*_pdf)[itr->first] << std::endl;
	//	for ( unsigned iv=0 ; iv<v.size() ; iv++ ) std::cout << "\t\t\t" << v[iv] << "\t" << (*_pdf)[v[iv]] << std::endl;

	std::vector<int> c(2);
	c[0] = i;
	c[1] = 0;

	const combination& comb = (*_pdf)[itr->first]; 

	for ( unsigned ic=0 ; ic<comb.size() ; ic++ ) { 
	  c[1]++;
	  c.push_back( comb[ic].first );
	  c.push_back( comb[ic].second );
	}

	for ( unsigned iv=0 ; iv<v.size() ; iv++ ) { 

	  const combination& comb = (*_pdf)[v[iv]]; 
	  
	  for ( unsigned ic=0 ; ic<comb.size() ; ic++ ) { 
	    c[1]++;
	    c.push_back( comb[ic].first );
	    c.push_back( comb[ic].second );
	  }
	}

	combinations.push_back( combination( c ) );

	i++;
	itr++;
      }

      //      std::set< int>::iterator eitr  = empty.begin();
      //      std::set< int>::iterator eend  = empty.end();

      //      std::cout << "\tempty: " << empty.size() << " sub-processes";
      //      while ( eitr!=eend ) std::cout << " " << (*eitr++);
      //      std::cout << std::endl; 
     
      lumi_pdf newpdf( "newpdf.config", combinations, 0 );
      
      pdf_combinations.push_back( newpdf.serialise() );
      
      //      std::cout << newpdf << std::endl;
 
      if ( found ) break;

    }

    bool common = true;
    for ( unsigned ipdf=1 ; ipdf<pdf_combinations.size() ; ipdf++ ) { 
    
      if ( pdf_combinations[ipdf].size()>0 ) {
	if ( pdf_combinations[ipdf]!=pdf_combinations[ipdf-1] ) { 
	  /// std::cout << "pdfs " << ipdf << " and " << ipdf-1 << " don't match" << std::endl; 
	  //		    << lumi_pdf("duff.config", pdf_combinations[ipdf]) 
	  common = false;
	}  
      } 
    }      
    
    /// create the actual lumi_pdfs from these combinations

    if ( common && pdf_combinations.size()>0 ) { 
      pdf_combinations[0].push_back(ckmcharge);

      lumi_pdf lpdf( name+label[iorder]+".config",   pdf_combinations[0]);

      std::cout << lpdf.name() << std::endl;

      lpdf.write( lpdf.name() );

      if ( genpdfname.size() ) genpdfname += ":";
      genpdfname += name+label[iorder]+".config";
    }  
  }

  std::cout << "appl::grid::shrink() genpdfname " << genpdfname << std::endl;

  //  if ( addpdf(m_genpdfname) )  findgenpdf( m_genpdfname );

  /// horray!! here we have the optimised pdf combinations written out, so now we 
  /// to delete the duplicated (or empty) grids ...

  /// loop over the igrids, telling each grid which processes to keep
  
  for( int iobs=0 ; iobs<Nobs_internal() ; iobs++ ) { 
    for( int iorder=0 ; iorder<2 ; iorder++ ) {
      //      std::cout << "appl::grid::shrink()  obs " << iobs << "\torder " << iorder << std::endl;       
      m_grids[iorder][iobs]->shrink( keep[iorder] );
    }
  }

  /// ... and need to replace the genpdf with these new ones
  
  m_genpdfname = genpdfname;
  addpdf( m_genpdfname );
  findgenpdf( genpdfname );

  std::cout << "appl::grid::shrink()  new " 
	    << "\t" << m_genpdf[0]->name() << ":" << m_genpdf[0]->size() 
	    << "\t" << m_genpdf[1]->name() << ":" << m_genpdf[1]->size() 
	    <<  std::endl;

}


void appl::grid::combineReference(bool force) { 

  if ( m_combine.empty() ) return;

  if ( force ) { 
    if ( m_obs_bins_combined ) { 
      if ( m_obs_bins_combined!=m_obs_bins ) delete m_obs_bins_combined;
      m_obs_bins_combined = 0;
    }
  }

  if ( m_obs_bins_combined && m_obs_bins_combined!=m_obs_bins) return; 

  std::vector<double> hvec(  m_obs_bins->GetNbinsX(), 0 );
  std::vector<double> hvece( m_obs_bins->GetNbinsX(), 0 );


  for ( int i=m_obs_bins->GetNbinsX() ; i-- ; )  { 
    hvec[i]  = m_obs_bins->GetBinContent( i+1 );
    hvece[i] = m_obs_bins->GetBinError( i+1 );
  }

  combineBins( hvec );
  combineBins( hvece, 2 );
  
  std::vector<double> limits(m_combine.size()+1);
  
  int i=0;
  limits[0] = m_obs_bins->GetBinLowEdge(i+1);
  for ( unsigned ib=0 ; ib<m_combine.size() ; ib++) { 
    i += m_combine[ib]; 
    limits[ib+1] = m_obs_bins->GetBinLowEdge(i+1);
  }
  
  /// need to make this a class variable set to 0 so we don't 
  /// recalculate this every time, only if m_combine changes 
  TH1D* h = new TH1D("reference", "xsec", m_combine.size(), &limits[0] );
  h->SetDirectory(0);

  for ( unsigned i=0 ; i<hvec.size() ; i++ ) { 
    h->SetBinContent( i+1, hvec[i] );
    h->SetBinError( i+1, hvece[i] );
  }

  m_obs_bins_combined = h;
  
}



void appl::grid::combineBins(std::vector<double>& hvec, int power ) const { 
  
  /// now combine bins if required ...
  
  if ( m_combine.size()!=0 ) { 
    /// need to go through, scaling by bin width, adding and then dividing by bin width again
    /// in the TH1D* version, will need to recalculate the bin limits to create the new histogram
    
    std::vector<double> _hvec(m_combine.size(),0);
    
    unsigned nbins = 0;

    unsigned i=0;

    for ( unsigned ic=0 ; ic<m_combine.size() ; ic++ ) { 

      nbins += m_combine[ic];

      if ( nbins>hvec.size() ) throw grid::exception( std::cerr << "too many bins specified for rebinning"  ); 

      double sigma = 0;
      double width = 0;

      for ( int ib=0 ; ib<m_combine[ic] && i<hvec.size() ; ib++, i++ ) { 
	double deltaobs = m_obs_bins->GetBinLowEdge(i+2)-m_obs_bins->GetBinLowEdge(i+1);
	if ( power==1 ) sigma +=  hvec[i]*deltaobs;
	if ( power==2 ) sigma += (hvec[i]*deltaobs*hvec[i]*deltaobs);
	width += deltaobs;
      }

      if ( power==1 ) _hvec[ic] = sigma/width;
      if ( power==2 ) _hvec[ic] = std::sqrt(sigma)/width;
    }

    hvec = _hvec;
  }
}


std::ostream& operator<<(std::ostream& s, const appl::grid& g) {
  
  s << "==================================================" << std::endl;
  
  //  s << "appl::grid version " << g.version() << "\t(" << g.subProcesses(0) << " initial states, " << g.Nobs_internal() << " observable bins)" << std::endl;

  std::string basis[5] = {  "-LO, ",  "-NLO, ",  "-NNLO, ", "-Xtra0", "-Xtra1" };  
  std::string order[appl::MAXGRIDS];
  for ( int i=0 ; i<appl::MAXGRIDS ; i++ ) { 
    if ( i<5) order[i] = basis[i];
    else      order[i] = "-Unknown";
  }

  s << "appl::grid version " << g.version() << "\t( "; 
  for ( int i=0 ; i<g.nloops()+1 ; i++ ) s << g.subProcesses(i) << order[i];
  s << "initial states, " << g.Nobs_internal() << " observable bins )" << std::endl;
  if ( g.isOptimised() ) s << "Optimised grid" << std::endl;
  if ( g.isSymmetric() ) s << "Symmetrised in x1, x2" << std::endl;
  else                   s << "Unsymmetrised in x1, x2" << std::endl;
  if ( g.getNormalised() ) s << "Normalised " << std::endl;
  s << "leading order of processes  "  << g.leadingOrder() << std::endl;
  s << "number of loops for grid    " << g.nloops() << std::endl;   
  s << "x->y coordinate transform:  "  << g.getTransform() << std::endl;
  s << "genpdf in use: " << g.getGenpdf() << std::endl;
  s << "--------------------------------------------------" << std::endl;
  s << "Observable binning: [ " << g.Nobs_internal() 
    << " bins : " << g.obsmin() << ",  " << g.obsmax() << " ]" << std::endl;

  //  for( int iorder=0 ; iorder<1 ; iorder++ ) {
  for( int iobs=0 ; iobs<g.Nobs_internal() ; iobs++ ) {
    s << iobs << "\t" 
      << std::setprecision(5) << std::setw(5) << g.getReference()->GetBinLowEdge(iobs+1) << "\t- " 
      << std::setprecision(5) << std::setw(5) << g.getReference()->GetBinLowEdge(iobs+2) << "\t"; 
    s << "   " << *(g.weightgrid(0,iobs)) << std::endl;
  }
  //  }

  s << std::endl;
  
  return s;
}
