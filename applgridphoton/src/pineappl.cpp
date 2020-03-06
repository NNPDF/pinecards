#include "pineappl.h"

#include "appl_grid/appl_grid.h"
#include "appl_grid/lumi_pdf.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <string>
#include <utility>
#include <vector>

namespace
{

std::string get_unique_id()
{
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::nanoseconds;

    auto const duration = high_resolution_clock::now().time_since_epoch();
    auto const count = duration_cast<nanoseconds>(duration).count();

    return std::to_string(count);
}

}

struct pineappl_lumi
{
    std::vector<std::vector<double>> factors;
    std::vector<std::vector<int>> pdg_ids;
};

pineappl_lumi* pineappl_lumi_new()
{
    return new pineappl_lumi();
}

void pineappl_lumi_delete(pineappl_lumi* lumi)
{
    delete lumi;
}

void pineappl_lumi_add(
    pineappl_lumi* lumi,
    unsigned combinations,
    int* pdg_id_pairs,
    double* factors
) {
    if (lumi == nullptr)
    {
        // TODO: error
    }

    if (pdg_id_pairs == nullptr)
    {
        // TODO: error
    }

    // TODO: non-unity factors are NYI
    assert( std::all_of(factors, factors + combinations, [](double f) { return f == 1.0; }) );

    lumi->factors.emplace_back(std::vector<double>(factors, factors + combinations));
    lumi->pdg_ids.emplace_back(std::vector<int>(pdg_id_pairs, pdg_id_pairs + 2 * combinations));
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
    pineappl_lumi* lumi,
    pineappl_subgrid_format format,
    unsigned subgrids,
    int* subgrid_params,
    int n_bins,
    double const* bin_limits,
    unsigned nx,
    double x_min,
    double x_max,
    unsigned x_order,
    unsigned nq2,
    double q2_min,
    double q2_max,
    unsigned q2_order,
    char const* map
) {
    // STEP 1: prepare the vector containing the grid parameters for all grids

    // TODO: other format are currently not implemented
    assert( format == pineappl_subgrid_format::as_a_logxir_logxif );

    std::vector<appl::order_id> order_ids;

    for (unsigned i = 0; i != subgrids; ++i)
    {
        order_ids.emplace_back(
            subgrid_params[4 * i + 0], // alphas
            subgrid_params[4 * i + 1], // alpha
            subgrid_params[4 * i + 3], // logmuf
            subgrid_params[4 * i + 2]  // logmur
        );
    }

    // STEP 2: prepare the PDF pdg id pairs

    std::string const pdf_name = "pineappl_appl_grid_pdf_bridge_" + get_unique_id();
    std::vector<int> lumi_vector;
    lumi_vector.push_back(lumi->pdg_ids.size());

    for (std::size_t i = 0; i != lumi->pdg_ids.size(); ++i)
    {
        lumi_vector.push_back(i);
        lumi_vector.push_back(lumi->pdg_ids.at(i).size() / 2);

        for (std::size_t j = 0; j != lumi->pdg_ids.at(i).size() / 2; ++j)
        {
            lumi_vector.push_back(lumi->pdg_ids.at(i).at(2 * j + 0));
            lumi_vector.push_back(lumi->pdg_ids.at(i).at(2 * j + 1));
        }
    }

    // the object will be destroyed by APPLgrid (hopefully)
    new lumi_pdf(pdf_name, lumi_vector);

    // STEP 3: create the appl grids

    return new pineappl_grid(appl::grid(n_bins, bin_limits, nq2, q2_min, q2_max, q2_order, nx,
        x_min, x_max, x_order, pdf_name, order_ids, map));
}

pineappl_grid* pineappl_grid_read(char const* filename)
{
    return new pineappl_grid(appl::grid(filename));
}

void pineappl_grid_delete(pineappl_grid* grid)
{
    delete grid;
}

pineappl_subgrid_format pineappl_grid_get_subgrid_format(pineappl_grid* /*grid*/)
{
    // TODO: implement the general case
    return pineappl_subgrid_format::as_a_logxir_logxif;
}

unsigned pineappl_grid_get_subgrids(pineappl_grid* grid)
{
    return grid->grid.order_ids().size();
}

void pineappl_grid_get_subgrid_params(pineappl_grid* grid, int* subgrid_params)
{
    // TODO: implement the general case
    unsigned i = 0;

    for (auto const& order_id : grid->grid.order_ids())
    {
        subgrid_params[i++] = order_id.alphs();
        subgrid_params[i++] = order_id.alpha();
        subgrid_params[i++] = order_id.lmur2();
        subgrid_params[i++] = order_id.lmuf2();
    }
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
