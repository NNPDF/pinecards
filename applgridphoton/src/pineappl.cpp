#include "pineappl.h"

#include "appl_grid/appl_grid.h"
#include "appl_grid/lumi_pdf.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <string>
#include <utility>
#include <vector>

struct pineappl_lumi
{
    std::vector<double> factors;
    std::vector<std::vector<int>> combinations;
};

pineappl_lumi* pineappl_lumi_new()
{
    return new pineappl_lumi();
}

void pineappl_lumi_delete(pineappl_lumi* lumi)
{
    delete lumi;
}

void pineappl_lumi_add(pineappl_lumi* lumi, unsigned combinations, int* pdg_id_pairs, double factor)
{
    if (lumi == nullptr)
    {
        // TODO: error
    }

    if (pdg_id_pairs == nullptr)
    {
        // TODO: error
    }

    // TODO: non-unity factors are NYI
    assert( factor == 1.0 );

    lumi->factors.push_back(factor);
    lumi->combinations.emplace_back(std::vector<int>(pdg_id_pairs, pdg_id_pairs + 2 * combinations));
}

struct pineappl_grid
{
    pineappl_grid(appl::grid&& grid)
        : grid(std::move(grid))
    {
    }

    appl::grid grid;
};

pineappl_grid* pineappl_grid_new(
    int n_bins,
    double const* bin_limits,
    pineappl_lumi* lumi,
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

    std::string const pdf_name = "pineappl_appl_grid_pdf_bridge_0"; // TODO: add unique identifier
    std::vector<int> lumi_vector;
    lumi_vector.push_back(lumi->combinations.size());

    for (std::size_t i = 0; i != lumi->combinations.size(); ++i)
    {
        lumi_vector.push_back(i);
        lumi_vector.push_back(lumi->combinations.at(i).size() / 2);

        for (std::size_t j = 0; j != lumi->combinations.at(i).size() / 2; ++j)
        {
            lumi_vector.push_back(lumi->combinations.at(i).at(2 * j + 0));
            lumi_vector.push_back(lumi->combinations.at(i).at(2 * j + 1));
        }
    }

    // the object will be destroyed by APPLgrid (hopefully)
    new lumi_pdf(pdf_name, lumi_vector);

    // STEP 3: create the appl grids

    return new pineappl_grid(appl::grid(n_bins, bin_limits, nq2, q2_min, q2_max, q2_order, nx,
        x_min, x_max, x_order, pdf_name, order_ids, map));
}

void pineappl_grid_delete(pineappl_grid* grid)
{
    delete grid;
}

void pineappl_grid_fill(
    pineappl_grid* grid,
    double x1,
    double x2,
    double q2,
    double observable,
    double const* weight,
    unsigned grid_index
) {
    grid->grid.fill_grid(x1, x2, q2, observable, weight, grid_index);
}

void pineappl_grid_scale(pineappl_grid* grid, double factor)
{
    grid->grid *= factor;
}

void pineappl_grid_write(pineappl_grid* grid, char const* filename)
{
    grid->grid.Write(filename);
}

void pineappl_grid_convolute(
    pineappl_grid* grid,
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

    auto const result = grid->grid.vconvolute(pdf1, pdf2, alphas, 99, scale_ren,
        scale_fac, scale_energy);

    std::copy(result.begin(), result.end(), bins);
}
