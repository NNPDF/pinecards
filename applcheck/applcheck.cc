/*
This program convolves an applgrid (for a total cross section)
with the central grid of a set of PDFs and outputs its value (in pb),
*/

#include <iostream>
#include <fstream>
#include <memory>
#include "LHAPDF/LHAPDF.h"
#include "appl_grid/appl_grid.h"

std::unique_ptr<LHAPDF::PDF> pdf;
bool use_photon;

extern "C" void evolvepdf(double const& x, double const& q, double* xfx)
{
    // 2x 6 quark flavour + gluon + photon
    static std::vector<double> buffer(2 * 6 + 1 + 1);

    pdf->xfxQ(x, q, buffer);
    std::copy(buffer.begin(), buffer.end(), xfx);

    if (use_photon)
    {
        xfx[2 * 6 + 1] = pdf->xfxQ(22, x, q);
    }
}

extern "C" double alphaspdf(double const& q)
{
    return pdf->alphasQ(q);
}

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
  pdf.reset(LHAPDF::mkPDF(_pdfname, imem_init));
  use_photon = pdf->hasFlavor(22);
  std::vector<double> const& xsec_appl = g.vconvolute(evolvepdf, alphaspdf, nloops);

  for (std::size_t i = 0; i != xsec_appl.size(); ++i)
  {
    double xsec = xsec_appl.at(i);
    cout << "   bin #" << i << ": " << std::scientific << xsec << " [pb]" << endl;
  }

  pdf.release();
}
