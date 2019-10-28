#ifndef SPARSE_MATRIX_3D_ROOT_INTERFACE_HPP
#define SPARSE_MATRIX_3D_ROOT_INTERFACE_HPP

#include "SparseMatrix3d.h"

#include <TH3D.h>

SparseMatrix3d* sparse_matrix3d_from_th3d(TH3D const& histogram);

#endif
