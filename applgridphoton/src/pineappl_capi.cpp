#include "pineappl_capi.h"

#include "appl_grid/appl_grid.h"
#include "appl_grid/appl_pdf.h"
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

double (*pdf1_ptr)(int32_t pdg_id, double x, double q2, void *state) = nullptr;
double (*pdf2_ptr)(int32_t pdg_id, double x, double q2, void *state) = nullptr;
double (*alphas_ptr)(double q2, void *state) = nullptr;
void* state = nullptr;

double alphas(double const& q)
{
    return alphas_ptr(q * q, state);
}

void xfx1(double const& x, double const& q, double* pdfs)
{
    pdfs[ 0] = pdf1_ptr(-6, x, q * q, state);
    pdfs[ 1] = pdf1_ptr(-5, x, q * q, state);
    pdfs[ 2] = pdf1_ptr(-4, x, q * q, state);
    pdfs[ 3] = pdf1_ptr(-3, x, q * q, state);
    pdfs[ 4] = pdf1_ptr(-2, x, q * q, state);
    pdfs[ 5] = pdf1_ptr(-1, x, q * q, state);
    pdfs[ 6] = pdf1_ptr(21, x, q * q, state);
    pdfs[ 7] = pdf1_ptr( 1, x, q * q, state);
    pdfs[ 8] = pdf1_ptr( 2, x, q * q, state);
    pdfs[ 9] = pdf1_ptr( 3, x, q * q, state);
    pdfs[10] = pdf1_ptr( 4, x, q * q, state);
    pdfs[11] = pdf1_ptr( 5, x, q * q, state);
    pdfs[12] = pdf1_ptr( 6, x, q * q, state);
    pdfs[13] = pdf1_ptr(22, x, q * q, state);
}

void xfx2(double const& x, double const& q, double* pdfs)
{
    pdfs[ 0] = pdf2_ptr(-6, x, q * q, state);
    pdfs[ 1] = pdf2_ptr(-5, x, q * q, state);
    pdfs[ 2] = pdf2_ptr(-4, x, q * q, state);
    pdfs[ 3] = pdf2_ptr(-3, x, q * q, state);
    pdfs[ 4] = pdf2_ptr(-2, x, q * q, state);
    pdfs[ 5] = pdf2_ptr(-1, x, q * q, state);
    pdfs[ 6] = pdf2_ptr(21, x, q * q, state);
    pdfs[ 7] = pdf2_ptr( 1, x, q * q, state);
    pdfs[ 8] = pdf2_ptr( 2, x, q * q, state);
    pdfs[ 9] = pdf2_ptr( 3, x, q * q, state);
    pdfs[10] = pdf2_ptr( 4, x, q * q, state);
    pdfs[11] = pdf2_ptr( 5, x, q * q, state);
    pdfs[12] = pdf2_ptr( 6, x, q * q, state);
    pdfs[13] = pdf2_ptr(22, x, q * q, state);
}

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

pineappl_lumi *pineappl_lumi_new()
{
    return new pineappl_lumi();
}

void pineappl_lumi_delete(pineappl_lumi *lumi)
{
    delete lumi;
}

void pineappl_lumi_add(
    pineappl_lumi *lumi,
    uintptr_t combinations,
    const int32_t *pdg_id_pairs,
    const double *factors
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
    assert( (factors == nullptr) ||
        std::all_of(factors, factors + combinations, [](double f) { return f == 1.0; }) );

    if (factors == nullptr)
    {
        lumi->factors.emplace_back(combinations, 1.0);
    }
    else
    {
        lumi->factors.emplace_back(factors, factors + combinations);
    }

    lumi->pdg_ids.emplace_back(pdg_id_pairs, pdg_id_pairs + 2 * combinations);
}

struct pineappl_keyval
{
    pineappl_keyval() = default;

    std::unordered_map<std::string, bool> bools;
    std::unordered_map<std::string, double> doubles;
    std::unordered_map<std::string, int> ints;
    std::unordered_map<std::string, std::string> strings;
};

pineappl_keyval *pineappl_keyval_new()
{
    pineappl_keyval* result = new pineappl_keyval();

    // amcblast default values
    pineappl_keyval_set_int(result, "nx", 50);
    pineappl_keyval_set_double(result, "x_min", 2e-7);
    pineappl_keyval_set_double(result, "x_max", 1.0);
    pineappl_keyval_set_int(result, "x_order", 3);
    pineappl_keyval_set_int(result, "nq2", 30);
    pineappl_keyval_set_double(result, "q2_min", 100);
    pineappl_keyval_set_double(result, "q2_max", 1000000);
    pineappl_keyval_set_int(result, "q2_order", 3);

    pineappl_keyval_set_bool(result, "reweight", false);
    pineappl_keyval_set_string(result, "documentation", "");

    return result;
}

void pineappl_keyval_delete(pineappl_keyval *key_vals)
{
    delete key_vals;
}

void pineappl_keyval_set_bool(pineappl_keyval *key_vals, const char *key, bool value)
{
    key_vals->bools[key] = value;
}

void pineappl_keyval_set_double(pineappl_keyval *key_vals, const char *key, double value)
{
    key_vals->doubles[key] = value;
}

void pineappl_keyval_set_int(pineappl_keyval *key_vals, const char *key, int32_t value)
{
    key_vals->ints[key] = value;
}

void pineappl_keyval_set_string(pineappl_keyval *key_vals, const char *key, const char *value)
{
    key_vals->strings[key] = value;
}

bool pineappl_keyval_bool(const pineappl_keyval *key_vals, const char *key)
{
    return key_vals->bools.at(key);
}

double pineappl_keyval_double(const pineappl_keyval *key_vals, const char *key)
{
    return key_vals->doubles.at(key);
}

int32_t pineappl_keyval_int(const pineappl_keyval *key_vals, const char *key)
{
    return key_vals->ints.at(key);
}

const char *pineappl_keyval_string(const pineappl_keyval *key_vals, const char *key)
{
    return key_vals->strings.at(key).c_str();
}

struct pineappl_grid
{
    pineappl_grid(appl::grid&& grid, appl::appl_pdf const* lumi)
        : grid(std::move(grid))
        , lumi(lumi)
    {
    }

    appl::grid grid;
    appl::appl_pdf const* lumi;
};

pineappl_grid *pineappl_grid_new(
    const pineappl_lumi *lumi,
    uintptr_t orders,
    const uint32_t *order_params,
    uintptr_t bins,
    const double *bin_limits,
    const pineappl_keyval *key_vals
) {
    // STEP 1: prepare the vector containing the grid parameters for all grids

    std::vector<appl::order_id> order_ids;

    for (unsigned i = 0; i != orders; ++i)
    {
        order_ids.emplace_back(
            order_params[4 * i + 0], // alphas
            order_params[4 * i + 1], // alpha
            order_params[4 * i + 3], // logmuf
            order_params[4 * i + 2]  // logmur
        );
    }

    // STEP 2: prepare the PDF pdg id pairs

    // important: `pdf_name` must have `.config` suffix, other loading will not work
    std::string const pdf_name = "pineappl_appl_grid_pdf_bridge_" + get_unique_id() + ".config";
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
    auto* lumi_ptr = new lumi_pdf(pdf_name, lumi_vector);

    // STEP 3: create the appl grids

    auto result = new pineappl_grid(appl::grid(bins, bin_limits,
        pineappl_keyval_int(key_vals, "nq2"),
        pineappl_keyval_double(key_vals, "q2_min"),
        pineappl_keyval_double(key_vals, "q2_max"),
        pineappl_keyval_int(key_vals, "q2_order"),
        pineappl_keyval_int(key_vals, "nx"),
        pineappl_keyval_double(key_vals, "x_min"),
        pineappl_keyval_double(key_vals, "x_max"),
        pineappl_keyval_int(key_vals, "x_order"),
        pdf_name, order_ids, "f2"), lumi_ptr);

    result->grid.reweight(pineappl_keyval_bool(key_vals, "reweight"));
    result->grid.setDocumentation(pineappl_keyval_string(key_vals, "documentation"));

    return result;
}

pineappl_grid *pineappl_grid_read(const char *filename)
{
    auto grid = appl::grid(filename);
    auto* result = new pineappl_grid(std::move(grid), grid.genpdf(0));

    for (std::size_t i = 1; i < result->grid.order_ids().size(); ++i)
    {
        assert( result->grid.genpdf(i) == result->lumi );
    }

    return result;
}

void pineappl_grid_delete(pineappl_grid *grid)
{
    auto* lumi = grid->lumi;

    // TODO: not sure this order is needed
    delete grid;
    delete lumi;
}

uintptr_t pineappl_grid_order_count(const pineappl_grid *grid)
{
    return grid->grid.order_ids().size();
}

void pineappl_grid_order_params(const pineappl_grid *grid, uint32_t *order_params)
{
    // TODO: implement the general case
    unsigned i = 0;

    for (auto const& order_id : grid->grid.order_ids())
    {
        order_params[i++] = order_id.alphs();
        order_params[i++] = order_id.alpha();
        order_params[i++] = order_id.lmur2();
        order_params[i++] = order_id.lmuf2();
    }
}

void pineappl_grid_fill_all(
    pineappl_grid *grid,
    double x1,
    double x2,
    double q2,
    uintptr_t order,
    double observable,
    const double *weights
) {
    grid->grid.fill_grid(x1, x2, q2, observable, weights, order);
}

void pineappl_grid_fill(
    pineappl_grid * grid,
    double x1,
    double x2,
    double q2,
    uintptr_t order,
    double observable,
    uintptr_t lumi,
    double weight
) {
    // TODO: does `size` do what I think it does?
    std::vector<double> weights(grid->lumi->size());
    weights.at(lumi) = weight;
    grid->grid.fill_grid(x1, x2, q2, observable, weights.data(), order);
}

void pineappl_grid_scale(pineappl_grid *grid, double factor)
{
    grid->grid *= factor;
}

void pineappl_grid_optimize(pineappl_grid *grid)
{
    grid->grid.optimise();
}

void pineappl_grid_write(/*const*/ pineappl_grid *grid, const char *filename)
{
    grid->grid.Write(filename);
}

void pineappl_grid_convolute(
    /*const*/ pineappl_grid *grid,
    double (*xfx1)(int32_t pdg_id, double x, double q2, void *state),
    double (*xfx2)(int32_t pdg_id, double x, double q2, void *state),
    double (*alphas)(double q2, void *state),
    void *state,
    const bool *order_mask,
    const bool *lumi_mask,
    double xi_ren,
    double xi_fac,
    double *results
) {
    pdf1_ptr = xfx1;
    pdf2_ptr = xfx2;
    alphas_ptr = alphas;
    ::state = state;

    assert( lumi_mask == nullptr );

    std::vector<double> result;

    if (order_mask == nullptr)
    {
        result = grid->grid.vconvolute(::xfx1, ::xfx2, ::alphas, 99, xi_ren, xi_fac, 1.0);
    }
    else
    {
        auto const orders = grid->grid.vconvolute_orders(::xfx1, ::xfx2, ::alphas, xi_ren, xi_fac, 1.0);

        result.resize(grid->grid.Nobs_internal());

        for (std::size_t i = 0; i != orders.size(); ++i)
        {
            if (order_mask[i])
            {
                for (int j = 0; j != grid->grid.Nobs_internal(); ++j)
                {
                    result.at(j) = orders.at(i).at(j);
                }
            }
        }
    }

    std::copy(result.begin(), result.end(), results);
}

void pineappl_grid_merge_and_delete(pineappl_grid *grid, pineappl_grid *other)
{
    auto& g = grid->grid;
    auto& o = other->grid;

    bool same_bins =
        (g.Nobs_internal() == o.Nobs_internal()) &&
        (g.obsmin_internal() == o.obsmin_internal()) &&
        (g.obsmax_internal() == o.obsmax_internal());

    if (same_bins)
    {
        for (int i = 0; i != g.Nobs_internal(); ++i)
        {
            if (g.deltaobs_internal(i) != o.deltaobs_internal(i))
            {
                same_bins = false;
            }
        }
    }

    // if we want to merge grids with the same bins, we have to use `+=`
    if (same_bins)
    {
        grid->grid += other->grid;
    }
    else
    {
        std::vector<appl::grid> grids;
        grids.reserve(2);
        grids.emplace_back(std::move(grid->grid));
        grids.emplace_back(std::move(other->grid));

        grid->grid = appl::grid(std::move(grids));
    }

    delete other;
}

uintptr_t pineappl_grid_bin_count(const pineappl_grid *grid)
{
    return grid->grid.Nobs_internal();
}

void pineappl_grid_bin_sizes(const pineappl_grid *grid, double *bin_sizes)
{
    for (int i = 0; i != grid->grid.Nobs_internal(); ++i)
    {
        bin_sizes[i] = grid->grid.deltaobs_internal(i);
    }
}
