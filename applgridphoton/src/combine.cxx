//
//   @file    combine.cxx         
//            simple code to add together many grids 
//
//   @author M.Sutton
// 
//   Copyright (C) 2013 M.Sutton (sutt@cern.ch)    
//
//   $Id: combine.cxx, v0.0   Sat 13 Jul 2013 09:54:51 CEST sutt $


#include <cassert>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <cstdlib>

#include <pineappl_capi.h>
#include "amconfig.h"

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
  s << "        --verbose  \t display grid documentation during add\n";
  s << "    -v, --version  \t displays the APPLgrid version\n";
  s << "    -h, --help     \t display this help\n";
  //s << "\nSee " << PACKAGE_URL << " for more details\n"; 
  s << "\nReport bugs to <" << PACKAGE_BUGREPORT << ">";
  s << std::endl;
  return 0;
}

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
  double rscale = 1; /// grid only

  /// flags to see whether hscale of rscale have been set
  bool rset = false;


  bool verbose = false; 

  /// by default add all histograms
  bool addall = true;

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
    else if ( std::string(argv[i])=="-a" || std::string(argv[i])=="--all" ) addall = true;
    else { 
      grids.push_back(argv[i]);
    }
  }

  if ( d!=1 ) { 
    if ( !rset ) rscale = d; 
  }

  if ( grids.size()<1 ) return usage(std::cerr, argc, argv);

  if ( output_grid=="" ) return usage(std::cerr, argc, argv);

  std::cout << "reading grids:\n";
  for (auto const& grid: grids) {
    std::cout << ' ' << grid << '\n';
  }
  std::cout << "output to: "      << output_grid << std::endl;

  std::vector<std::string> newgrids;
  
  if ( grids.size()==1 ) addall = true;
    
  if ( !addall ) {
    // section removed
    assert( false );
  }
  else {
    newgrids = grids;
  }
  
  if ( newgrids.empty() ) return 0;

  grids = newgrids;

  auto tstart = std::chrono::high_resolution_clock::now();

  /// now add the grids together
  
  auto* g = pineappl_grid_read(grids[0].c_str());


  /// will use the rms of the different reference histograms to estimate the proper 
  /// uncertainties

  for ( unsigned i=1 ; i<grids.size() ; i++ ) { 

    auto t = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now() - tstart).count();

    double remaining = t*grids.size()/i - t;

    std::cout << "applgrid-combine: adding grid " << i+1 << " of " << grids.size() 
	      << "\ttime so far " << t << "s"  
	      << "\testimated time remaining " << remaining << "s"  
	      << std::endl;  

    auto* _g = pineappl_grid_read(grids[i].c_str());
    pineappl_grid_merge_and_delete(g, _g);
  }

  if ( rscale!=1 ) { 
    pineappl_grid_scale(g, rscale);
  }

  if      ( wscale!=1 ) assert( false );
  else if ( weight!=0 )
  {
    pineappl_grid_scale(g, 1.0 / weight);
  }

  if ( shrink ) { 
    assert( false );
    //auto toptstart = std::chrono::high_resolution_clock::now();
    //g.shrink( newpdfname );
    //auto topt = std::chrono::duration_cast<std::chrono::milliseconds>(
    //  std::chrono::high_resolution_clock::now() - toptstart).count();
    //std::cout << argv[0] << ": compressed grid in " << topt << " ms" << std::endl;
  }

  if ( optimise ) { 
    auto toptstart = std::chrono::high_resolution_clock::now();
    pineappl_grid_optimize(g);
    auto topt = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - toptstart).count();
    std::cout << argv[0] << ": optimised grid in " << topt << " ms" << std::endl;
  }

  //  std::cout << "writing " << output_grid << std::endl;
  pineappl_grid_write(g, output_grid.c_str());
  pineappl_grid_delete(g);

  auto t = std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::high_resolution_clock::now() - tstart).count();
  
  std::cout << argv[0] << ": added " << grids.size() << " grids in " << t << " s" << std::endl; 
  std::cout << argv[0] << ": output to  " << output_grid << std::endl; 

  return 0;
}
