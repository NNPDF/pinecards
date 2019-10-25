// emacs: this is -*- c++ -*-

//  appl_grid.h       

//  grid class header - all the functions needed to create and 
//  fill the grid from an NLO calculation program
//  
//  Copyright (C) 2007 Mark Sutton (sutt@hep.ucl.ac.uk)    

// $Id: appl_grid.h, v1.00 2007/10/16 17:01:39 sutt

// Fixme: this needs to be tidied up. eg there are too many different, 
//        and too many version of, accessors for x/y, Q2/tau etc there 
//        should be only  one set, for x and Q2 *or* y and tau, but 
//        not both. In fact they should be for x and Q2, since y and tau 
//        should be purely an internal grid issue of no concern for the 
//        user.

#ifndef __APPL_PHOTON
#define __APPL_PHOTON
#endif

#ifndef __APPL_GRID_H
#define __APPL_GRID_H

#include <cassert>
#include <vector>
#include <iostream>
#include <cmath>
#include <string>
#include <exception>

#include "TH1D.h"

namespace appl { 


/// forward declarations - full definitions included 
/// from appl_grid.cxx 
class igrid;
class appl_pdf;


const int MAXGRIDS = 64;

class order_id
{
public:
    order_id(int alphs, int alpha, int lmuf2, int lmur2)
        : alphs_{alphs}
        , alpha_{alpha}
        , lmuf2_{lmuf2}
        , lmur2_{lmur2}
    {
    }

    int alphs() const
    {
        return alphs_;
    }

    int alpha() const
    {
        return alpha_;
    }

    int lmuf2() const
    {
        return lmuf2_;
    }

    int lmur2() const
    {
        return lmur2_;
    }

private:
    int alphs_;
    int alpha_;
    int lmuf2_;
    int lmur2_;
};

inline bool operator==(order_id const& a, order_id const& b)
{
    return (a.alphs() == b.alphs()) && (a.alpha() == b.alpha()) && (a.lmuf2() == b.lmuf2())
        && (a.lmur2() == b.lmur2());
}

/// externally visible grid class
class grid {

public:

  // grid error exception
  class exception : public std::exception { 
  public:
    exception(const std::string& s) { std::cerr << what() << " " << s << std::endl; }
    exception(std::ostream&)      { std::cerr << std::endl; }
    virtual const char* what() const throw() { return "appl::grid::exception"; }
  };

  grid(std::vector<grid>&& grids);

  grid( int Nobs, const double* obsbins,
	int NQ2=50,  double Q2min=10000.0, double Q2max=25000000.0, int Q2order=5,
        int Nx=50,   double xmin=1e-5,     double xmax=0.9,         int xorder=5,
	std::string genpdf="mcfm_pdf",
       std::vector<order_id> const& order_ids = std::vector<order_id>(),
	std::string transform="f2" );

  // read from a file
  grid(const std::string& filename="./grid.root", const std::string& dirname="grid");

  virtual ~grid();
  
  // update grid with one set of event weights
  void fill(const double x1, const double x2, const double Q2, 
	    const double obs, 
	    const double* weight, const int iorder);
  
  
  void fill_phasespace(const double x1, const double x2, const double Q2, 
		       const double obs, 
		       const double* weight, const int iorder);
  
  
  void fill_grid(const double x1, const double x2, const double Q2, 
		 const double obs, 
		 const double* weight, const int iorder)  {
    if (isOptimised())   fill(x1, x2, Q2, obs, weight, iorder);
    else                 fill_phasespace(x1, x2, Q2, obs, weight, iorder);
  }

  // trim/untrim the grid to reduce memory footprint
  void trim();
  void untrim();

  // perform the convolution to a specified number of loops
  // nloops=-1 gives the nlo part only
  std::vector<double>  vconvolute(void   (*pdf)(const double& , const double&, double* ), 
				  double (*alphas)(const double& ), 
				  int     nloops, 
				  double  rscale_factor=1,
				  double  fscale_factor=1,
				  double  Escale=1 );

  std::vector<double>  vconvolute(void   (*pdf1)(const double& , const double&, double* ), 
				  void   (*pdf2)(const double& , const double&, double* ), 
				  double (*alphas)(const double& ), 
				  int     nloops, 
				  double  rscale_factor=1,
				  double  fscale_factor=1,
				  double  Escale=1 );

  std::vector<std::vector<double>>  vconvolute_orders(void   (*pdf1)(const double& , const double&, double* ),
				  void   (*pdf2)(const double& , const double&, double* ),
				  double (*alphas)(const double& ),
				  double  rscale_factor=1,
				  double  fscale_factor=1,
				  double  Escale=1 );

  // perform the convolution to the max number of loops in grid
  std::vector<double> vconvolute(void   (*pdf)(const double& , const double&, double* ), 
				 double (*alphas)(const double& ) )   { 
    return vconvolute( pdf, alphas, nloops() );
  } 

  std::vector<order_id> const& order_ids() const;

  // optimise the bin limits
  void optimise(bool force=false);

  bool getNormalised() const      { return m_normalised; } 

  bool reweight(bool t=false); 

  // access to internal grids if need be
  const igrid* weightgrid(int iorder, int iobs) const { return m_grids[iorder][iobs]; }
  
  // save grid to specified file
  void Write(const std::string& filename, const std::string& dirname="grid", const std::string& pdfname="" );

  // accessors for the observable after possible bin combination
  int    Nobs()               const { return m_obs_bins_combined->GetNbinsX(); }
  double obslow(int iobs)     const { return m_obs_bins_combined->GetBinLowEdge(iobs+1); }
  double obsmax()             const { return obslow(Nobs()); } 
  double deltaobs(int iobs)   const { return m_obs_bins_combined->GetBinWidth(iobs+1); }

  const TH1D* getReference() const { return m_obs_bins_combined; } 
  TH1D*       getReference()       { return m_obs_bins_combined; } 

  // accessors for the observable befor any bin combination
  int    Nobs_internal()               const { return m_obs_bins->GetNbinsX(); }
  double obslow_internal(int iobs)     const { return m_obs_bins->GetBinLowEdge(iobs+1); }
  double deltaobs_internal(int iobs)   const { return m_obs_bins->GetBinWidth(iobs+1); }
  double obsmin_internal()             const { return obslow_internal(0); } 
  double obsmax_internal()             const { return obslow_internal(Nobs_internal()); } 

  // number of subprocesses 
  int subProcesses(int i) const;

  // general status accessors
  double& run() { return m_run; }
 
  // accessors for the status information
  bool isOptimised() const { return m_optimised; }
  int  nloops() const;

  std::string getGenpdf()    const { return m_genpdfname; }

  // find the number of words used for storage
  int size() const; 

  grid& operator*=(const double& d); 
  grid& operator+=(const grid& g);

  void setDocumentation(const std::string& s);
  void addDocumentation(const std::string& s);

  std::string  getDocumentation() const { return m_documentation; }
  std::string& getDocumentation()       { return m_documentation; }

  /// set the ckm matrix values if need be
  /// takes a 3x3 matrix with the format { { Vud, Vus, Vub }, { Vcd, Vcs, Vcb }, { Vtd, Vts, Vtb } }  
  void setckm( const std::vector<std::vector<double> >& ckm );

  /// set the squared ckm matrix values if need be
  /// the squared terms for eihter W+ or W- production - you probably should use setckm()
  void setckm2( const std::vector<std::vector<double> >& ckm2 );

  /// reduce number of subprocesses if possible
  void shrink(const std::string& name, int ckmcharge=0);

  /// set combine the  be combined after the convolution
  void combineReference(bool force=false);

  void combineBins(std::vector<double>& v, int power=1 ) const;

  const appl_pdf* genpdf(int i) const { return m_genpdf[i]; }

protected:

  /// get the required pdf combinations from those registered   
  void findgenpdf( std::string s );

  /// add a generic pdf to the data base of registered pdfs
  void addpdf( const std::string& s, const std::vector<int>& combinations=std::vector<int>() );

  appl_pdf* genpdf(int i) { return m_genpdf[i]; }
  
public: 

  int subproc() const { return m_subproc; }

protected:

  // histograms for saving the observable
  TH1D*  m_obs_bins;
  TH1D*  m_obs_bins_combined;

  // the actual weight grids themselves
  std::vector<igrid**> m_grids;

  // total cross section qand uncertainty
  double m_total;
  double m_totalerror;

  // state variables
  double   m_run;
  bool     m_optimised;
  bool     m_trimmed;

  bool   m_normalised;

  bool   m_symmetrise; 
 
  // transform and pdf combination tags
  std::string m_transform; 
  std::string m_genpdfname; 

  // pdf combination class
  std::vector<appl_pdf*> m_genpdf;

  static const std::string m_version;

  double m_cmsScale;

  double m_dynamicScale;

  std::string m_documentation;
  
  std::vector<double>                m_ckmsum;
  std::vector<std::vector<double> >  m_ckm2;
  std::vector<std::vector<double> >  m_ckm;

  bool            m_read;

  std::vector<int> m_combine;

  int  m_subproc;
  int  m_bin;

  std::vector<order_id> m_order_ids;
};

};

#endif // __APPL_GRID_H 
