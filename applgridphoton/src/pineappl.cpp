#include "pineappl.h"

#include "appl_grid/appl_grid.h"
#include "appl_grid/lumi_pdf.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <chrono>
#include <string>
#include <unordered_map>
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

struct pineappl_storage
{
    pineappl_storage(char const* method)
        : method(method)
    {
    }

    std::string method;
    std::unordered_map<std::string, bool> bools;
    std::unordered_map<std::string, double> doubles;
    std::unordered_map<std::string, int> ints;
    std::unordered_map<std::string, std::string> strings;
};

pineappl_storage* pineappl_storage_new(char const* method)
{
    auto* result = new pineappl_storage(method);

    if (std::string(method) == "papplgrid_f2")
    {
        // amcblast default values
        pineappl_storage_set_int(result, "nx", 50);
        pineappl_storage_set_double(result, "x_min", 2e-7);
        pineappl_storage_set_double(result, "x_max", 1.0);
        pineappl_storage_set_int(result, "x_order", 3);
        pineappl_storage_set_int(result, "nq2", 30);
        pineappl_storage_set_double(result, "q2_min", 100);
        pineappl_storage_set_double(result, "q2_max", 1000000);
        pineappl_storage_set_int(result, "q2_order", 3);

        pineappl_storage_set_bool(result, "reweight", false);
        pineappl_storage_set_string(result, "documentation", "");
    }
    else
    {
        // TODO: implement other methods
        return nullptr;
    }

    return result;
}

void pineappl_storage_delete(pineappl_storage* storage)
{
    delete storage;
}

void pineappl_storage_set_bool(pineappl_storage* storage, char const* key, bool value)
{
    storage->bools[key] = value;
}

void pineappl_storage_set_double(pineappl_storage* storage, char const* key, double value)
{
    storage->doubles[key] = value;
}

void pineappl_storage_set_int(pineappl_storage* storage, char const* key, int value)
{
    storage->ints[key] = value;
}

void pineappl_storage_set_string(pineappl_storage* storage, char const* key, char const* value)
{
    storage->strings[key] = value;
}

bool pineappl_storage_get_bool(pineappl_storage* storage, char const* key)
{
    return storage->bools.at(key);
}

double pineappl_storage_get_double(pineappl_storage* storage, char const* key)
{
    return storage->doubles.at(key);
}

int pineappl_storage_get_int(pineappl_storage* storage, char const* key)
{
    return storage->ints.at(key);
}

char const* pineappl_storage_get_string(pineappl_storage* storage, char const* key)
{
    return storage->strings.at(key).c_str();
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
    pineappl_storage* storage
) {
    // STEP 1: prepare the vector containing the grid parameters for all grids

    // TODO: other format are currently not implemented
    assert( format == pineappl_subgrid_format::as_a_logxir_logxif );

    // TODO: at the moment only the modified APPLgrid package is supported (`papplgrid`)
    auto const pos = storage->method.find('_');
    assert( storage->method.substr(0, pos) == "papplgrid" );

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

    auto result = new pineappl_grid(appl::grid(n_bins, bin_limits,
        pineappl_storage_get_int(storage, "nq2"),
        pineappl_storage_get_double(storage, "q2_min"),
        pineappl_storage_get_double(storage, "q2_max"),
        pineappl_storage_get_int(storage, "q2_order"),
        pineappl_storage_get_int(storage, "nx"),
        pineappl_storage_get_double(storage, "x_min"),
        pineappl_storage_get_double(storage, "x_max"),
        pineappl_storage_get_int(storage, "x_order"),
        pdf_name, order_ids, storage->method.substr(pos + 1)));

    result->grid.reweight(pineappl_storage_get_bool(storage, "reweight"));
    result->grid.setDocumentation(pineappl_storage_get_string(storage, "documentation"));

    return result;
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
