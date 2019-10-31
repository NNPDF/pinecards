#include <pineappl.h>

#include <catch2/catch.hpp>

#include <array>

void simple_pdf(double const& x, double const&, double* pdf)
{
    std::fill_n(pdf, 14, x);
}

double alphas(double const&)
{
    return 1.0;
}

TEST_CASE("", "")
{
    // create a new and empty luminosity function
    int const lumi_id = pineappl_lumi_new();

    // this is the first time that we call the function, thus the index must be zero
    CHECK( lumi_id == 0 );

    // add a new entry to the luminosity function; the combination `1.0 * (up up)`
    std::array<int, 2> pdg_id_pairs = { 2, 2 };
    pineappl_lumi_add(lumi_id, pdg_id_pairs.size() / 2, pdg_id_pairs.data(), 1.0);

    // we'd like to have a single grid of order alpha^2 - for example LO Drell-Yan
    std::array<int, 4> grid_parameters = { 0, 2, 0, 0 };

    // global grid parameters, not really that important here
    int const    nq2      = 30;
    double const q2_min   = 100;
    double const q2_max   = 1000000;
    int const    q2_order = 1;//3;
    int const    nx       = 50;
    double const x_min    = 2e-7;
    double const x_max    = 1;
    int const    x_order  = 1;//3;

    // a distribution with only one bin
    std::array<double, 2> bin_limits = { 0.0, 1.0 };

    // create a new file
    int file_id = pineappl_file_open(
        bin_limits.size() - 1,
        bin_limits.data(),
        lumi_id,
        pineappl_grid_format::as_a_logmur_logmuf,
        grid_parameters.size() / 4,
        grid_parameters.data(),
        nq2,
        q2_min,
        q2_max,
        q2_order,
        nx,
        x_min,
        x_max,
        x_order,
        "f2"
    );

    // this is the first time that we call the function, thus the index must be zero
    CHECK( file_id == 0 );

    std::array<double, 1> const weights = { 1.0 };

    pineappl_file_fill(file_id, 0.25, 0.25, 10000.0, 0.25, weights.data(), 0);

    double result;

    pineappl_file_convolute(
        file_id,
        simple_pdf,
        simple_pdf,
        alphas,
        nullptr,
        1.0,
        1.0,
        1.0,
        &result
    );

    CHECK( result == 1.0 );
}
