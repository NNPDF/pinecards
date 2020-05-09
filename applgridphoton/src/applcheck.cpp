#include <algorithm>
#include <array>
#include <cstddef>
#include <iostream>
#include <memory>
#include <vector>

#include <LHAPDF/LHAPDF.h>
#include <pineappl_capi.h>

double xfx(int32_t pdg_id, double x, double q2, void* state)
{
    auto* pdf = static_cast <LHAPDF::PDF*> (state);
    return pdf->xfxQ2(pdg_id, x, q2);
}

double alphas(double q2, void* state)
{
    auto* pdf = static_cast <LHAPDF::PDF*> (state);
    return pdf->alphasQ2(q2);
}

int main(int argc, char* argv[])
{
    // set LHAPDF low verbosity
    LHAPDF::setVerbosity(0);

    if (argc != 3)
    {
        std::cerr << "Usage: applcheck PDF-set-name/LHAID applgrid-file\n";
        return 1;
    }

    std::unique_ptr<LHAPDF::PDF> pdf;

    auto* g = pineappl_grid_read(argv[2]);

    int lhaid = -1;

    try
    {
        lhaid = std::stoi(argv[1]);
    }
    catch (...)
    {
    }

    // initialise the PDF set via LHAPDF6
    if (lhaid == -1)
    {
        pdf.reset(LHAPDF::mkPDF(argv[1], 0));
    }
    else
    {
        pdf.reset(LHAPDF::mkPDF(lhaid));
    }

    std::vector<uint32_t> orders(4 * pineappl_grid_order_count(g));
    pineappl_grid_order_params(g, orders.data());

    std::vector<double> bins(pineappl_grid_bin_count(g));
    std::vector<double> bin_sizes(bins.size());
    pineappl_grid_bin_sizes(g, bin_sizes.data());

    std::cout << "\n>>> all bins, all orders:\n\n";

    for (std::size_t i = 0; i != orders.size() / 4; ++i)
    {
        if ((orders.at(4 * i + 2) != 0) || (orders.at(4 * i + 3) != 0))
        {
            continue;
        }

        auto const alphas = orders.at(4 * i + 0);
        auto const alpha = orders.at(4 * i + 1);

        bool* order_mask = new bool[orders.size() / 4]();

        order_mask[i] = true;

        pineappl_grid_convolute(g, ::xfx, ::xfx, ::alphas, pdf.get(),
            order_mask, nullptr, 1.0, 1.0, bins.data());

        delete [] order_mask;

        for (std::size_t j = 0; j != bins.size(); ++j)
        {
            double const diff_xsec = bins.at(j);
            double const inte_xsec = diff_xsec * bin_sizes.at(j);

            std::cout << " bin #" << std::setw(2) << j << ", O(as^" << alphas << " a^" << alpha
                << "): " << std::scientific << std::setw(13) << diff_xsec << " [pb/GeV] or "
                << std::setw(13) << inte_xsec << " [pb]\n";
        }
    }

    pineappl_grid_convolute(g, ::xfx, ::xfx, ::alphas, pdf.get(),
        nullptr, nullptr, 1.0, 1.0, bins.data());

    std::cout << "\n>>> all bins:\n\n";

    double sum = 0.0;

    for (std::size_t i = 0; i != bins.size(); ++i)
    {
        double const diff_xsec = bins.at(i);
        double const inte_xsec = diff_xsec * bin_sizes.at(i);

        std::cout << " bin #" << std::setw(2) << i << ": " << std::scientific << diff_xsec
            << " [pb/GeV] or " << inte_xsec << " [pb]\n";

        sum += inte_xsec;
    }

    std::cout << "\n>>> sum:\n\n " << std::scientific << sum << " [pb]\n";

    pdf.release();

    pineappl_grid_delete(g);
}
