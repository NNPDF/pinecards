//   
//   fappl_grid.cxx        
//     
//      fortran callable wrapper functions for the c++  
//      appl grid project.
//                   
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: fappl.cxx, v1.0   Wed May 21 14:31:36 CEST 2008 sutt

#include <map>
#include <iostream>

#include "appl_grid/appl_grid.h"
#include "appl_grid/fastnlo.h"


#include "fappl_grid.h"

/// externally defined alpha_s and pdf routines for fortran 
/// callable convolution wrapper
extern "C" double fnalphas_(const double& Q); 
extern "C" void   fnpdf_(const double& x, const double& Q, double* f);



static int idcounter = 0;
static std::map<int,appl::grid*> _grid;


/// grid std::map management

void ngrids_(int& n) { n=_grid.size(); }

void gridids_(int* ids) { 
  std::map<int,appl::grid*>::iterator gitr = _grid.begin();
  for ( int i=0 ; gitr!=_grid.end() ; gitr++, i++ ) ids[i] = gitr->first;
}



void bookgrid_(int& id, const int& Nobs, const double* binlims) 
{
  id = idcounter++;

  std::map<int,appl::grid*>::iterator gitr = _grid.find(id);

  if ( gitr==_grid.end() ) {
    std::cout << "bookgrid_() creating grid with id " << id << std::endl; 
    _grid.insert(  std::map<int,appl::grid*>::value_type( id, new appl::grid( Nobs, binlims,
									      2,    10, 1000, 1,
									      12,  1e-5, 1, 3, 
									      "nlojet", 1, 3, "f3") ) ) ;									 
    //  _grid->symmetrise(true);
  }
  else throw appl::grid::exception( std::cerr << "grid with id " << id << " already exists" << std::endl );  

}


void readgrid_(int& id, const char* s) {
  id = idcounter++;
  std::map<int,appl::grid*>::iterator gitr = _grid.find(id);
  if ( gitr==_grid.end() ) { 
    appl::grid* g = new appl::grid(s);
    std::cout << "readgrid: " << id << " " << g->getDocumentation() << std::endl;
    _grid.insert(  std::map<int,appl::grid*>::value_type( id, g ) ); 
  }
  else throw appl::grid::exception( std::cerr << "grid with id " << id << " already exists" << std::endl );  
}


  
void printgrid_(const int& id) { 
  std::map<int,appl::grid*>::iterator gitr = _grid.find(id);
  if ( gitr!=_grid.end() ) { 
    std::cout << "grid id " << id << "\n" << *gitr->second << std::endl;
  }
  else throw appl::grid::exception( std::cerr << "No grid with id " << id << std::endl );
}


void printgrids_() { 
  std::map<int,appl::grid*>::iterator gitr = _grid.begin();
  for ( ; gitr!=_grid.end() ; gitr++ ) { 
    std::cout << "grid id " << gitr->first << "\n" << *gitr->second << std::endl;
  }
}

  
void printgriddoc_(const int& id) { 
  std::map<int,appl::grid*>::iterator gitr = _grid.find(id);
  if ( gitr!=_grid.end() ) { 
    std::cout << "grid id " << id << "\n" << gitr->second->getDocumentation() << std::endl;
  }
  else throw appl::grid::exception( std::cerr << "No grid with id " << id << std::endl );
}


void releasegrid_(const int& id) { 
  std::map<int,appl::grid*>::iterator gitr = _grid.find(id);
  if ( gitr!=_grid.end() )     { 
    delete gitr->second; 
    _grid.erase(gitr);
  }
  else throw appl::grid::exception( std::cerr << "No grid with id " << id << std::endl );
}


void releasegrids_() { 
  std::map<int,appl::grid*>::iterator gitr = _grid.begin();
  for ( ; gitr!=_grid.end() ; gitr++ ) { 
    delete gitr->second; 
    _grid.erase(gitr);
  }
}

void setckm_( const int& id, const double* ckm ) { 
  std::map<int,appl::grid*>::iterator gitr = _grid.find(id);
  if ( gitr!=_grid.end() ) {
    gitr->second->setckm( ckm );
  }
  else throw appl::grid::exception( std::cerr << "No grid with id " << id << std::endl );
}


void getckm_( const int& id, double* ckm ) { 
  std::map<int,appl::grid*>::iterator gitr = _grid.find(id);
  if ( gitr!=_grid.end() ) { 
    std::vector<std::vector<double> > __ckm = gitr->second->getckm();
    for ( unsigned i=0 ; i<__ckm.size() ; i++ ) {
      for ( unsigned j=0 ; j<__ckm[i].size() ; j++ ) {
	ckm[i*3+j] = __ckm[i][j];
      }
    }
  }
  else throw appl::grid::exception( std::cerr << "No grid with id " << id << std::endl );
}



void redefine_(const int& id, 
	       const int& iobs, const int& iorder, 
	       const int& NQ2,  const double& Q2min, const double& Q2max, 
	       const int& Nx,   const double&  xmin, const double&  xmax) 
{
  std::map<int,appl::grid*>::iterator gitr = _grid.find(id);
  if ( gitr!=_grid.end() ) {
    gitr->second->redefine(iobs, iorder, 
			   NQ2, Q2min, Q2max, 
			   Nx,   xmin,  xmax); 
  }
  else throw appl::grid::exception( std::cerr << "No grid with id " << id << std::endl );
  
} 



int getnbins_(const int& id) { 
  std::map<int,appl::grid*>::iterator gitr = _grid.find(id);
  if ( gitr!=_grid.end() ) return gitr->second->Nobs();
  else throw appl::grid::exception( std::cerr << "No grid with id " << id << std::endl );
}

int getbinnumber_(const int& id, double& x) { 
  std::map<int,appl::grid*>::iterator gitr = _grid.find(id);
  if ( gitr!=_grid.end() ) return gitr->second->obsbin(x);
  else throw appl::grid::exception( std::cerr << "No grid with id " << id << std::endl );
}

double getbinlowedge_(const int& id, int& bin) { 
  std::map<int,appl::grid*>::iterator gitr = _grid.find(id);
  if ( gitr!=_grid.end() ) return gitr->second->obslow(bin);
  else throw appl::grid::exception( std::cerr << "No grid with id " << id << std::endl );
}

double getbinwidth_(const int& id, const int& bin) { 
  std::map<int,appl::grid*>::iterator gitr = _grid.find(id);
  if ( gitr!=_grid.end() ) return gitr->second->deltaobs(bin);
  else throw appl::grid::exception( std::cerr << "No grid with id " << id << std::endl );
}




void convolute_(const int& id, double* data) { 
  convolutewrap_(id, data, fnpdf_, fnalphas_); 
}


void convolutewrap_(const int& id, double* data, 
		    void (*pdf)(const double& , const double&, double* ),  
		    double (*alphas)(const double& ) ) {  
  std::map<int,appl::grid*>::iterator gitr = _grid.find(id);
  if ( gitr!=_grid.end() ) { 
    appl::grid*    g = gitr->second;
    std::vector<double> v = g->vconvolute( pdf, alphas);
    for ( unsigned i=0 ; i<v.size() ; i++ ) data[i] = v[i];      
  }
  else throw appl::grid::exception( std::cerr << "No grid with id " << id << std::endl );
}





void convoluteorder_(const int& id, const int& nloops, double* data) { 
  std::map<int,appl::grid*>::iterator gitr = _grid.find(id);
  if ( gitr!=_grid.end() ) { 
    appl::grid*    g = gitr->second;
    std::vector<double> v = g->vconvolute(fnpdf_, fnalphas_, nloops);
    for ( unsigned i=0 ; i<v.size() ; i++ ) data[i] = v[i];      
  }
  else throw appl::grid::exception( std::cerr << "No grid with id " << id << std::endl );
}




void fullconvolutewrap_(const int& id, double* data, 
			void (*pdf)(const double& , const double&, double* ),  
			double (*alphas)(const double& ),
			const int& nloops,
			const double& rscale, const double& fscale  ) {  
  std::map<int,appl::grid*>::iterator gitr = _grid.find(id);
  if ( gitr!=_grid.end() ) { 
    appl::grid*    g = gitr->second;
    std::vector<double> v = g->vconvolute( pdf, alphas, nloops, rscale, fscale);
    for ( unsigned i=0 ; i<v.size() ; i++ ) data[i] = v[i];      
  }
  else throw appl::grid::exception( std::cerr << "No grid with id " << id << std::endl );
}


void fullconvolute_(const int& id, double* data, 
		    const int& nloops,
		    const double& rscale, const double& fscale  ) {  
  fullconvolutewrap_( id, data, fnpdf_, fnalphas_, nloops, rscale, fscale);
}



/// convolute functions with beam energy scaling 

void escaleconvolute_(const int& id, double* data, const double& Escale) { 
  escaleconvolutewrap_(id, data, fnpdf_, fnalphas_, Escale); 
}

void escaleconvolutewrap_(const int& id, double* data, 
			  void (*pdf)(const double& , const double&, double* ),  
			  double (*alphas)(const double& ), 
			  const double& Escale ) {  
  std::map<int,appl::grid*>::iterator gitr = _grid.find(id);
  if ( gitr!=_grid.end() ) { 
    appl::grid*    g = gitr->second;
    std::vector<double> v = g->vconvolute( pdf, alphas, g->nloops(), 1, 1, Escale );
    for ( unsigned i=0 ; i<v.size() ; i++ ) data[i] = v[i];      
  }
  else throw appl::grid::exception( std::cerr << "No grid with id " << id << std::endl );
}



void escalefullconvolutewrap_(const int& id, double* data, 
			      void (*pdf)(const double& , const double&, double* ),  
			      double (*alphas)(const double& ),
			      const int& nloops,
			      const double& rscale, const double& fscale,
			      const double& Escale ) {  
  std::map<int,appl::grid*>::iterator gitr = _grid.find(id);
  if ( gitr!=_grid.end() ) { 
    appl::grid*    g = gitr->second;
    std::vector<double> v = g->vconvolute( pdf, alphas, nloops, rscale, fscale, Escale);
    for ( unsigned i=0 ; i<v.size() ; i++ ) data[i] = v[i];      
  }
  else throw appl::grid::exception( std::cerr << "No grid with id " << id << std::endl );
}


void escalefullconvolute_(const int& id, double* data, 
			  const int& nloops,
			  const double& rscale, const double& fscale,
			  const double& Escale ) {  
  escalefullconvolutewrap_(id, data, 
			   fnpdf_, fnalphas_, nloops, rscale, fscale, Escale );
  
}


void writegrid_(const int& id, const char* s) { 
  std::map<int,appl::grid*>::iterator gitr = _grid.find(id);
  if ( gitr!=_grid.end() ) { 
    std::cout << "writegrid_() writing " << s << "\tid " << id << std::endl;
    appl::grid* g = gitr->second;
    g->trim();
    //   g->print();
    g->Write(s);
  }
  else throw appl::grid::exception( std::cerr << "No grid with id " << id << std::endl );
}



void fillgrid_(const int& id, 
	       const int& ix1, const int& ix2, const int& iQ,  
	       const int& iobs, 
	       const double* w, 
	       const int& iorder ) { 
  //  std::cout << "ix " << ix1 << " " << ix2 << "  iQ" << iQ << " " << iobs << "  " << iorder << std::endl;  
  std::map<int,appl::grid*>::iterator gitr = _grid.find(id);
  if ( gitr!=_grid.end() ) { 
    gitr->second->fill_index(ix1, ix2, iQ, iobs, w, iorder);
  }  
  else throw appl::grid::exception( std::cerr << "No grid with id " << id << std::endl );
 
}


void readfastnlogrids_( int* ids, const char* s ) { 

  /// create the fastnlo grids
  fastnlo f(s);

  /// don't want the grids managed by the fastnlo object, 
  /// manage them in fortran with the std::map
  f.manageGrids(false);

  ///copy to the fortran accessible grid std::map
  std::vector<appl::grid*> grids = f.grids();

  //  std::cout << "hooray!" << std::endl;
  
  for ( unsigned i=0 ; i<grids.size() ; i++ ) { 
    int id = idcounter++;
    std::map<int,appl::grid*>::iterator gitr = _grid.find(id);
    if ( gitr==_grid.end() )  { 
      _grid.insert(  std::map<int,appl::grid*>::value_type( id, grids[i] ) );
      // std::cout << grids[i]->getDocumentation() << std::endl;
    }
    else throw appl::grid::exception( std::cerr << "grid with id " << id << " already exists" << std::endl );
    ids[i] = id;
  }  

}


void getrun_( const int& id, double& run ) { 
  std::map<int,appl::grid*>::iterator gitr = _grid.find(id);
  if ( gitr!=_grid.end() ) { 
    appl::grid*    g = gitr->second;
    run = g->run();
  }
  else throw appl::grid::exception( std::cerr << "No grid with id " << id << std::endl );
}


  

