#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include <LHAPDF/LHAPDF.h>
#include <appl_grid/appl_grid.h>

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

int main(int argc, char* argv[])
{
    // set LHAPDF low verbosity
    LHAPDF::setVerbosity(0);

    if (argc != 3)
    {
        std::cerr << "Usage: applcheck PDF-set-name applgrid-file\n";
        return 1;
    }

    appl::grid g(argv[2]);
    const int nloops = g.nloops();

    // initialise the PDF set via LHAPDF6
    pdf.reset(LHAPDF::mkPDF(argv[1], 0));
    use_photon = pdf->hasFlavor(22);
    std::vector<double> const& xsec_appl = g.vconvolute(evolvepdf, alphaspdf, nloops);

    for (std::size_t i = 0; i != xsec_appl.size(); ++i)
    {
        double const xsec = xsec_appl.at(i);
        std::cout << "    bin #" << i << ": " << std::scientific << xsec << " [pb(/GeV)]\n";
    }

    pdf.release();
}
