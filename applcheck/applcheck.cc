/*
This program convolves an applgrid (for a total cross section)
with the central grid of a set of PDFs and outputs its value (in pb),
*/

#include <iostream>
#include <fstream>
#include "LHAPDF/LHAPDF.h"
#include "appl_grid/appl_grid.h"

extern "C" void evolvepdf_(const double& , const double& , double* ); 
extern "C" double alphaspdf_(const double& Q);

using namespace LHAPDF;
using namespace std;
using namespace appl;

int main(int argc, char* argv[]) {

  //Set LHAPDF low verbosity
  LHAPDF::setVerbosity(0);

  if (argc != 3)
  {
    std::cerr << "Usage: applcheck PDF-set-name applgrid-file\n";
    return 1;
  }

  //Define the input PDF sets
  string nameset = argv[1];
  //Define the applgrids
  string applgridname = argv[2];
 
  string gridname= applgridname;
  appl::grid g(gridname);
  //g.trim();
  const int nloops = g.nloops();
  cout << "applgrid initialised" << endl;

  //Initialise the PDF set via LHAPDF6
  cout << "Initialising PDF set" << endl;
  std::string _pdfname=nameset;
  cout << "PDF set " << _pdfname << endl;
  int imem_init = 0;
  LHAPDF::initPDFSet( _pdfname, imem_init );
  LHAPDF::initPDF( 0 );
  std::vector<double> const& xsec_appl = g.vconvolute( evolvepdf_, alphaspdf_ , nloops);

  for (std::size_t i = 0; i != xsec_appl.size(); ++i)
  {
    double xsec = xsec_appl.at(i);
    cout << "   bin #" << i << ": " << std::scientific << xsec << " [pb]" << endl;
  }
}
