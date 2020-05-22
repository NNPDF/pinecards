/*
 * PineAPPL - PineAPPL is not an extension of APPLgrid
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

#ifndef PINEAPPL_H
#define PINEAPPL_H

/* Generated with cbindgen:0.14.1 */

#include <stdbool.h>
#include <stdint.h>

/**
 * Main data structure of `PineAPPL`. This structure contains a `Subgrid` for each `LumiEntry`,
 * bin, and coupling order it was created with.
 */
typedef struct pineappl_grid pineappl_grid;

/**
 * Key-value storage for passing optional information during grid creation with
 * `pineappl_grid_new`.
 */
typedef struct pineappl_keyval pineappl_keyval;

/**
 * Type for defining a luminosity function.
 */
typedef struct pineappl_lumi pineappl_lumi;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * Returns the number of bins in `grid`.
 */
uintptr_t pineappl_grid_bin_count(const pineappl_grid *grid);

/**
 * Stores the bin sizes of `grid` in `bin_sizes`.
 */
void pineappl_grid_bin_sizes(const pineappl_grid *grid, double *bin_sizes);

/**
 * Convolutes the specified grid with the PDFs `xfx1` and `xfx2` and strong coupling `alphas`.
 * These functions must evaluate the PDFs for the given `x` and `q2` and write the results for all
 * partons into `pdfs`. Note that the value must be the PDF for the given `pdg_id` multiplied with
 * `x`. The value of the pointer `state` provided to these functions is the same one given to this
 * function. The parameter `order_mask` must be as long as there are perturbative orders contained
 * in `grid` and is used to selectively disable (`false`) or enable (`true`) individual orders. If
 * `order_mask` is set to `NULL`, all orders are active. The parameter `lumi_mask` can be used
 * similarly, but must be as long as the luminosity function `grid` was created with has entries,
 * or `NULL`. The values `xi_ren` and `xi_fac` can be used to vary the renormalization and
 * factorization from its central value, which corresponds to `1.0`. After convolution of the grid
 * with the PDF the value of the observable for each bin is written into `results`.
 */
void pineappl_grid_convolute(/*const*/ pineappl_grid *grid,
                             double (*xfx1)(int32_t pdg_id, double x, double q2, void *state),
                             double (*xfx2)(int32_t pdg_id, double x, double q2, void *state),
                             double (*alphas)(double q2, void *state),
                             void *state,
                             const bool *order_mask,
                             const bool *lumi_mask,
                             double xi_ren,
                             double xi_fac,
                             double *results);

/**
 * Delete a grid previously created with `pineappl_grid_new`.
 */
void pineappl_grid_delete(pineappl_grid *grid);

/**
 * Performs an operation `name` on `grid` using as input or output parameters `key_vals`. This is
 * used to get access to functions that are otherwise not available through other functions. If
 * the operation was successful, returns `true`. Otherwise, or if the `name` wasn't recognized
 * `false` is returned.
 */
bool pineappl_grid_ext(pineappl_grid *grid, const char *name, pineappl_keyval *key_vals);

/**
 * Fill `grid` for the given momentum fractions `x1` and `x2`, at the scale `q2` for the given
 * value of the `order`, `observable`, and `lumi` with `weight`.
 */
void pineappl_grid_fill(pineappl_grid *grid,
                        double x1,
                        double x2,
                        double q2,
                        uintptr_t order,
                        double observable,
                        uintptr_t lumi,
                        double weight);

/**
 * Fill `grid` for the given momentum fractions `x1` and `x2`, at the scale `q2` for the given
 * value of the `order` and `observable` with `weights`. The parameter of weight must contain a
 * result for entry of the luminosity function the grid was created with.
 */
void pineappl_grid_fill_all(pineappl_grid *grid,
                            double x1,
                            double x2,
                            double q2,
                            uintptr_t order,
                            double observable,
                            const double *weights);

/**
 * Merges `other` into `grid` and subsequently deletes `other`.
 */
void pineappl_grid_merge_and_delete(pineappl_grid *grid, pineappl_grid *other);

/**
 * Creates a new and empty grid. The creation requires four different sets of parameters:
 * - The luminosity function `lumi`: A pointer to the luminosity function that specifies how the
 * cross section should be reconstructed.
 * - Order specification `orders` and `order_params`. Each `PineAPPL` grid contains a number of
 * different perturbative orders, specified by `orders`. The array `order_params` stores the
 * exponent of each perturbative order and must contain 4 integers denoting the exponent of the
 * string coupling, of the electromagnetic coupling, of the logarithm of the renormalization
 * scale, and finally of the logarithm of the factorization scale.
 * - The observable definition `bins` and `bin_limits`. Each `PineAPPL` grid can store observables
 * from a one-dimensional distribution. To this end `bins` specifies how many observables are
 * stored and `bin_limits` must contain `bins + 1` entries denoting the left and right limit for
 * each bin.
 * - More (optional) information can be given in a key-value storage `key_vals`, which might be
 * a null pointer, to signal there are no further parameters that need to be set.
 */
pineappl_grid *pineappl_grid_new(const pineappl_lumi *lumi,
                                 uintptr_t orders,
                                 const uint32_t *order_params,
                                 uintptr_t bins,
                                 const double *bin_limits,
                                 const pineappl_keyval *key_vals);

/**
 * Return the number of orders in `grid`.
 */
uintptr_t pineappl_grid_order_count(const pineappl_grid *grid);

/**
 * Write the order parameters of `grid` into `order_params`.
 */
void pineappl_grid_order_params(const pineappl_grid *grid, uint32_t *order_params);

/**
 * Read a `PineAPPL` grid from a file with name `filename`.
 */
pineappl_grid *pineappl_grid_read(const char *filename);

/**
 * Scale all grids in `grid` by `factor`.
 */
void pineappl_grid_scale(pineappl_grid *grid, double factor);

/**
 * Scales each subgrid by a factor which is the product of the given values `alphas`, `alpha`,
 * `logxir`, and `logxif`, each raised to the corresponding powers for each subgrid. In addition,
 * every subgrid is scaled by a factor `global` independently of its order.
 */
void pineappl_grid_scale_by_order(pineappl_grid *grid,
                                  double alphas,
                                  double alpha,
                                  double logxir,
                                  double logxif,
                                  double global);

/**
 * Write `grid` to a file with name `filename`.
 */
void pineappl_grid_write(/*const*/ pineappl_grid *grid, const char *filename);

/**
 * Get the boolean-valued parameter with name `key` stored in `key_vals`.
 */
bool pineappl_keyval_bool(const pineappl_keyval *key_vals, const char *key);

/**
 * Delete the previously created object pointed to by `key_vals`.
 */
void pineappl_keyval_delete(pineappl_keyval *key_vals);

/**
 * Get the double-valued parameter with name `key` stored in `key_vals`.
 */
double pineappl_keyval_double(const pineappl_keyval *key_vals, const char *key);

/**
 * Get the string-valued parameter with name `key` stored in `key_vals`.
 */
int32_t pineappl_keyval_int(const pineappl_keyval *key_vals, const char *key);

/**
 * Return a pointer to newly-created `pineappl_keyval` object.
 */
pineappl_keyval *pineappl_keyval_new(void);

/**
 * Set the double-valued parameter with name `key` to `value` in `key_vals`.
 */
void pineappl_keyval_set_bool(pineappl_keyval *key_vals, const char *key, bool value);

/**
 * Set the double-valued parameter with name `key` to `value` in `key_vals`.
 */
void pineappl_keyval_set_double(pineappl_keyval *key_vals, const char *key, double value);

/**
 * Set the int-valued parameter with name `key` to `value` in `key_vals`.
 */
void pineappl_keyval_set_int(pineappl_keyval *key_vals, const char *key, int32_t value);

/**
 * Set the string-valued parameter with name `key` to `value` in `key_vals`.
 */
void pineappl_keyval_set_string(pineappl_keyval *key_vals, const char *key, const char *value);

/**
 * Get the int-valued parameter with name `key` stored in `key_vals`.
 */
const char *pineappl_keyval_string(const pineappl_keyval *key_vals, const char *key);

/**
 * Adds a linear combination of initial states to the luminosity function `lumi`.
 */
void pineappl_lumi_add(pineappl_lumi *lumi,
                       uintptr_t combinations,
                       const int32_t *pdg_id_pairs,
                       const double *factors);

/**
 * Delete luminosity function previously created with `pineappl_lumi_new`.
 */
void pineappl_lumi_delete(pineappl_lumi *lumi);

/**
 * Creates a new luminosity function and returns a pointer to it. If no longer needed, the object
 * should be deleted using `pineappl_lumi_delete`.
 */
pineappl_lumi *pineappl_lumi_new(void);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif /* PINEAPPL_H */
