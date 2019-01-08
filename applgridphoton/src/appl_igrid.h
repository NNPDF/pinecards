// emacs: this is -*- c++ -*-
//
//   appl_igrid.h        
//
//  grid class header - all the functions needed to create and 
//  fill the grid from an NLO calculation program, based on the 
//  class from D.Clements and T.Carli.
//
//  See contribution from T.Carli et al from the HERA-LHC 
//  workshop - working group 3 
//  
//  Copyright (C) 2007 Mark Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: appl_igrid.h, v2.00  Fri Nov  2 05:31:03 GMT 2007 sutt $

// Fixme: this needs to be tidied up. eg there are too many different, 
//        and too many version of, accessors for x/y, Q2/tau etc there 
//        should be only  one set, for x and Q2 *or* y and tau, but 
//        not both. In fact maybe they should be for x and Q2, since y 
//        and tau should perhaps be purely an internal grid issue of no 
//        concern for the user.


#ifndef __APPL_IGRID_H
#define __APPL_IGRID_H

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cmath>


#include "appl_grid/Directory.h"
#include "appl_grid/appl_pdf.h"

#include "SparseMatrix3d.h"

#include "Cache.h"

namespace appl {

class grid;



class igrid {

private:

  // grid error exception
  class exception { 
  public:
    exception(const std::string& s) { std::cerr << s << std::endl; }; 
    exception(std::ostream& s)      { std::cerr << std::endl; };
  };

  typedef double (igrid::*transform_t)(double) const;

  // structure to store the x<->y transform pairs for 
  struct transform_vec {
    transform_vec() : mfx(0), mfy(0) { } 
    transform_vec( transform_t __fx, transform_t __fy) : mfx(__fx), mfy(__fy) { } 
    transform_t mfx;
    transform_t mfy;
  };
  

public:

  igrid();

  igrid(int NQ2,   double Q2min=10000.0, double Q2max=25000000.0,  int Q2order=5,  
	int Nx=50, double xmin=1e-5,     double xmax=0.9,          int xorder=5, 
	std::string transform="f", int Nproc=6, bool disflag=false);

  igrid(const igrid& g);

  // read grid from stored file
  igrid(TFile& f, const std::string& s);

  ~igrid();

  // optimise the grid dinemsions  
  void optimise() { optimise(m_Ntau, m_Ny1, m_Ny2); }
  void optimise(int NQ2, int Nx1, int Nx2);  
 
  // formatted print
  std::ostream&  print(std::ostream& s=std::cout) const {
    header(std::cout);
    for ( int i=0 ; i<m_Nproc ; i++ ) { 
      s << "sub process " << i << std::endl; 
      m_weight[i]->print();
    }
    return s;
  }
  
  // return the number of words used for storage
  int size() const {
    int _size = 0;
    for ( int i=0 ; i<m_Nproc ; i++ ) _size += m_weight[i]->size();
    return _size;
  }

  // trim unfilled elements
  void trim() { for ( int i=0 ; i<m_Nproc ; i++ )  m_weight[i]->trim(); }

  // inflate unfilled elements
  void untrim() { for ( int i=0 ; i<m_Nproc ; i++ ) m_weight[i]->untrim(); }

  // write to the current root directory
  void write(const std::string& name);
  
  // update grid with one set of event weights
  void fill(const double x1, const double x2, const double Q2, const double* weight);

  // fast filling with no interpolation for optimisation
  void fill_phasespace(const double x1, const double x2, const double Q2, const double* weight);

  void fill_index(const int ix1, const int ix2, const int iQ2, const double* weight);

  // get the sparse structure for easier access  
  const SparseMatrix3d* weightgrid(int ip) { return m_weight[ip]; }
  SparseMatrix3d**      weightgrid()       { return m_weight; } 


  // this section stores the available x<->y transforms.
  // the function pairs are stored in a std::map with a (const std::string) tag - the tag can 
  // be saved to a root file to uniquely identify the transform pair.
  // additional user defined transform pairs can *no longer* be added to the std::map since now
  // only local class functions can be used

  // transform 
  double fy(double x) const { return (this->*mfy)(x); }
  double fx(double x) const { return (this->*mfx)(x); }

  std::string transform() const { return m_transform; } 


  transform_t mfy;
  transform_t mfx;

  // initialise the transform map - no longer shared between class members
  void init_fmap() { 
    if ( m_fmap.size()==0 ) { 
      add_transform( "f",  &igrid::_fx,  &igrid::_fy  );
      add_transform( "f0", &igrid::_fx0, &igrid::_fy0  );
      add_transform( "f1", &igrid::_fx1, &igrid::_fy1  );
      add_transform( "f2", &igrid::_fx2, &igrid::_fy2  );
      add_transform( "f3", &igrid::_fx3, &igrid::_fy3  );
      add_transform( "f4", &igrid::_fx4, &igrid::_fy4  );
    }
  }

  /// add a transform
  void add_transform(const std::string transform, transform_t __fx, transform_t __fy ) { 
    if ( m_fmap.find(transform)!=m_fmap.end() ) { 
      throw exception("igrid::add_fmap() transform "+transform+" already in std::map");
    }
    m_fmap[transform] = transform_vec( __fx, __fy );
  }

  // define all these so that ymin=fy(xmin) rather than ymin=fy(xmax)
  double _fy(double x) const { return std::log(1/x-1); } 
  double _fx(double y) const { return 1/(1+std::exp(y)); } 

  double _fy0(double x) const { return -std::log(x); } 
  double _fx0(double y) const { return  std::exp(-y); } 

  double _fy1(double x) const { return std::sqrt(-std::log(x)); } 
  double _fx1(double y) const { return std::exp(-y*y); } 

  double _fy2(double x) const { return -std::log(x)+m_transvar*(1-x); }
  double _fx2(double y) const {
    // use Newton-Raphson: y = ln(1/x)
    // solve   y - yp - a(1 - exp(-yp)) = 0
    // deriv:  - 1 -a exp(-yp)

    if ( m_transvar==0 )  return std::exp(-y); 
        
    const double eps  = 1e-12;  // our accuracy goal
    const int    imax = 100;    // for safety (avoid infinite loops)
    
    double yp = y;
    double x, delta, deriv;
    for ( int iter=imax ; iter-- ; ) {
      x = std::exp(-yp);
      delta = y - yp - m_transvar*(1-x);
      if ( std::fabs(delta)<eps ) return x; // we have found good solution
      deriv = -1 - m_transvar*x;
      yp  -= delta/deriv;
    }
    // exceeded maximum iterations 
    std::cerr << "_fx2() iteration limit reached y=" << y << std::endl; 
    std::cout << "_fx2() iteration limit reached y=" << y << std::endl; 
    return std::exp(-yp); 
  }
  
  double _fy3(double x) const { return std::sqrt(-std::log10(x)); }
  double _fx3(double y) const { return std::pow(10,-y*y); } 

  // fastnlo dis transform
  double _fy4(double x) const { return -std::log10(x); }
  double _fx4(double y) const { return  std::pow(10,-y); } 





  // pdf weight function
  // double _fun(double x) { return pow(x,0.65)*pow((1-0.99*x),-3.3); } 
  // double _fun(double x) { double n=(1-0.99*x); return sqrt(x)/(n*n*n); } 
  // double _fun(double x) { return 1; } 
  
  // this is significantly quicker than pow(x,1.5)*pow(1-0.99*x,3) 
  static double _fun(double x)      { double n=(1-0.99*x); return std::sqrt(x*x*x)/(n*n*n); } 
  static double weightfun(double x) { return _fun(x); }
  

  // using log(log(Q2/mLambda)) or just log(log(Q2)) makes 
  // little difference for the LHC range
  static double ftau(double Q2) { return std::log(std::log(Q2/0.0625)); }
  static double fQ2(double tau) { return 0.0625*std::exp(std::exp(tau)); }

  
  // grid value accessors
  double gety1(int iy)    const { return m_weight[0]->yaxis()[iy]; }   
  double gety2(int iy)    const { return m_weight[0]->zaxis()[iy]; }   
  //  double gety(int iy)     const { return gety1(iy); } 

  double gettau(int itau) const { return m_weight[0]->xaxis()[itau]; } 

  // number of subprocesses

  int SubProcesses() const { return m_Nproc; } 

  // kinematic variable accessors
  // y (x) 
  int    Ny1()      const { return m_weight[0]->yaxis().N(); }    
  double y1min()    const { return m_weight[0]->yaxis().min(); }  
  double y1max()    const { return m_weight[0]->yaxis().max(); }  
  double deltay1()  const { return m_weight[0]->yaxis().delta(); }

  int    Ny2()      const { return m_weight[0]->zaxis().N(); }    
  double y2min()    const { return m_weight[0]->zaxis().min(); }  
  double y2max()    const { return m_weight[0]->zaxis().max(); }  
  double deltay2()  const { return m_weight[0]->zaxis().delta(); }

  //  int    Ny()      const { return Ny1(); } 
  //  double ymin()    const { return y1min(); }  
  //  double ymax()    const { return y2min(); }  
  //  double deltay()  const { return deltay1(); }  

  //  int yorder(int i)   { return m_yorder=i; }  
  int yorder()  const { return m_yorder; }  

  // tau (Q2)
  int    Ntau()     const { return m_weight[0]->xaxis().N(); } 
  double taumin()   const { return m_weight[0]->xaxis().min(); } 
  double taumax()   const { return m_weight[0]->xaxis().max(); } 
  double deltatau() const { return m_weight[0]->xaxis().delta(); } 
  
  //  int tauorder(int i)  { return m_tauorder=i; }  
  int tauorder() const { return m_tauorder; }  


  // maybe these are redundant and should be removed
  int    getNQ2()     const { return m_weight[0]->xaxis().N(); }
  double getQ2min()   const { return fQ2(taumin()); }
  double getQ2max()   const { return fQ2(taumax()); }
  
  int    getNx1()     const { return m_weight[0]->yaxis().N(); }
  double getx1min()   const { return fx(y1max()); }
  double getx1max()   const { return fx(y1min()); }

  int    getNx2()     const { return m_weight[0]->zaxis().N(); }
  double getx2min()   const { return fx(y2max()); }
  double getx2max()   const { return fx(y2min()); }

  //  int    getNx()     const { return getNx1(); } 
  //  double getxmin()   const { return getx2max(); }
  //  double getxmax()   const { return getx1max(); }

  /// these set the static class used to initialise the 
  /// local variables upon grid creation, since a value may 
  /// be needed by the constructor  
  static double transformvar()         { return transvar; }
  static double transformvar(double v) { return transvar=v; }

  bool   symmetrise(bool t=true)   { return m_symmetrise=t; }
  bool   isSymmetric() const       { return m_symmetrise; }

  bool   isOptimised() const       { return m_optimised; }
  bool   setOptimised(bool t=true) { return m_optimised=t; } 

  bool   isDISgrid() const       { return m_DISgrid; }
  bool   seDISgrid(bool t=true)  { return m_DISgrid=t; } 

  bool   reweight(bool t=true)   { return m_reweight=t; }

  bool   shrink(const std::vector<int>& keep);

  // setup the pdf grid for calculating the pdf using 
  // interpolation - needed if you actually want 
  // the interpolated values for the pdf's or if you want 
  // the grid to calculate the cross section for you
  void setuppdf(double (*alphas)(const double&),
		//		void (*pdf0)(const double& , const double&, double* ),
		//		void (*pdf1)(const double& , const double&, double* )=0,
		NodeCache* pdf0,
		NodeCache* pdf1=0,
		int nloop=0, 
		double rscale_factor=1,
		double fscale_factor=1,
		double beam_scale=1 );

  // get the interpolated pdf's
  //  void pdfinterp(double x1, double Q2, double* f);

  double convolute(NodeCache* pdf0,
		   NodeCache* pdf1,
		   // void   (*pdf0)(const double& , const double&, double* ), 
		   // void   (*pdf1)(const double& , const double&, double* ), 
		   appl_pdf* genpdf, 
		   double (*alphas)(const double& ), 
		   int     lo_order=0,  
		   int     nloop=0, 
		   double  rscale_factor=1,
		   double  fscale_factor=1,
		   double Escale=1 );
  

  
  /// convolute method for amcatnlo grids
  double amc_convolute(NodeCache* pdf0,
		       NodeCache* pdf1,
		       // void   (*pdf0)(const double& , const double&, double* ), 
		       // void   (*pdf1)(const double& , const double&, double* ), 
		       appl_pdf* genpdf, 
 		       double (*alphas)(const double& ), 
 		       int     lo_order=0,  
 		       int     nloop=0, 
 		       double  rscale_factor=1,
 		       double  fscale_factor=1,
 		       double Escale=1 );
  
  

  // some useful algebraic operators
  igrid& operator=(const igrid& g); 
  
  igrid& operator*=(const double& d) { 
    for ( int ip=0 ; ip<m_Nproc ; ip++ ) if ( m_weight[ip] ) (*m_weight[ip]) *= d; 
    return *this;
  } 

  // should really check all the limits and *everything* is the same
  igrid& operator+=(const igrid& g) { 
    for ( int ip=0 ; ip<m_Nproc ; ip++ ) {
      if ( m_weight[ip] && g.m_weight[ip] ) { 
	//if ( (*m_weight[ip]) == (*g.m_weight[ip]) ) (*m_weight[ip]) += (*g.m_weight[ip]);
	if ( m_weight[ip]->compare_axes( *g.m_weight[ip] ) ) (*m_weight[ip]) += (*g.m_weight[ip]); 
	else { 
	  throw exception("igrid::operator+=() grids do not match");
	}
      }
    }
    return *this;
  } 

  /// check that the grid axes match
  bool compare_axes(const igrid& g) const { 
    for ( int ip=0 ; ip<m_Nproc ; ip++ ) {
      if ( m_weight[ip] && g.m_weight[ip] ) { 
	if ( !m_weight[ip]->compare_axes( *g.m_weight[ip] ) )  return false;
	// if ( (*m_weight[ip]) != (*g.m_weight[ip]) )  return false;
      }
      if ( m_weight[ip]    && g.m_weight[ip]==0 ) return false; 
      if ( m_weight[ip]==0 && g.m_weight[ip]    ) return false; 
    }
    return true;
  }


  bool operator==(const igrid& g) const { 
    for ( int ip=0 ; ip<m_Nproc ; ip++ ) {
      if ( m_weight[ip] && g.m_weight[ip] ) { 
	if ( (*m_weight[ip]) != (*g.m_weight[ip]) ) return false;
      }
      if ( m_weight[ip]    && g.m_weight[ip]==0 ) return false; 
      if ( m_weight[ip]==0 && g.m_weight[ip]    ) return false; 
    }
    return true;
  } 

  bool operator!=(const igrid& g) const { return !((*this)==g); }


  // ouput header
  std::ostream& header(std::ostream& s) const;


private:

  // internal common construct for the different types of constructor
  void construct();

  // cleanup
  void deleteweights();
  void deletepdftable();

  // interpolation section - inline and static internals for calculation of the 
  // interpolation for storing on the grid nodes 

  // x (y) interpolation formula
  int fk1(double x) const {
    double y = fy(x);
    // make sure we are in the range covered by our binning
    if( y<y1min() || y>y1max() ) {
      if ( y<y1min() ) std::cerr <<"\tWarning: x1 out of range: x=" << x << "\t(y=" << y << ")\tBelow Delx=" << x-fx(y1min());
      else             std::cerr <<"\tWarning: x1 out of range: x=" << x << "\t(y=" << y << ")\tAbove Delx=" << x-fx(y1min());
      std::cerr << " ( " <<  fx(y1max())  << " - " <<  fx(y1min())  << " )"  
		<< "\ty=" << y << "\tDely=" << y-y1min() << " ( " << y1min() << " - " <<  y1max()  << " )" << std::endl;
      //     cerr << "\t" << m_weight[0]->yaxis() << "\n\t" 
      //          << m_weight[0]->yaxis().transform(fx) << std::endl; 
    }
    int k = (int)((y-y1min())/deltay1() - (m_yorder>>1)); // fast integer divide by 2
    if ( k<0 ) k=0;  
    // shift interpolation end nodes to enforce range
    if(k+m_yorder>=Ny1())  k=Ny1()-1-m_yorder;    
    return k;
  }

  int fk2(double x) const {
    double y = fy(x);
    // make sure we are in the range covered by our binning
    if( y<y2min() || y>y2max() ) {
      if ( y<y2min() ) std::cerr <<"\tWarning: x2 out of range: x=" << x << "\t(y=" << y << ")\tBelow Delx=" << x-fx(y2min());
      else             std::cerr <<"\tWarning: x2 out of range: x=" << x << "\t(y=" << y << ")\tAbove Delx=" << x-fx(y2min());
      std::cerr << " ( " <<  fx(y2max())  << " - " <<  fx(y2min())  << " )"  
	   << "\ty=" << y << "\tdely=" << y-y2min() << " ( " << y2min() << " - " <<  y2max()  << " )" << std::endl;
      //     cerr << "\t" << m_weight[0]->yaxis() << "\n\t" << m_weight[0]->yaxis().transform(fx) << std::endl; 
    }
    int k = (int)((y-y2min())/deltay2() - (m_yorder>>1)); // fast integer divide by 2
    if ( k<0 ) k=0;  
    // shift interpolation end nodes to enforce range
    if(k+m_yorder>=Ny2())  k=Ny2()-1-m_yorder;    
    return k;
  }

  
  // Q2 (tau) interpolation formula 
  int fkappa(double Q2) const {
    double tau = ftau(Q2);
    // make sure we are in the range covered by our binning
    if( tau<taumin() || tau>taumax() ) {
      std::cerr << "\tWarning: Q2 out of range Q2=" << Q2 
		<< "\t ( " << fQ2(taumin()) << " - " << fQ2(taumax()) << " )" << std::endl;
      //      cerr << "\t" << m_weight[0]->xaxis() << "\n\t" << m_weight[0]->xaxis().transform(fQ2) << std::endl; 
    }
    int kappa = (int)((tau-taumin())/deltatau() - (m_tauorder>>1)); // fast integer divide by 2
    // shift interpolation end nodes to enforce range
    if(kappa+m_tauorder>=Ntau()) kappa=Ntau()-1-m_tauorder;  
    if(kappa<0) kappa=0;
    return kappa;
  }
  
  //  int _fk(double x)      const { return fk(x);  }
  //  int _fkappa(double Q2) const { return fkappa(Q2); }

  // fast -1^i function
  static int pow1(int i) { return ( 1&i ? -1 : 1 ); } 

  // fast factorial
  static double fac(int i) {
    int j;
    static int ntop = 4;
    static double f[34] = { 1, 1, 2, 6, 24 }; 
    if ( i<0 )  {  std::cerr << "igrid::fac() negative input"  << std::endl; return 0;  }
    if ( i>33 ) {  std::cerr << "igrid::fac() input too large" << std::endl; return 0;  }
    while ( ntop<i ) {
      j=ntop++;
      f[ntop]=f[j]*ntop;
    } 
    return f[i];
  }

  // although not the best way to calculate interpolation coefficients,
  // it may be the best for our use, where the "y" values of the nodes 
  // are not yet defined at the time of evaluation.
  static double fI(int i, int n, double u) {
    if ( n==0 && i==0 )     return 1.0;
    if ( std::fabs(u-i)<1e-8 ) return 1.0;
    //    if ( std::fabs(u-n)<u*1e-8 ) return 1.0;
    double product = pow1(n-i) / ( fac(i)*fac(n-i)*(u-i) );
    for( int z=0 ; z<=n ; z++ )  product *= (u-z);
    return product;
  }
  

public:

  void setparent( grid* parent ) { m_parent=parent; }

private:


  /// parent grid so that it can access parent 
  /// grid paremeters
  grid* m_parent;

  // ranges of interest

  // x <-> y parameters
  int    m_Ny1; 
  double m_y1min; 
  double m_y1max;
  double m_deltay1;

  int    m_Ny2; 
  double m_y2min; 
  double m_y2max;
  double m_deltay2;

  int    m_yorder; 

  // tau <-> Q2 parameters
  int    m_Ntau; 
  double m_taumin; 
  double m_taumax;
  double m_deltatau;
  int    m_tauorder; 

  int    m_Nproc;   // number of subprocesses

  // grid state information

  // useful transform information for storage in root file 
  std::string                   m_transform;

  std::map<const std::string, transform_vec> m_fmap;

  static double  transvar;   // transform function parameter
  double         m_transvar; // local copy transform function parameter

  /// *don't* make m_reweight static!!! otherwise we can't mix 
  ///  reweighted and non-reweighted grids!!! 
  bool   m_reweight;    // reweight the pdf?
  
  bool   m_symmetrise;   // symmetrise the grid or not 
  bool   m_optimised;    // optimised?

  // the actual weight grids
  SparseMatrix3d**   m_weight;

  // pdf value table for convolution 
  // (NB: doesn't need to be a class variable)
  double*** m_fg1; 
  double*** m_fg2; 

  // values from the splitting functions
  double*** m_fsplit1; 
  double*** m_fsplit2; 

  // alpha_s table
  double*   m_alphas;

  // flag to emulate a 2d (Q2, x) grid of use the 
  // full 3d (Q2, x1, x2) grid
  bool m_DISgrid;

};

};

std::ostream& operator<<(std::ostream& s, const appl::igrid& mygrid);


#endif // __APPL_IGRID_H 
