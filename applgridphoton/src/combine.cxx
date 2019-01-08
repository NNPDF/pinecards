//
//   @file    combine.cxx         
//            simple code to add together many grids 
//
//   @author M.Sutton
// 
//   Copyright (C) 2013 M.Sutton (sutt@cern.ch)    
//
//   $Id: combine.cxx, v0.0   Sat 13 Jul 2013 09:54:51 CEST sutt $


#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <cstdlib>

#include "appl_grid/appl_grid.h"
#include "amconfig.h"

#include "appl_grid/appl_timer.h"


#include "TFile.h"

int usage(std::ostream& s, int argc, char** argv) { 
  if ( argc<1 ) return -1; /// should never be the case 
  s << "Usage: " << argv[0] << " [OPTIONS] -o output_grid.root  input_grid.root [input_grid1.root ... input_gridN.root]\n\n";
  s << "  APPLgrid \'" << argv[0] << "\' adds " << PACKAGE_STRING << " grid files together into a single grid\n\n"; 
  s << "Configuration: \n";
  s << "    -o filename   \t name of output grid (filename required)\n\n";
  s << "Options: \n";
  s << "    -g, --gscale  value\t rescale output grid by value, \n";
  s << "    -r, --rscale  value\t rescale reference histogram by value, \n";
  s << "    -s, --scale   value\t rescale both output grid and reference histogram by value\n";
  //  s << "    -a, --all     \tadd all grids (default)\n";
  s << "    -w, --wscale  value\t rescale the weight normalisation for the output grid\n";
  s << "        --weight  value\t set the value of the weight normalisation for the output grid directly\n";
  s << "        --optimise     \t optimise the output grid\n";
  s << "        --compress     \t try to reduce the number of parton luminosity\n"
    << "                       \t combinations\n";
  s << "    -c, --chi2    value\t if set, exclude grids with a chi2 with respect\n"
    << "                       \t to the median larger than value\n";
  s << "        --verbose  \t display grid documentation during add\n";
  s << "    -v, --version  \t displays the APPLgrid version\n";
  s << "    -h, --help     \t display this help\n";
  //s << "\nSee " << PACKAGE_URL << " for more details\n"; 
  s << "\nReport bugs to <" << PACKAGE_BUGREPORT << ">";
  s << std::endl;
  return 0;
}





/// bins 


struct bin {

  bin( double y=0, double ye=0 ) : _x(0), _y(y), _ye(ye), _y2(_y*_y), _n(1) { 
    _ys.push_back(_y);
  } 

  bin( TH1D* h, int i ) : _x(h->GetBinCenter(i)), _y(h->GetBinContent(i)), _ye(h->GetBinError(i)), _y2(_y*_y), _n(1) { 
    _ys.push_back(_y);
  } 

  bin( const bin& b) : _x(b._x), _y(b._y), _ye(b._ye), _y2(b._y2), _n(b._n), _ys(b._ys) { } 

  unsigned size() const { return _ys.size(); }

  bin& operator+=( const bin& b) { 
    _y  += b._y;
    _y2 += b._y2;
    _ye  = std::sqrt( _ye*_ye + b._ye*b._ye );
    _n  += b._n;
    _ys.push_back(b._y);
    return *this;
  }

  bin& operator*=( double d ) { 
    _y  *= d;
    _ye *= d;
    _y2 *= d*d;
    for ( unsigned i=0 ; i<_ys.size() ; i++ ) _ys[i] *= d;
    return *this;
  }

  bin& operator/=( double d ) { return operator*=(1/d); } 


  bool operator<( const bin& b)  const { return _y<b._y; }
  bool operator>( const bin& b)  const { return _y>b._y; }
  bool operator<=( const bin& b) const { return _y<=b._y; }
  bool operator>=( const bin& b) const { return _y>=b._y; }
  bool operator==( const bin& b) const { return _y==b._y; }
  bool operator!=( const bin& b) const { return _y!=b._y; }
  

  double _x;
  double _y;
  double _ye;

  double _y2;

  int    _n;

  std::vector<double> _ys;


  double mean()   const { return _y/_n; }  
  double rms()    const { return std::sqrt( ( _y2 - _y*_y/_n )/_n ); }  
  double mean_error()  const { return std::sqrt( ( _y2 - _y*_y/_n )/(_n*_n) ); }  


  double median() const {
    std::vector<double> v = _ys;
    std::sort( v.begin(), v.end() );
    if ( (v.size()&1) ) return v[v.size()/2];
    return 0.5*(v[v.size()/2] + v[v.size()/2-1]);
  }


};



bin operator+( const bin& b0, const bin& b1 ) { return bin( b0._y+b1._y, std::sqrt( b0._ye*b0._ye* + b1._ye*b1._ye) ); }
bin operator*( double d, const bin& b )       { return bin( d*b._y, d*b._ye ); }



/// cross section 

struct Xsection : public std::vector<bin> {
  
  Xsection(TH1D* h) { // : _bins(*this) { 
    for ( int i=0 ; i<h->GetNbinsX() ; i++ ) _bins.push_back( bin( h, i+1 ) );
  }
  
  bin  operator[](int i) const { return _bins[i]; }
  bin& operator[](int i)       { return _bins[i]; }

  unsigned size()  const { return _bins.size(); };
  bool     empty() const { return _bins.empty(); };

  Xsection& operator+=( Xsection& x) { 
    for ( unsigned i=0 ; i<_bins.size() ; i++ ) _bins[i] += x._bins[i];
    return *this;
  }

  Xsection& operator*=( double d ) { 
    for ( unsigned i=0 ; i<_bins.size() ; i++ ) _bins[i] *= d;
    return *this;
  }


  std::vector<bin>  mean() const {
    std::vector<bin> _mean;
    for ( unsigned i=0 ; i<_bins.size() ; i++ ) {
      _mean.push_back( bin( _bins[i].mean(), _bins[i].mean_error() ) ); 
    }
    return _mean;
  }


  std::vector<bin>  median() const {
    std::vector<bin> _median;
    for ( unsigned i=0 ; i<_bins.size() ; i++ ) {
      _median.push_back( bin( _bins[i].median(), _bins[i].mean_error() ) ); 
    }
    return _median;
  }

  /// data members 

  std::vector<bin> _bins;

}; 



Xsection operator*( const Xsection& x, double d ) { 
  Xsection xs(x);
  return (xs *= d);
}


Xsection operator*( double d, const Xsection& x )        { return x*d; }


/// calculate a chi2 of the Xsecstion with respect to some value 

double chi2( const std::vector<bin>& xs1, const Xsection& xs2, double fsigma ) { 

  if ( xs1.empty() ) return 0;
  
  double c2 = 0;

  for ( unsigned i=0 ; i<xs1.size() ; i++ ) { 
    double d = xs1[i]._y - xs2[i]._y;
    
    /// take fractional sigma and turn it to actual sigma
    double s = xs1[i]._y*fsigma;

    /// 5*"sigma on the mean" will be treated as our
    /// ad hoc figure of merrit
    c2 += d*d/(s*s);
  }

  /// return chi2 per dof
  return c2/xs1.size();

}


/// streamers 

std::ostream& operator<<( std::ostream& s, const bin& b ) { 
  return s << "( " << b._x << ":\t" << b._y << " +- " << b._ye << " " << ( b._y!=0 ? 100*b._ye/b._y : 0 ) << "%   n: " << b._n << " )";
}

template<class T>
std::ostream& operator<<( std::ostream& s, const std::vector<T>& v ) { 
  for ( unsigned i=0 ; i<v.size() ; i++ ) s << " " << v[i] << "\n";
  return s;
}

std::ostream& operator<<( std::ostream& s, const Xsection& x ) {  return s << x._bins; }





int main(int argc, char** argv) { 

  /// check correct number ofg parameters
  if ( argc<2 ) return usage( std::cerr, argc, argv );


  std::string output_grid = "";

  /// handle the "perform and exit" parameters 
  for ( int i=1 ; i<argc ; i++ ) { 
    if ( std::string(argv[i])=="-h" || std::string(argv[i])=="--help" )    return usage( std::cout, argc, argv ); 
    if ( std::string(argv[i])=="-v" || std::string(argv[i])=="--version" ) {
      std::cout << argv[0] << " APPLgrid version " << PACKAGE_VERSION << std::endl; 
      return 0;
    }
  }

  /// scaling factors
  double d = 1;      /// overall
  double hscale = 1; /// histogram only
  double rscale = 1; /// grid only

  /// flags to see whether hscale of rscale have been set
  bool hset = false;
  bool rset = false;


  bool verbose = false; 

  /// by default add all histograms
  bool addall = true;

  /// if set, the following will disable adding all histigrams 
  /// exclude those with a chi2 larger than this value
  double chi2_limit = 0;

  /// the list of grids to process
  std::vector<std::string> grids;

  double weight = 0;
  double wscale   = 1;

  bool optimise = false;
  bool shrink   = false;
  std::string newpdfname="";
 
  /// handle configuration parameters
  for ( int i=1 ; i<argc ; i++ ) { 
    if      ( std::string(argv[i])=="--verbose" ) verbose = true;
    else if ( std::string(argv[i])=="-o" ) { 
      ++i;
      if ( i<argc ) output_grid = argv[i];
      else  return usage( std::cerr, argc, argv );
    }
    else if ( std::string(argv[i])=="-s" || std::string(argv[i])=="--scale" ) { 
      ++i;
      if ( i<argc ) d = std::atof(argv[i]);
      else  return usage( std::cerr, argc, argv );
    }
    else if ( std::string(argv[i])=="-g" || std::string(argv[i])=="--gscale" ) { 
      ++i;
      if ( i<argc ) { 
	rscale = std::atof(argv[i]);
	rset = true;
      }
      else  return usage( std::cerr, argc, argv );
    }
    else if ( std::string(argv[i])=="--optimise" ) optimise = true;
    else if ( std::string(argv[i])=="--compress" ) { 
      ++i;
      if ( i<argc ) { 
	newpdfname = argv[i];
	shrink = true;
      }
      else  return usage( std::cerr, argc, argv );
    }
    else if ( std::string(argv[i])=="--weight" ) {  
      ++i;
      if ( i<argc ) { 
	weight = std::atof(argv[i]);
      }
      else  return usage( std::cerr, argc, argv );
    }
    else if ( std::string(argv[i])=="-w" || std::string(argv[i])=="--wscale" ) {  
      ++i;
      if ( i<argc ) { 
	wscale = std::atof(argv[i]);
      }
      else  return usage( std::cerr, argc, argv );
    }
    else if ( std::string(argv[i])=="-r" || std::string(argv[i])=="--rscale" ) { 
      ++i;
      if ( i<argc ) { 
	hscale = std::atof(argv[i]);
	hset = true;
      }
      else  return usage( std::cerr, argc, argv );
    }
    else if ( std::string(argv[i])=="-c" || std::string(argv[i])=="--chi2" ) { 
      ++i;
      if ( i<argc ) { 
	chi2_limit = std::atof(argv[i]);
	addall = false;
      }
      else  return usage( std::cerr, argc, argv );
    }
    else if ( std::string(argv[i])=="-a" || std::string(argv[i])=="--all" ) addall = true;
    else { 
      grids.push_back(argv[i]);
    }
  }

  if ( d!=1 ) { 
    if ( !hset ) hscale = d;
    if ( !rset ) rscale = d; 
  }

  if ( grids.size()<1 ) return usage(std::cerr, argc, argv);

  if ( output_grid=="" ) return usage(std::cerr, argc, argv);

  /// before adding the grids together, need tpo go through them to reject
  /// the outliers

  /// start by reading all the reference histograms and calculating the means etc

  std::cout << "reading grids:\n" << grids << std::endl;
  std::cout << "output to: "      << output_grid << std::endl;

  std::vector<TH1D*> ref;
  ref.reserve(grids.size());


  std::vector<std::string>::iterator gitr=grids.begin();
  while ( gitr!=grids.end()  ) { 
    TFile f(gitr->c_str());
    TH1D* _h = (TH1D*)f.Get("grid/reference");

    /// remove obvious non-grid files
    if ( _h ) { 
      _h->SetDirectory(0);
      ref.push_back(_h);
      gitr++;
    }
    else {
      grids.erase( gitr );
    }
  }


  if ( ref.empty() ) { 
    std::cerr << "grid list empty " << std::endl;
    return 0;
  }

  std::vector<std::string> newgrids;
  
  if ( grids.size()==1 ) addall = true;
    
  if ( !addall ) { 

    /// now get cross section measures and the rms, medians etc

    Xsection txsec( ref[0] );
    
    for ( unsigned i=1 ; i<ref.size() ; i++ ) { 
      Xsection _txsec( ref[i] );
      txsec += _txsec;
    }  
        
    std::vector<bin> _mean   = txsec.mean();
    std::vector<bin> _median = txsec.median();
    
    /// find median fractional fractional rms
    
    std::vector<double> rms; 
    rms.reserve(_mean.size());
 
    for ( unsigned i=0 ; i<_mean.size() ; i++ ) { 
      if ( _mean[i]._y ) rms.push_back(_mean[i]._ye/_mean[i]._y);
      else               rms.push_back(100);
    }
    
    std::sort(rms.begin(),rms.end());
    
    double median_error = 0;
  
    if ( !rms.empty() ) { 
      if ( rms.size()&1 ) { 
	median_error = rms[rms.size()/2];
	//  std::cout << "median uncertainty " << median_error << std::endl;
      }
      else { 
	median_error = 0.5*(rms[rms.size()/2]+rms[rms.size()/2-1]);
	//      std::cout << "median uncertainty " << median_error << std::endl;
      }
    }
    
    
    //  std::cout << "mean: \n" << _mean << "\nmedian: \n" << _median << std::endl;
    
    /// now loop through them again, find thos with large fluctuations and exclude them
    
    double fsigma = median_error*std::sqrt(grids.size());
    
    for ( unsigned i=1 ; i<ref.size() ; i++ ) { 
      Xsection _txsec( ref[i] );
      
      double c2 = chi2( _median, _txsec, fsigma );
      
      //    std::cout << grids[i] << "\tchi2 " << c2 << std::endl;
      
      /// exclude any grids where the cross section is outside 4 sigma of 
      /// the median fractional uncertainty
      
      if ( c2<chi2_limit ) newgrids.push_back( grids[i] );
      else { 
	std::cout << "excluding " << grids[i] << "\tchi2 " << c2 << std::endl;
      }
    }
    
  }
  else {
    newgrids = grids;
  }
  
  if ( newgrids.empty() ) return 0;

  grids = newgrids;

  struct timeval tstart = appl_timer_start();

  /// now add the grids together
  
  appl::grid  g( grids[0] );

  g.untrim();


  /// will use the rms of the different reference histograms to estimate the proper 
  /// uncertainties

  std::vector<bin> bxsec;
  TH1D* h = g.getReference();

  //  for ( int i=0 ; i<h->GetNbinsX() ; i++ ) bxsec.push_back( bin(h,i+1) );

  //  std::cout << bxsec << std::endl;

  Xsection xsec( g.getReference() );

  //  std::cout << xsec << std::endl;

  if ( verbose ) std::cout << g.getDocumentation() << std::endl;

  for ( unsigned i=1 ; i<grids.size() ; i++ ) { 

    double t = appl_timer_stop( tstart )*0.001; 

    double remaining = t*grids.size()/i - t;

    std::cout << "applgrid-combine: adding grid " << i+1 << " of " << grids.size() 
	      << "\ttime so far " << t << "s"  
	      << "\testimated time remaining " << remaining << "s"  
	      << std::endl;  

    appl::grid  _g( grids[i] );
    _g.untrim();
    if ( verbose ) std::cout << _g.getDocumentation() << std::endl;
    g += _g;

    Xsection _xsec( _g.getReference() );

    //    std::cout << i << " " <<  xsec << std::endl;
  
    xsec += _xsec;

    //   std::cout << i << " " << _xsec << std::endl;
    //   std::cout << i << "\n" <<  xsec.mean() << std::endl;

  }


  ///  for ( int i=0 ; i<h->GetNbinsX() ; i++ ) { std::cout << "h " << i << " " << h->GetBinContent(i+1) << std::endl; }   
  ///  std::cout << "raw " << xsec << "\n" << xsec.mean() << "\n" << (xsec*grids.size()).mean() << std::endl;
  
  /// get the errors, and 

  if ( grids.size()>1 ) { 
    std::vector<bin> m = xsec.mean();  

    for ( unsigned i=0 ; i<m.size() ; i++ ) { 
      h->SetBinContent(i+1, m[i]._y*grids.size() );
      h->SetBinError(i+1, m[i]._ye*grids.size() );
    }
  }    

  if ( rscale!=1 ) { 
    g *= rscale;
    g.getReference()->Scale( 1/rscale );
  }

  //  if ( hscale!=rscale ) g.getReference()->Scale( hscale/rscale );
  if ( hscale!=1 ) g.getReference()->Scale( hscale );

  if      ( wscale!=1 ) g.run() *= wscale;
  else if ( weight!=0 ) g.run()  = weight;

  if ( shrink ) { 
    struct timeval toptstart = appl_timer_start(); 
    g.shrink( newpdfname ); 
    double topt = appl_timer_stop( toptstart ); 
    std::cout << argv[0] << ": compressed grid in " << topt << " ms" << std::endl;   
  }

  if ( optimise ) { 
    struct timeval toptstart = appl_timer_start(); 
    g.optimise();
    double topt = appl_timer_stop( toptstart ); 
    std::cout << argv[0] << ": optimised grid in " << topt << " ms" << std::endl;   
  }

  //  std::cout << "writing " << output_grid << std::endl;
  g.Write(output_grid);

  double t = appl_timer_stop( tstart )*0.001; 
  
  std::cout << argv[0] << ": added " << grids.size() << " grids in " << t << " s" << std::endl; 
  std::cout << argv[0] << ": output to  " << output_grid << std::endl; 

  return 0;
}
