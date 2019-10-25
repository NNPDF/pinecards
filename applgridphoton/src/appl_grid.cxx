
//  appl_grid.cxx        

//   grid class - all the functions needed to create and 
//   fill the grid from an NLO calculation program. 
//  
//  Copyright (C) 2007 Mark Sutton (sutt@hep.ucl.ac.uk)    

// $Id: appl_grid.cxx, v1.00 2007/10/16 17:01:39 sutt $

#include <cstdlib>
#include <stdio.h>
#include <sys/stat.h>

#include <algorithm>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <numeric>

#include "appl_grid/appl_pdf.h"
#include "appl_timer.h"
#include "Directory.h"
#include "appl_grid/appl_grid.h"
#include "appl_grid/lumi_pdf.h"

#include "appl_igrid.h"
#include "Cache.h"
#include "SparseMatrix3d.h"


#include "TFileString.h"
#include "TFileVector.h"

#include "TFile.h"
#include "TObjString.h"
#include "TVectorT.h"


#include "amconfig.h"

namespace
{

std::string chomptoken(std::string& s1, const std::string& s2)
{
  std::string s3 = "";
  std::string::size_type pos = s1.find(s2);
  if ( pos != std::string::npos ) {
    s3 = s1.substr(0, pos);
    s1.erase(0, pos+1);
  }
  else {
    s3 = s1.substr(0, s1.size());
    s1.erase(0, s1.size()+1);
  }
  return s3;
}

std::vector<std::string> parse(std::string s, const std::string& key) {
  std::vector<std::string> clauses;
  while ( s.size() ) clauses.push_back( chomptoken(s, key) );
  return clauses;
}

}

/// this is a compatability flag for persistent versions 
/// of the grid
/// NB: ONLY change the major version if the persistent 
///     class changes in a non-backwards compatible way.

// const std::string appl::grid::m_version = "version-3.3";
const std::string appl::grid::m_version = PACKAGE_VERSION;

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

/// helper function
static bool contains(const std::string& s, const std::string& reg ) { 
  return s.find(reg)!=std::string::npos;
}

appl::grid::grid(int Nobs, const double* obsbins,
		 int NQ2,  double Q2min, double Q2max, int Q2order,
		 int Nx,   double xmin, double xmax,   int xorder,
		 std::string genpdfname,
         std::vector<order_id> const& order_ids,
		 std::string transform ) :
  m_grids(appl::MAXGRIDS),
  m_run(0), m_optimised(false), m_trimmed(false),  m_normalised(false), m_symmetrise(false),
  m_transform(transform), m_genpdfname(genpdfname),
  m_genpdf(appl::MAXGRIDS),
  m_documentation(""),
  m_read(false),
  m_subproc(-1),
  m_order_ids(order_ids)
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

  for ( int iorder=0 ; iorder<m_order_ids.size() ; iorder++ ) m_grids[iorder] = 0;

  for ( int iorder=0 ; iorder<m_order_ids.size() ; iorder++ ) {
    m_grids[iorder] = new igrid*[Nobs];
    for ( int iobs=0 ; iobs<Nobs ; iobs++ ) {
      m_grids[iorder][iobs] = new igrid(NQ2, Q2min, Q2max, Q2order,
					Nx, xmin, xmax, xorder,
					m_transform, m_genpdf[iorder]->Nproc());
      m_grids[iorder][iobs]->setparent( this );
    }
  }
  //  std::cout << "appl::grid::construct() return" << std::endl;
}

appl::grid::grid(const std::string& filename, const std::string& dirname)  :
  m_order_ids(),
  m_grids(appl::MAXGRIDS),
  m_optimised(false),  m_trimmed(false), 
  m_normalised(false),
  m_symmetrise(false), m_transform(""), 
  m_genpdf(appl::MAXGRIDS),
  m_documentation(""),
  m_read(false),
  m_subproc(-1)
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

  std::unique_ptr<TFileString> _tagsp{
    dynamic_cast <TFileString*> (gridfilep->Get((dirname+"/Tags").c_str()))};

  if ( !_tagsp ) {
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
 
  if ( setup->GetNoElements()>7 ) m_normalised = ( (*setup)(7)!=0 ? true : false );
  else                            m_normalised = true;

  int n_userdata = 0;
  if ( setup->GetNoElements()>11 ) n_userdata = int((*setup)(11)+0.5);

  m_order_ids.reserve((*setup)(12));
  for (std::size_t i = 0; i != m_order_ids.capacity(); ++i)
  {
    m_order_ids.emplace_back(
      static_cast <int> ((*setup)(12 + 1 + i * 4)),
      static_cast <int> ((*setup)(12 + 2 + i * 4)),
      static_cast <int> ((*setup)(12 + 3 + i * 4)),
      static_cast <int> ((*setup)(12 + 4 + i * 4))
    );
  }


  //  std::vector<double> _ckmsum;
  std::vector<std::vector<double> > _ckm2;
  std::vector<std::vector<double> > _ckm;



  /// check whether we need to read in the ckm matrices

  if ( setup->GetNoElements()>9 && (*setup)(9)!=0 ) {

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

      std::unique_ptr<TVectorT<double>> _combinations{
        dynamic_cast <TVectorT<double>*> (gridfilep->Get( label.c_str() ))};

      label += "N"; /// add an N for each order, N-LO, NN-LO etc

      if ( !_combinations ) throw exception(std::cerr << "grid::grid() cannot read pdf combination " << namevec[i] << std::endl );

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

  for( int iorder=0 ; iorder<m_order_ids.size() ; iorder++ ) {
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

std::vector<appl::order_id> const& appl::grid::order_ids() const
{
    return m_order_ids;
}

int appl::grid::nloops() const
{
    int max = 0;
    for (auto const& order : m_order_ids)
    {
        max = std::max(max, order.lmuf2() + order.lmur2());
    }

    return max;
}

  // number of subprocesses 
int appl::grid::subProcesses(int i) const { 
  if ( i<0 || i>=m_order_ids.size() ) throw exception( std::cerr << "grid::subProcess(int i) " << i << " out or range [0-" << m_order_ids.size()-1 << "]" << std::endl );
  return m_grids[i][0]->SubProcesses();     
}  

appl::grid::~grid() {
  for( int iorder=0 ; iorder<m_order_ids.size() ; iorder++ ) {
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

appl::grid& appl::grid::operator*=(const double& d) { 
  for( int iorder=0 ; iorder<m_order_ids.size() ; iorder++ ) {
    for( int iobs=0 ; iobs<Nobs_internal() ; iobs++ ) (*m_grids[iorder][iobs])*=d; 
  }
  getReference()->Scale( d );
  combineReference(true);
  return *this;
}

appl::grid& appl::grid::operator+=(const appl::grid& g) {
  m_run      += g.m_run;
  m_optimised = g.m_optimised;
  m_trimmed   = g.m_trimmed;
  if ( Nobs_internal()!=g.Nobs_internal() )   throw exception("grid::operator+ Nobs bin mismatch");

    // the grids in `this` and in `g` might be sorted differently, or they might even contain
    // different grids

    for (std::size_t i = 0; i != g.m_order_ids.size(); ++i)
    {
        auto const grid_it = std::find(m_order_ids.begin(), m_order_ids.end(), g.m_order_ids.at(i));

        if (grid_it != m_order_ids.end())
        {
            std::size_t const index = std::distance(m_order_ids.begin(), grid_it);

            // TODO: we should probably check whether the luminosity functions are the same

            for (int iobs = 0; iobs < Nobs_internal(); ++iobs)
            {
                *m_grids[index][iobs] += *g.m_grids[i][iobs];
            }
        }
        else
        {
            m_grids[m_order_ids.size()] = new igrid*[Nobs_internal()];

            for (int iobs = 0; iobs < Nobs_internal(); ++iobs)
            {
                m_grids[m_order_ids.size()][iobs] = new igrid(*g.m_grids[i][iobs]);
                m_grids[m_order_ids.size()][iobs]->setparent(this);

                *m_grids[m_order_ids.size()][iobs] = *g.m_grids[i][iobs];
            }

            // TODO: is simply copying the pointer correct?
            m_genpdf[m_order_ids.size()] = g.m_genpdf.at(i);

            m_order_ids.push_back(g.m_order_ids.at(i));
        }
    }

  /// grrr use root TH1::Add() even though I don't like it. 
  getReference()->Add( g.getReference() );
  combineReference(true);

  return *this;
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

void appl::grid::trim() {
  m_trimmed = true;
  for( int iorder=0 ; iorder<m_order_ids.size() ; iorder++ ) {
    for( int iobs=0 ; iobs<Nobs_internal() ; iobs++ ) m_grids[iorder][iobs]->trim(); 
  }
}

void appl::grid::untrim() {
  m_trimmed = false;
  for( int iorder=0 ; iorder<m_order_ids.size() ; iorder++ ) {
    for( int iobs=0 ; iobs<Nobs_internal() ; iobs++ ) m_grids[iorder][iobs]->untrim(); 
  }
}
 /// get the required pdf combinations from those registered   
void appl::grid::findgenpdf( std::string s ) { 
    std::vector<std::string> names = parse( s, ":" );
    if ( names.size()==unsigned(m_order_ids.size()) ) for ( int i=0 ; i<m_order_ids.size() ; i++ ) m_genpdf[i] = appl_pdf::getpdf( names[i] );
    else  if ( names.size()==1 )           for ( int i=0 ; i<m_order_ids.size() ; i++ ) m_genpdf[i] = appl_pdf::getpdf( names[0] );
    else  { 
      throw exception( std::cerr << "requested " << m_order_ids.size() << " pdf combination but given " << names.size() << std::endl );
    }
}


void appl::grid::addpdf( const std::string& s, const std::vector<int>& combinations ) {
  //  std::cout << "addpdf() in " << std::endl;

    /// parse names, if they contain .dat, then create the new generic pdfs
    /// they will be added to the pdf std::map automatically 
    std::vector<std::string> names = parse( s, ":" );

    unsigned imax = unsigned(m_order_ids.size());

    /// check to see whether we have a different pdf for each order
    if ( names.size()!=imax ) { 
      if ( names.size()==1 ) imax = 1;
      else { 
	throw exception( std::cerr << "requested " << m_order_ids.size() << " pdf combination but given " << names.size() << std::endl );
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
      assert( false );
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
  for ( int i=0 ; i<m_order_ids.size() ; i++ ) m_genpdf[i]->setckm2(ckm2);
}


void appl::grid::setckm( const std::vector<std::vector<double> >& ckm ) { 
  for ( int i=0 ; i<m_order_ids.size() ; i++ ) m_genpdf[i]->setckm(ckm);
}

// set the rewight flag of the internal grids
bool appl::grid::reweight(bool t) { 
  for( int iorder=0 ; iorder<m_order_ids.size() ; iorder++ ) {
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
  TVectorT<double>* setup=new TVectorT<double>(64);
  (*setup)(0) = m_run;
  (*setup)(1) = ( m_optimised  ? 1 : 0 );
  (*setup)(2) = ( m_symmetrise ? 1 : 0 );
  (*setup)(7) = ( m_normalised ? 1 : 0 );

  if ( m_genpdf[0]->getckmsum().size()==0 ) (*setup)(9) = 0;
  else                                      (*setup)(9) = 1;

  (*setup)(12) = m_order_ids.size();
  for (std::size_t i = 0; i != m_order_ids.size(); ++i)
  {
    (*setup)(12 + 1 + i * 4) = m_order_ids.at(i).alphs();
    (*setup)(12 + 2 + i * 4) = m_order_ids.at(i).alpha();
    (*setup)(12 + 3 + i * 4) = m_order_ids.at(i).lmuf2();
    (*setup)(12 + 4 + i * 4) = m_order_ids.at(i).lmur2();
  }

  setup->Write("State");
  
  if ( (*setup)(9) == 1 ) {
    
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
    
    for ( unsigned i=0 ; i<namevec.size() && i<unsigned(m_order_ids.size()) ; i++ ) {

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
  for( int iorder=0 ; iorder<m_order_ids.size() ; iorder++ ) {
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

  /// now write out vector of bins to be combined if this has been set
  if ( m_combine.size()>0 ) { 
    TVectorT<double>* _combine = new TVectorT<double>(m_combine.size()); 
    for ( unsigned i=m_combine.size() ; i-- ; ) (*_combine)(i) = m_combine[i]+0.5; /// NB: add 0.5 to prevent root double -> int rounding errors
    _combine->Write( "CombineBins" );
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
  if ( fscale_factor!=1 ) {

    if ( pdf2==0 || pdf1==pdf2 ) { 

      if ( hoppet == 0 ) { 
	double Qmax = 15000;
	hoppet = new hoppet_init( Qmax );
      } 

      bool newpdf = hoppet->compareCache( pdf1 );
      
      if ( newpdf ) hoppet->fillCache( pdf1 );

    }

  }
#endif

    //    std::cout << "amc@NLO convolution" << std::endl;
    
    std::string label = "nlo";

    auto const& leading_order = *std::min_element(m_order_ids.begin(), m_order_ids.end(),
        [](order_id const& a, order_id const& b) {
            return (a.alphs() + a.alpha()) < (b.alphs() + b.alpha());
        });
    std::size_t const lo_power = leading_order.alphs() + leading_order.alpha();

    for ( int iobs=0 ; iobs<Nobs_internal() ; iobs++ ) {  

      double dsigma = 0; 

      double log_xif2 = std::log(fscale_factor * fscale_factor);
      double log_xir2 = std::log(rscale_factor * rscale_factor);

      for (int i = 0; i != m_order_ids.size(); ++i)
      {
        double factor = 1.0;

        std::size_t const power = m_order_ids.at(i).alphs() + m_order_ids.at(i).alpha() - lo_power;

        if (power > nloops)
        {
            continue;
        }

        if (m_order_ids.at(i).lmuf2() != 0)
        {
            factor *= log_xif2;
        }

        if (m_order_ids.at(i).lmur2() != 0)
        {
            factor *= log_xir2;
        }

        if (factor != 0.0)
        {
            dsigma += m_grids[i][iobs]->amc_convolute( _pdf1, _pdf2, m_genpdf[i], alphas, m_order_ids.at(i).alphs(), 0, rscale_factor, fscale_factor, Escale);
        }
      }

      double deltaobs = m_obs_bins->GetBinLowEdge(iobs+2)-m_obs_bins->GetBinLowEdge(iobs+1);      
      hvec.push_back( invNruns*Escale2*dsigma/deltaobs );
    }

  /// now combine bins if required ...

  /// combine bins if required
  if ( m_combine.size()!=0 ) { 
    combineBins( hvec );
  }

  //  double _ctime = appl_timer_stop(_ctimer);
  //  std::cout << "grid::convolute() " << label << " convolution time=" << _ctime << " ms" << std::endl;
  
  cache1.stats();
  if ( cache2.ncalls() ) cache2.stats();
  
  return hvec;
}

std::vector<std::vector<double>> appl::grid::vconvolute_orders(void (*pdf1)(const double& , const double&, double* ),
					   void (*pdf2)(const double& , const double&, double* ),
					   double (*alphas)(const double& ),
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

  std::vector<std::vector<double>> hvec(m_order_ids.size());

  double invNruns = 1;
  if ( (!m_normalised) && run() ) invNruns /= double(run());

  //  TH1D* h = new TH1D(*m_obs_bins);
  //  h->SetName("xsec");

#ifdef HAVE_HOPPET
  // check if we need to use the splitting function, and if so see if we
  // need to initialise it again, and do so if required
  if ( fscale_factor!=1 ) {

    if ( pdf2==0 || pdf1==pdf2 ) {

      if ( hoppet == 0 ) {
	double Qmax = 15000;
	assert( false );
	hoppet = new hoppet_init( Qmax );
      }

      bool newpdf = hoppet->compareCache( pdf1 );

      if ( newpdf ) hoppet->fillCache( pdf1 );

    }

  }
#endif

    //    std::cout << "amc@NLO convolution" << std::endl;

    for (int i = 0; i != m_order_ids.size(); ++i)
    {
      double log_xif2 = std::log(fscale_factor * fscale_factor);
      double log_xir2 = std::log(rscale_factor * rscale_factor);

      for ( int iobs=0 ; iobs<Nobs_internal() ; iobs++ ) {

        double dsigma = 0;
        double factor = 1.0;

        if (m_order_ids.at(i).lmuf2() != 0)
        {
            factor *= log_xif2;
        }

        if (m_order_ids.at(i).lmur2() != 0)
        {
            factor *= log_xir2;
        }

        if (factor != 0.0)
        {
            dsigma = m_grids[i][iobs]->amc_convolute( _pdf1, _pdf2, m_genpdf[i], alphas, m_order_ids.at(i).alphs(), 0, rscale_factor, fscale_factor, Escale);
        }

        double deltaobs = m_obs_bins->GetBinLowEdge(iobs+2)-m_obs_bins->GetBinLowEdge(iobs+1);
        hvec.at(i).push_back( invNruns*Escale2*dsigma/deltaobs );
      }

    }

  return hvec;
}

void appl::grid::optimise(bool force) {
  if ( !force && m_optimised ) return;
  m_optimised = true;
  m_read = false;
  for ( int iorder=0 ; iorder<m_order_ids.size() ; iorder++ ) {
    for ( int iobs=0 ; iobs<Nobs_internal() ; iobs++ )  { 
      std::cout << "grid::optimise() bin " << iobs << "\t";
      m_grids[iorder][iobs]->optimise();
    }
  }
  m_obs_bins->Reset();
}

/// methods to handle the documentation
void appl::grid::setDocumentation(const std::string& s) { m_documentation = s; }
void appl::grid::addDocumentation(const std::string& s) {   
  if ( m_documentation.size() ) m_documentation += s;
  else                          setDocumentation(s);    
}

// find the number of words used for storage
int appl::grid::size() const { 
    int _size = 0;
    for( int iorder=0 ; iorder<m_order_ids.size() ; iorder++ ) {
      for( int iobs=0 ; iobs<Nobs_internal() ; iobs++ ) _size += m_grids[iorder][iobs]->size();
    }
    return _size;
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
  for( int iorder=0 ; iorder<m_order_ids.size() ; iorder++ ) {

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

      for ( int ik=0 ; ik<m_order_ids.size() ; ik++ ) std::cout << m_genpdf[iorder]->name() << std::endl;

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
    for( int iorder=0 ; iorder<m_order_ids.size() ; iorder++ ) {
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

appl::grid::grid(std::vector<appl::grid>&& grids)
    // TODO: check if all of the following member variables are set correctly
    : m_optimised(false)
    , m_trimmed(false)
    , m_normalised(false)
    , m_symmetrise(false)
    , m_read(false)
    , m_subproc(-1)
    , m_run(0.0)
{
    if (grids.empty())
    {
        throw std::runtime_error("called `merge_bins` with empty `grids` parameter");
    }

    for (auto& grid : grids)
    {
        grid.untrim();
    }

    std::vector<std::size_t> grid_indices(grids.size());
    std::iota(grid_indices.begin(), grid_indices.end(), 0);

    // instead of sorting the grids we sort the *indices* of the grids, according to their lower
    // bin limit
    std::sort(grid_indices.begin(), grid_indices.end(), [&](std::size_t a, std::size_t b) {
        return grids.at(a).obsmin_internal() < grids.at(b).obsmin_internal();
    });

    std::size_t Nobs = 0;
    for (auto const& grid : grids)
    {
        Nobs += grid.Nobs_internal();
    }

    std::size_t const NQ2     = grids.front().m_grids[0][0]->getNQ2();
    double      const Q2min   = grids.front().m_grids[0][0]->getQ2min();
    double      const Q2max   = grids.front().m_grids[0][0]->getQ2max();
    std::size_t const Q2order = grids.front().m_grids[0][0]->tauorder();
    std::size_t const Nx      = grids.front().m_grids[0][0]->getNx1();
    double      const xmin    = grids.front().m_grids[0][0]->getx1min();
    double      const xmax    = grids.front().m_grids[0][0]->getx1max();
    std::size_t const xorder  = grids.front().m_grids[0][0]->yorder();

    auto const& transform = grids.front().m_grids[0][0]->transform();

    std::size_t const orders = grids.front().order_ids().size();

    for (std::size_t i = 1; i != grids.size(); ++i)
    {
        // TODO: relax the condition `!=` with a certain numerical precision?

        // check that the bins are non-overlapping and contiguous
        if (grids.at(grid_indices.at(i)).obsmin_internal() !=
            grids.at(grid_indices.at(i - 1)).obsmax_internal())
        {
            throw std::runtime_error("called `merge_bins` with overlapping or non-contiguous bins");
        }

        // check that all the fixed orders are the same and in the same order
        std::equal(grids.at(i).order_ids().begin(), grids.at(i).order_ids().end(),
            grids.front().order_ids().begin());

        // TODO: the following will not work
//        auto const* lumi_front = appl::grid::getpdf(grids.front().m_genpdf[j]);
//
//        // check that all luminosities are the same and in the same order
//        for (std::size_t j = 0; j != orders; ++j)
//        {
//            auto const* lumi = appl::grid::getpdf(grids.at(i).m_genpdf[j]);
//
//            if (lumi != lumi_front)
//            {
//                throw std::runtime_error("called `merge_bins` with different grids having "
//                    "different luminosities");
//            }
//        }

        // check that (most?) grid parameters are the same
        if (NQ2 != grids.at(i).m_grids[0][0]->getNQ2())
        {
            throw std::runtime_error("called `merge_bins` with different grid parameters");
        }

//        if (Q2min != grids.at(i).m_grids[0][0]->getQ2min())
//        {
//            throw std::runtime_error("called `merge_bins` with different grid parameters");
//        }
//
//        if (Q2max != grids.at(i).m_grids[0][0]->getQ2max())
//        {
//            throw std::runtime_error("called `merge_bins` with different grid parameters");
//        }

        if (Q2order != grids.at(i).m_grids[0][0]->tauorder())
        {
            throw std::runtime_error("called `merge_bins` with different grid parameters");
        }

        if (Nx != grids.at(i).m_grids[0][0]->getNx1())
        {
            throw std::runtime_error("called `merge_bins` with different grid parameters");
        }

//        if (xmin != grids.at(i).m_grids[0][0]->getx1min())
//        {
//            throw std::runtime_error("called `merge_bins` with different grid parameters");
//        }
//
//        if (xmax != grids.at(i).m_grids[0][0]->getx1max())
//        {
//            throw std::runtime_error("called `merge_bins` with different grid parameters");
//        }

        if (Nx != grids.at(i).m_grids[0][0]->getNx2())
        {
            throw std::runtime_error("called `merge_bins` with different grid parameters");
        }

//        if (xmin != grids.at(i).m_grids[0][0]->getx2min())
//        {
//            throw std::runtime_error("called `merge_bins` with different grid parameters");
//        }
//
//        if (xmax != grids.at(i).m_grids[0][0]->getx2max())
//        {
//            throw std::runtime_error("called `merge_bins` with different grid parameters");
//        }

        if (xorder != grids.at(i).m_grids[0][0]->yorder())
        {
            throw std::runtime_error("called `merge_bins` with different grid parameters");
        }

        if (transform != grids.at(i).m_grids[0][0]->transform())
        {
            throw std::runtime_error("called `merge_bins` with different grid parameters");
        }
    }

    std::vector<double> obsbins;
    obsbins.reserve(Nobs + 1);

    for (std::size_t i = 0; i != grids.size(); ++i)
    {
        for (std::size_t j = 0; j != grids.at(i).Nobs_internal(); ++j)
        {
            obsbins.push_back(grids.at(grid_indices.at(i)).obslow(j));
        }
    }

    obsbins.push_back(grids.back().obsmax());

    m_obs_bins = new TH1D("referenceInternal", "Bin-Info for Observable", Nobs, obsbins.data());
    m_obs_bins->SetDirectory(0);
    m_obs_bins->Sumw2();

    // copy the bin info
    std::size_t bin = 1;
    for (std::size_t i = 0; i != grids.size(); ++i)
    {
        auto const& grid = grids.at(grid_indices.at(i));

        for (std::size_t j = 0; j != grid.Nobs_internal(); ++j)
        {
            auto const value = grid.m_obs_bins->GetBinContent(j + 1);
            auto const error = grid.m_obs_bins->GetBinError(j + 1);

            m_obs_bins->SetBinContent(bin, value / grid.m_run);
            m_obs_bins->SetBinError(bin, error / grid.m_run);

            ++bin;
        }
    }

    auto const underflow_value = grids.at(grid_indices.front()).m_obs_bins->GetBinContent(0);
    auto const underflow_error = grids.at(grid_indices.front()).m_obs_bins->GetBinError(0);

    m_obs_bins->SetBinContent(0, underflow_value / grids.at(grid_indices.front()).m_run);
    m_obs_bins->SetBinError(0,   underflow_error / grids.at(grid_indices.front()).m_run);

    auto const overflow_value = grids.at(grid_indices.back()).m_obs_bins->GetBinContent(
        grids.at(grid_indices.back()).m_obs_bins->GetNbinsX());
    auto const overflow_error = grids.at(grid_indices.back()).m_obs_bins->GetBinError(
        grids.at(grid_indices.back()).m_obs_bins->GetNbinsX());

    m_obs_bins->SetBinContent(0, overflow_value / grids.at(grid_indices.front()).m_run);
    m_obs_bins->SetBinError(0,   overflow_error / grids.at(grid_indices.front()).m_run);

    m_obs_bins_combined = m_obs_bins;

    // TODO: here we assume that all luminosities are the same (as for the first grid)
    m_genpdfname = grids.front().m_genpdfname;

    if (contains(m_genpdfname, ".dat") || contains(m_genpdfname, ".config"))
    {
        addpdf(m_genpdfname);
    }

    findgenpdf(m_genpdfname);

    m_grids.resize(orders);
    m_genpdf.resize(orders);
    m_order_ids = grids.front().m_order_ids;
    m_documentation = grids.at(grid_indices.front()).m_documentation;
    m_transform = transform;

    for (std::size_t i = 0; i != orders; ++i)
    {
        std::size_t obs = 0;

        m_grids.at(i) = new igrid*[Nobs];

        for (std::size_t j = 0; j != grids.size(); ++j)
        {
            for (std::size_t k = 0; k != grids.at(grid_indices.at(j)).Nobs_internal(); ++k)
            {
                m_grids.at(i)[obs] = new igrid(*grids.at(grid_indices.at(j)).m_grids.at(i)[k]);
                m_grids.at(i)[obs]->setparent(this);

                *m_grids.at(i)[obs] *= 1.0 / grids.at(grid_indices.at(j)).m_run;

                ++obs;
            }
        }

        m_genpdf.at(i) = grids.front().m_genpdf[i];
    }
}
