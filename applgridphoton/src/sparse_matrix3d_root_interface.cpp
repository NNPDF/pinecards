#include "sparse_matrix3d_root_interface.hpp"

#include "tsparse2d.h"
#include <TH1D.h>

SparseMatrix3d* sparse_matrix3d_from_th3d(TH3D const& histogram)
{
    // TODO: are the following casts neccessary?
    TH1D* hx = (TH1D*)(&histogram)->Project3D("x");
    TH1D* hy = (TH1D*)(&histogram)->Project3D("y");
    TH1D* hz = (TH1D*)(&histogram)->Project3D("z");

    auto* matrix = new SparseMatrix3d(
        hx->GetNbinsX(), hx->GetBinCenter(1), hx->GetBinCenter(hx->GetNbinsX()),
        hy->GetNbinsX(), hy->GetBinCenter(1), hy->GetBinCenter(hy->GetNbinsX()),
        hz->GetNbinsX(), hz->GetBinCenter(1), hz->GetBinCenter(hz->GetNbinsX())
    );

    delete hx;
    delete hy;
    delete hz;

    for (std::size_t i = 0; i < matrix->xaxis().N(); ++i)
    {
        for (std::size_t j = 0; j < matrix->yaxis().N(); ++j)
        {
            for (std::size_t k = 0; k < matrix->zaxis().N(); ++k)
            {
                (*matrix)(i,j,k) = histogram.GetBinContent(i + 1, j + 1, k + 1);
            }
        }
    }

    return matrix;
}
