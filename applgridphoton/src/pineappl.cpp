#include "pineappl.h"

#include "appl_grid/appl_grid.h"
#include "appl_grid/lumi_pdf.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <string>
#include <vector>

namespace
{

struct lumi_entry
{
    lumi_entry(double factor, std::vector<int> const& combinations)
        : factor{factor}
        , combinations{combinations}
    {
    }

    double factor;
    std::vector<int> combinations;
};

std::vector<std::vector<lumi_entry>>& luminosities()
{
    static std::vector<std::vector<lumi_entry>> luminosities_;
    return luminosities_;
}

std::vector<appl::grid>& appl_grids()
{
    static std::vector<appl::grid> appl_grids_;
    return appl_grids_;
}

}

int pineappl_lumi_new()
{
    int const index = luminosities().size();
    luminosities().emplace_back();
    return index;
}

void pineappl_lumi_add(int lumi, unsigned combinations, int* pdg_id_pairs, double factor)
{
    if ((lumi < 0) || (static_cast <unsigned> (lumi) >= luminosities().size()))
    {
        // TODO: error
    }

    if (pdg_id_pairs == nullptr)
    {
        // TODO: error
    }

    // TODO: non-unity factors are NYI
    assert( factor == 1.0 );

    luminosities().at(lumi).emplace_back(factor,
        std::vector<int>(pdg_id_pairs, pdg_id_pairs + 2 * combinations));
}

int pineappl_file_open(
    int n_bins,
    double const* bin_limits,
    int lumi_id,
    pineappl_grid_format format,
    int grids,
    int* grid_parameters,
    unsigned nq2,
    double q2_min,
    double q2_max,
    unsigned q2_order,
    unsigned nx,
    double x_min,
    double x_max,
    unsigned x_order,
    char const* map
) {
    int const index = appl_grids().size();

    // STEP 1: prepare the vector containing the grid parameters for all grids

    // TODO: other format are currently not implemented
    assert( format == pineappl_grid_format::as_a_logmur_logmuf );

    std::vector<appl::order_id> order_ids;

    for (int i = 0; i != grids; ++i)
    {
        order_ids.emplace_back(
            grid_parameters[4 * i + 0], // alphas
            grid_parameters[4 * i + 1], // alpha
            grid_parameters[4 * i + 3], // logmuf
            grid_parameters[4 * i + 2]  // logmur
        );
    }

    // STEP 2: prepare the PDF combinations

    std::string const pdf_name = "pineappl_appl_grid_pdf_bridge_" + std::to_string(index);
    std::vector<int> lumi_vector;
    lumi_vector.push_back(luminosities().at(lumi_id).size());

    for (std::size_t i = 0; i != luminosities().at(lumi_id).size(); ++i)
    {
        lumi_vector.push_back(i);
        lumi_vector.push_back(luminosities().at(lumi_id).at(i).combinations.size() / 2);

        for (std::size_t j = 0; j != luminosities().at(lumi_id).at(i).combinations.size() / 2; ++j)
        {
            lumi_vector.push_back(luminosities().at(lumi_id).at(i).combinations.at(2 * j + 0));
            lumi_vector.push_back(luminosities().at(lumi_id).at(i).combinations.at(2 * j + 1));
        }
    }

    // the object will be destroyed by APPLgrid (hopefully)
    new lumi_pdf(pdf_name, lumi_vector);

    // STEP 3: create the appl grids

    appl_grids().emplace_back(n_bins, bin_limits, nq2, q2_min, q2_max, q2_order, nx, x_min, x_max,
        x_order, pdf_name, order_ids, map);

    return index;
}

void pineappl_file_close(int /*file_id*/)
{
    // TODO: deallocate memory
}

void pineappl_file_fill(
    int file_id,
    double x1,
    double x2,
    double q2,
    double observable,
    double const* weight,
    unsigned grid_index
) {
    appl_grids().at(file_id).fill_grid(x1, x2, q2, observable, weight, grid_index);
}

void pineappl_file_scale(int file_id, double factor)
{
    appl_grids().at(file_id) *= factor;
}

void pineappl_file_write(int file_id, char const* filename)
{
    appl_grids().at(file_id).Write(filename);
}

void pineappl_file_convolute(
    int file_id,
    pineappl_func_xfx pdf1,
    pineappl_func_xfx pdf2,
    pineappl_func_alphas alphas,
    int* grid_mask,
    double scale_ren,
    double scale_fac,
    double scale_energy,
    double* bins
) {
    // TODO: selecting and deselecting grids is not completely possible with APPLgrid
    assert( grid_mask == nullptr );

    auto const result = appl_grids().at(file_id).vconvolute(pdf1, pdf2, alphas, 99, scale_ren,
        scale_fac, scale_energy);

    std::copy(result.begin(), result.end(), bins);
}
