#include <pineappl_capi.h>

#include <algorithm>
#include <array>

#include <catch2/catch.hpp>

TEST_CASE("check read and write functions", "[files]")
{
    // create a new and empty luminosity function
    auto* lumi = pineappl_lumi_new();

    // add a new entry to the luminosity function; the combination `1.0 * (up up)`
    std::array<int32_t, 2> pdg_id_pairs = { 2, 2 };
    std::array<double, 1> factors = { 1.0 };
    pineappl_lumi_add(lumi, pdg_id_pairs.size() / 2, pdg_id_pairs.data(), factors.data());

    // we'd like to have a single grid of order alpha^2 - for example LO Drell-Yan
    std::array<uint32_t, 4> grid_parameters = { 0, 2, 0, 0 };

    // global grid parameters, not really that important here
    auto* key_vals = pineappl_keyval_new();

    pineappl_keyval_set_int(key_vals, "q2_order", 1);
    pineappl_keyval_set_int(key_vals, "x_order", 1);

    // a distribution with only one bin
    std::array<double, 2> bin_limits = { 0.0, 1.0 };

    // create a new file
    auto* grid = pineappl_grid_new(
        lumi,
        grid_parameters.size() / 4,
        grid_parameters.data(),
        bin_limits.size() - 1,
        bin_limits.data(),
        key_vals
    );

    pineappl_keyval_delete(key_vals);

    // delete luminosity function
    pineappl_lumi_delete(lumi);

    std::array<double, 1> const weights = { 1.0 };

    pineappl_grid_fill_all(grid, 0.25, 0.25, 10000.0, 0, 0.25, weights.data());

    pineappl_grid_write(grid, "test");
    pineappl_grid_delete(grid);

    grid = pineappl_grid_read("test");

    double result;

    pineappl_grid_convolute(
        grid,
        [](int32_t /*id*/, double x, double /*q2*/, void* /*state*/) { return x; },
        [](int32_t /*id*/, double x, double /*q2*/, void* /*state*/) { return x; },
        [](double /*q2*/, void* /*state*/) { return 1.0; },
        nullptr,
        nullptr,
        nullptr,
        1.0,
        1.0,
        &result
    );

    CHECK( result == 1.0 );

    pineappl_grid_delete(grid);
}
