#ifndef PINEAPPL_H
#define PINEAPPL_H

/*
 * PineAPPL - PineAPPL Is Not an Extension of APPLgrid
 * Copyright (C) 2019  Christopher Schwan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifdef __cplusplus
extern "C"
{
#endif

/// Struture that captures the definition of the luminosity function.
struct pineappl_lumi;

/// Creates a new luminosity function and return a pointer to it. If no longer needed, the structure
/// should be deleted using \ref pineappl_lumi_delete.
pineappl_lumi* pineappl_lumi_new(void);

/// Delete a previously created luminosity function using \ref pineappl_file_new.
void pineappl_lumi_delete(pineappl_lumi* lumi);

/// Adds a linear combination of initial states to the luminosity function `lumi`.
void pineappl_lumi_add(pineappl_lumi* lumi, unsigned combinations, int* pdg_id_pairs, double factors);

/// Enumeration that determines the meaning of the entries of the array `grid_parameters` in the
/// function \ref pineappl_file_new.
typedef enum
{
    ///
    as_a_logmur_logmuf = 1,
}
pineappl_grid_format;

///
struct pineappl_grid;

///
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
);

///
void pineappl_grid_delete(pineappl_grid* grid);

///
void pineappl_grid_fill(
    pineappl_grid* grid,
    double x1,
    double x2,
    double q2,
    double observable,
    double const* weight,
    unsigned grid_index
);

///
void pineappl_grid_scale(pineappl_grid* grid, double factor);

///
void pineappl_grid_write(pineappl_grid* grid, char const* filename);

// TODO: remove the `const&`

///
typedef void (*pineappl_func_xfx)(double const& x1, double const& q2, double* pdfs);

///
typedef double (*pineappl_func_alphas)(double const& q2);

///
void pineappl_grid_convolute(
    pineappl_grid* grid,
    pineappl_func_xfx pdf1,
    pineappl_func_xfx pdf2,
    pineappl_func_alphas alphas,
    int* grid_mask,
    double scale_ren,
    double scale_fac,
    double scale_energy,
    double* results
);

#ifdef __cplusplus
}
#endif

#endif
