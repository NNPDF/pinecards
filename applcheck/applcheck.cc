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
 
  //Define bin number
  int nbin=1;
 
  //Initialise APPLGRID vector
  vector<string> applgrids;
  vector<int> appl_bin;

  for(int ibin=0; ibin<nbin; ibin++)
    {
      applgrids.push_back(applgridname);
      appl_bin.push_back(ibin);
    }

  //Initialise process
  int iproc, nproc;
  iproc = -1;
  nproc=applgrids.size();
  cout << "number of processes " << nproc << endl;

  //Initialise cross section
  int nbins=0;
  double xsec=0.0;

  for(int ibin=0; ibin<nbin; ibin++)
    {
      cout << "Bin #" << ibin << " ... ";
      
      //Initialize the APPLGRID
      iproc=ibin+nbins;
      stringstream sbin;
      sbin << appl_bin.at(iproc);
      cout << "Initializing the APPLGRID" << endl;
      string gridname= applgridname;
      appl::grid g(gridname);
      g.trim();
      const int nloops = g.nloops();
      cout << "applgrid initialised" << endl;
      
      //Initialise the PDF set via LHAPDF6
      cout << "Initialising PDF set" << endl;
      std::string _pdfname=nameset;
      cout << "PDF set " << _pdfname << endl;
      int imem_init = 0;
      LHAPDF::initPDFSet( _pdfname, imem_init );
      LHAPDF::initPDF( 0 );
      std::vector<double> xsec_appl = g.vconvolute( evolvepdf_, alphaspdf_ , nloops);
      

	      
      xsec = xsec_appl.at( appl_bin.at(iproc) );
      cout << "   " << xsec << " [pb]" << endl;
      
      nbins=nbins+nbin;
      cout << "done" << endl;
    }
  
  if(nbins!=nproc)
    {
      cout << "Mismatch between number of bins and number of processes" << endl;
      cout << "nbins = " << nbins << "nproc = " << nproc << endl;
    } 
}

