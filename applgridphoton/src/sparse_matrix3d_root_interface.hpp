#ifndef SPARSE_MATRIX_3D_ROOT_INTERFACE_HPP
#define SPARSE_MATRIX_3D_ROOT_INTERFACE_HPP

#include "appl_grid/SparseMatrix3d.h"

#include <TH3D.h>

SparseMatrix3d* sparse_matrix3d_from_th3d(TH3D const& histogram);

TH3D* sparse_matrix3d_to_th3d(SparseMatrix3d const& matrix, std::string const& s);

#endif
