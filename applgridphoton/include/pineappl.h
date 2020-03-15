#ifndef PINEAPPL_H
#define PINEAPPL_H

/*
 * PineAPPL - PineAPPL Is Not an Extension of APPLgrid
 * Copyright (C) 2019-2020  Christopher Schwan
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

/// @struct pineappl_lumi
/// Struture capturing the definition of a luminosity function.
struct pineappl_lumi;

/// Creates a new luminosity function and returns a pointer to it. If no longer needed, the object
/// should be deleted using @ref pineappl_lumi_delete.
pineappl_lumi* pineappl_lumi_new(void);

/// Delete luminosity function previously created with @ref pineappl_lumi_new.
void pineappl_lumi_delete(pineappl_lumi* lumi);

/// Adds a linear combination of initial states to the luminosity function `lumi`.
void pineappl_lumi_add(
    pineappl_lumi* lumi,
    unsigned combinations,
    int* pdg_id_pairs,
    double* factors
);

/// Enumeration that determines the meaning of the entries of the array `subgrid_params` in the
/// function @ref pineappl_grid_new. This is important only for @ref pineappl_grid_convolute, which
/// reconstructs the hadronic cross section.
typedef enum
{
    /// For each grid, this requires `subgrid_params` be a tuple of four integers \f$ (a, b, c, d)
    /// \f$, denoting the subgrid's 1) strong coupling power, \f$ a \f$, 2) the electromagnetic
    /// coupling power, \f$ b \f$, 3) the power of the renormalisation scale logarithm, \f$ c \f$,
    /// and finally, 4) the power of the factorisation scale logarithm, \f$ d \f$. For each bin \f$
    /// w_i \f$, @ref pineappl_grid_convolute reconstructs the cross section as follows:
    /// \f[
    ///     \sigma = \left( \frac{\alpha_\mathrm{s}}{2 \pi} \right)^a
    ///              \left( \alpha \right)^b
    ///              \log^c \left( \xi_\mathrm{R} \right)
    ///              \log^d \left( \xi_\mathrm{F} \right) w_i
    /// \f]
    as_a_logxir_logxif = 1
}
pineappl_subgrid_format;

/// @struct pineappl_storage
/// Struct holding a definition how the events \f$ ( x_1, x_2, Q^2 ) \mapsto w_i \f$ are stored in
/// memory.
struct pineappl_storage;

/// Return a pointer to newly-created @ref pineappl_storage object. The parameter method determines
/// the storage layout.
pineappl_storage* pineappl_storage_new(char const* method);

/// Delete the previously created object pointed to by `storage`.
void pineappl_storage_delete(pineappl_storage* storage);

/// Set the double-valued parameter with name `key` to `value` for `storage`.
void pineappl_storage_set_bool(pineappl_storage* storage, char const* key, bool value);

/// Set the double-valued parameter with name `key` to `value` for `storage`.
void pineappl_storage_set_double(pineappl_storage* storage, char const* key, double value);

/// Set the int-valued parameter with name `key` to `value` for `storage`.
void pineappl_storage_set_int(pineappl_storage* storage, char const* key, int value);

/// Set the string-valued parameter with name `key` to `value` for `storage`.
void pineappl_storage_set_string(pineappl_storage* storage, char const* key, char const* value);

/// Get the boolean-valued parameter with name `key` for `storage`.
bool pineappl_storage_get_bool(pineappl_storage* storage, char const* key);

/// Get the double-valued parameter with name `key` for `storage`.
double pineappl_storage_get_double(pineappl_storage* storage, char const* key);

/// Get the string-valued parameter with name `key` for `storage`.
int pineappl_storage_get_int(pineappl_storage* storage, char const* key);

/// Get the int-valued parameter with name `key` for `storage`.
char const* pineappl_storage_get_string(pineappl_storage* storage, char const* key);

/// @struct pineappl_grid
/// Structure representing a PineAPPL grid.
struct pineappl_grid;

/// Create a new @ref pineappl_grid. The creation requires four different sets of parameters:
/// - Luminosity function: `lumi`. This parameter must be a previously created luminosity function,
///   see @ref pineappl_lumi_new.
/// - Subgrid specification: `format`, `subgrids`, and `subgrids_params`. Each PineAPPL grid
///   contains a number of subgrids, given as `subgrids`, which usually store the grids for each
///   perturbative order separately. The concrete meaning of the subgrids is determined by `format`
///   and `subgrids_params`.
/// - Observable definition: `bins` and `bin_limits`. Each subgrid can store observables from a
///   one-dimensional distribution. To this end `bins` specifies how many observables are stored and
///   `bin_limits` gives the boundaries for all of them. The parameter `bin_limits` must be an array
///   with `bins + 1` entries.
/// - Storage information: `storage`.
pineappl_grid* pineappl_grid_new(
    pineappl_lumi* lumi,
    pineappl_subgrid_format format,
    unsigned subgrids,
    int* subgrid_params,
    int bins,
    double const* bin_limits,
    pineappl_storage* storage
);

/// Read a @ref pineappl_grid from a file with name `filename`.
pineappl_grid* pineappl_grid_read(char const* filename);

/// Delete a grid previously created with @ref pineappl_grid_new.
void pineappl_grid_delete(pineappl_grid* grid);

/// Return the @pineappl_subgrid_format that `grid` was created with.
pineappl_subgrid_format pineappl_grid_get_subgrid_format(pineappl_grid* grid);

/// Return the number of subgrids in `grid`.
unsigned pineappl_grid_get_subgrids(pineappl_grid* grid);

/// Write the subgrid parameters of `grid` into `subgrid_params`.
void pineappl_grid_get_subgrid_params(pineappl_grid* grid, int* subgrid_params);

/// Fill `grid` at the position specified with `x1`, `x2`, and `q2`. The array `weight` must be as
/// long the corresponding luminosity function the grid was created with and contain the
/// corresponding weights at each index. The value `grid_index` selects one of the subgrids whose
/// meaning was specified during creation with `grid_parameters` in @ref pineappl_grid_new.
void pineappl_grid_fill(
    pineappl_grid* grid,
    double x1,
    double x2,
    double q2,
    double observable,
    double const* weight,
    unsigned grid_index
);

/// Scale all grids in `grid` by `factor`.
void pineappl_grid_scale(pineappl_grid* grid, double factor);

/// Write `grid` to a file with name `filename`.
void pineappl_grid_write(pineappl_grid* grid, char const* filename);

/// Function pointer type for PDFs \f$ f_a(x, Q^2) \f$. The result of the PDFs must be written into
/// `pdfs`, which must be an array of size 14. The value of the photon PDF must be written to the
/// last element of `pdfs`. Quarks PDF values must be written into `pdfs[i]`, where `i` is
/// calculated as follows: \f$ i = \mathrm{PDG value} + 6 \f$. The value of the gluon PDF must be
/// written in between the quark PDFs, at index `6`.
typedef void (*pineappl_func_xfx)(double x, double q2, double* pdfs, void* state);

/// Function pointer type for the evaluation of the strong coupling.
typedef double (*pineappl_func_alphas)(double q2, void* state);

/// Perform a convolution of the APPLgrid given as `grid` with the PDFs `pdf1` and `pdf2`. The
/// value `grid_mask` must either be `NULL` or an array as long as the luminosity function, which
/// can be used to selectively include (set the corresponding value in `grid_mask` to anything
/// non-zero) or exclude (zero) partonic initial states. The factors `scale_ren`, `scale_fac`, and
/// `scale_energy` can be used to scale the renormalisation scale, the factorisation scale, and the
/// energy from the their default (the value the grid was generated with) value. Results must be an
/// array as large as there are bins, and will contain the cross sections for each bin.
void pineappl_grid_convolute(
    pineappl_grid* grid,
    pineappl_func_xfx pdf1,
    pineappl_func_xfx pdf2,
    pineappl_func_alphas alphas,
    void* state,
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
